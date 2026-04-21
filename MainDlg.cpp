// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <atlconv.h>
#include <io.h>
#include <vector>
#include "Plugin.h"
#include "MainDlg.h"
#include "VsUtility.h"
#include "VsInterface.h"

extern CMainDlg* m_pMainDlg;
static const UINT WM_IMPORT_OBJ = WM_APP + 201;
static const char* kImportFieldPath = "/scene/model/tif_obj";

static std::vector<std::string> SplitArgs(const std::string& args)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < args.size(); ++i) {
        const char ch = args[i];
        if (ch == '"') {
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes && (ch == ' ' || ch == '\t')) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        current.push_back(ch);
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

static std::string FindObjPathFromArgs(const std::string& args)
{
    const std::vector<std::string> tokens = SplitArgs(args);
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        if (token.size() >= 4) {
            const std::string ext = token.substr(token.size() - 4);
            if (_stricmp(ext.c_str(), ".obj") == 0) {
                return token;
            }
        }
    }
    return std::string();
}

static bool WaitForFileGenerated(const std::string& filePath, DWORD timeoutMs)
{
    const DWORD intervalMs = 200;
    DWORD elapsed = 0;

    while (elapsed <= timeoutMs) {
        if (_access(filePath.c_str(), 0) == 0) {
            return true;
        }
        ::Sleep(intervalMs);
        elapsed += intervalMs;
    }

    return false;
}

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CFxPluginDlg) 

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CFxPluginDlg(CMainDlg::IDD, pParent)
	, m_exePath(_T(""))
	, m_arguments(_T(""))
	, m_statusText(_T("就绪"))
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CFxPluginDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EXE_PATH, m_exePath);
	DDX_Text(pDX, IDC_EDIT_ARGUMENTS, m_arguments);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_statusText);
}


BEGIN_MESSAGE_MAP(CMainDlg, CFxPluginDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EXECUTE, &CMainDlg::OnBnClickedButtonExecute)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CMainDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_KILL, &CMainDlg::OnBnClickedButtonKill)
	ON_MESSAGE(WM_IMPORT_OBJ, &CMainDlg::OnImportObjMessage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CMainDlg 消息处理程序
//////////////////////////////////////////////////////////////////////////
void CMainDlg::OnDestroy()
{	
    if (m_pMainDlg == this) {
        m_pMainDlg = NULL;
    }

	CFxPluginDlg::OnDestroy();
	// TODO: 在此处添加消息处理程序代码

}

BOOL CMainDlg::OnInitDialog()
{
	CFxPluginDlg::OnInitDialog();

	//初始化界面主控件
	// 设置默认的 EXE 路径
	m_exePath = _T("");
	m_arguments = _T("");

	UpdateData(FALSE);
	
	//调整控件的布局
	AdjustLayout();
	return TRUE; 
}

BOOL CMainDlg::AdjustLayout()
{
	if (!CFxPluginDlg::AdjustLayout()) return FALSE;
	
	CRect rectClient;
	GetClientRect(rectClient);


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//自定义对话框函数
//////////////////////////////////////////////////////////////////////////
void CMainDlg::InitDlgData(LPARAM lParam)
{

}

void CMainDlg::UpdateDlgData(LPARAM lParam)
{

}
void CMainDlg::DestroyDlgData(LPARAM lParam)
{

}

//////////////////////////////////////////////////////////////////////////
//其他函数
//////////////////////////////////////////////////////////////////////////


void CMainDlg::OnBnClickedButtonExecute()
{
    const HWND hwndDlg = m_hWnd;
    UpdateData(TRUE);

    if (m_exePath.IsEmpty()) {
        MessageBox(_T("请输入 EXE 文件路径"), _T("提示"));
        return;
    }

    // 转换为 std::string
    CT2A exePathA(m_exePath);
    CT2A argumentsA(m_arguments);
    std::string exePath((LPCSTR)exePathA);
    std::string arguments((LPCSTR)argumentsA);

    m_statusText = _T("正在执行并等待生成OBJ...");
    UpdateData(FALSE);

    const std::string objPath = FindObjPathFromArgs(arguments);
    if (objPath.empty()) {
        m_statusText = _T("参数中未找到 .obj 输出路径");
        UpdateData(FALSE);
        MessageBox(_T("请在参数中包含 OBJ 输出文件路径（如 \"D:\\\\out\\\\model.obj\"）"), _T("提示"));
        return;
    }

    // 执行 EXE（等待完成）
    if (m_executor.ExecuteExe(exePath, arguments, true)) {
        if (!WaitForFileGenerated(objPath, 30000)) {
            CString objPathMsg = CA2T(objPath.c_str());
            m_statusText = _T("EXE已结束，但OBJ未生成: ") + objPathMsg;
            UpdateData(FALSE);
            return;
        }

        // 避免在按钮回调栈内直接导入，改为投递消息异步导入，规避3dgs内部重入导致的崩溃。
        m_pendingObjPath = CA2T(objPath.c_str());
        ::SetDlgItemText(hwndDlg, IDC_STATIC_STATUS, _T("OBJ已生成，正在导入..."));
        PostMessage(WM_IMPORT_OBJ, 0, 0);
        return;
    }
    else {
        CString errorMsg = CA2T(m_executor.GetLastError().c_str());
        m_statusText = _T("执行失败: ") + errorMsg;
    }

    UpdateData(FALSE);
}

void CMainDlg::OnBnClickedButtonBrowse()
{
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[MAX_PATH] = { 0 };

    _tcscpy_s(szFile, MAX_PATH, m_exePath);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(TCHAR);
    ofn.lpstrFilter = _T("可执行文件 (*.exe)\0*.exe\0所有文件 (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        m_exePath = ofn.lpstrFile;
        UpdateData(FALSE);
    }
}

void CMainDlg::OnBnClickedButtonKill()
{
    if (m_executor.TerminateProcess()) {
        m_statusText = _T("进程已终止");
    }
    else {
        m_statusText = _T("终止失败或进程已完成");
    }
    UpdateData(FALSE);
}

void CMainDlg::ExecuteExe()
{
    OnBnClickedButtonExecute();
}

LRESULT CMainDlg::OnImportObjMessage(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    VS_KEY fieldKey = VsOpenField(kImportFieldPath);
    if (fieldKey == INVALID_KEY) {
        fieldKey = VsCreateField(kImportFieldPath);
    }

    if (fieldKey == INVALID_KEY) {
        if (::IsWindow(m_hWnd)) {
            ::SetDlgItemText(m_hWnd, IDC_STATIC_STATUS, _T("OBJ导入失败: 无法打开Field"));
        }
        return 0;
    }

    CT2A objPathA(m_pendingObjPath);    
    const BOOL importOk = VsUtReadObjFile((LPCSTR)objPathA);
    VsCloseField();

    if (::IsWindow(m_hWnd)) {
        ::SetDlgItemText(m_hWnd, IDC_STATIC_STATUS, importOk ? _T("OBJ导入成功") : _T("OBJ已生成，但导入失败"));
    }

    UpdateData(FALSE);
    return 0;
}

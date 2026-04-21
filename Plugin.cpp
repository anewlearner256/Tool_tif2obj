// MyPlugin.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Plugin.h"
#include "resource.h"


#include "MainDlg.h"

//***************************************************
//********** 1. PLUGIN 数据定义  ********************
//***************************************************
CMyPluginApp theApp;
CMainDlg* m_pMainDlg = NULL;

// 插件主入口函数
FXSDK_API_C void funcShowUI(FX_PTR dwData = NULL);

// 执行 EXE 函数
FXSDK_API_C void funcExecuteExe(FX_PTR dwData = NULL);
FXSDK_API_C void funcMain(FX_PTR dwData = NULL);
FXSDK_API_C void funcAbout(FX_PTR dwData = NULL);
LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam);


//***************************************************
//********** 2. PLUGIN 加载接口  ********************
//***************************************************
FXSDK_API int FxPluginDLL_Init(void* context) {
	
	PluginInfo* dll = (PluginInfo*)context;
	dll->Init();

	dll->SetProperty("Author", "HZY"); //插件作者
	dll->SetProperty("Summary","TIFFToOBGPlugin");    //插件名称
	dll->SetProperty("Description", "This is a test plugin for tiff convert to obj"); //插件描述
	dll->SetProperty("Version", 100);    //插件版本号
	dll->SetProperty("ClassID", (DWORD_PTR)&PLUGIN_CLASS_ID);  //插件ID 
	dll->SetProperty("dllMessageProc", (DWORD_PTR)messageProc);  //插件消息响应函数  
	dll->SetProperty("dllFilterMsgProc", (DWORD_PTR)FxLib::Thread::FxFilterDllMsg);  //Windows消息响应函数 

	// 获取资源句柄
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HMODULE hModule = AfxGetResourceHandle();
	HICON hIcon = (HICON)::LoadImage(hModule, MAKEINTRESOURCE(IDI_ICON_COMPONENT),
		IMAGE_ICON, 0, 0, 0);

	// 添加菜单命令
	dll->AddCmd(funcExecuteExe, "执行程序", true, true, hIcon);
	dll->AddCmd(funcAbout, "关于", false, true, hIcon);


	dll->AddCmd(funcMain, "TIFF转OBJ主窗口");
	FXDLL_ADDABOUT;

	return 1;
}

// ==================== 插件命令 ====================

void funcExecuteExe(FX_PTR dwData)
{
	FX_DECLARE_RESOURCE_DLL

	if (m_pMainDlg == NULL) {
		PwPositionType type = PwPosRight;
		if (!FxPluginCreateDialog(m_pMainDlg, type, TRUE, IDI_ICON1) || m_pMainDlg == NULL) {
			return;
		}
	}

	m_pMainDlg->ExecuteExe();
}



//***************************************************
//********** 3. PLUGIN 其他  ************************
//***************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CMyPluginApp, CMFCPluginApp)
END_MESSAGE_MAP()

// CMyPluginApp 构造
CMyPluginApp::CMyPluginApp()
{
	// TODO: 在此处添加构造代码，
	
}

// CMyPluginApp 初始化
BOOL CMyPluginApp::InitInstance()
{
	CMFCPluginApp::InitInstance();
	// TODO: 在此添加专用代码和/或调用基类
	m_pMainDlg = NULL;

	return TRUE;
}

int CMyPluginApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CMFCPluginApp::ExitInstance();
}

//***************************************************
//********** 4. PLUGIN 菜单命令  ********************
//***************************************************

void funcMain(FX_PTR dwData)
{
	FX_DECLARE_RESOURCE_DLL

	PwPositionType type = PwPosRight;
	FxPluginCreateDialog(m_pMainDlg, type, TRUE, IDI_ICON1);

}

void funcAbout(FX_PTR dwData)
{
	FX_DECLARE_RESOURCE_DLL
	FxPluginShowAboutDlg("TIFF转OBJ功能插件");
}

//***************************************************
//********** 5. 接收主程序的插件消息  ***************
//***************************************************

LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
		FX_DECLARE_RESOURCE_DLL
	theApp.messageProc(message, wParam, lParam);
		
	if(message == PEM_PLUGIN_FUNC) {
		//插件加载后消息
		if(wParam == PluginFunc_Init) {
			
		}
		//插件卸载前消息
		else if(wParam == PluginFunc_Destroy ) {
			
		}
	}
	//三维窗口关闭消息
	else if (message == PEO_MAIN_MSG && wParam == MAINMSG_ViewDestroy) { 

	}

	return 0;
}

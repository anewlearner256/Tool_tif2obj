#pragma once
#include "PluginCore/FxPluginDlg.h"
#include "ExeExecutor.h"
#include "VsUtility.h"

// CMainDlg 对话框
class CMainDlg : public CFxPluginDlg
{
	DECLARE_DYNAMIC(CMainDlg)

// 构造
public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();
	enum { IDD = IDD_MAINDLG };


	/**
	 * @brief 执行 EXE
	 */
	void ExecuteExe();

// 属性
protected:	
	long m_key;
	std::list<long> m_Keys;

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL AdjustLayout();

	afx_msg void OnDestroy();

	afx_msg void OnBnClickedButtonExecute();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonKill();
	afx_msg LRESULT OnImportObjMessage(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()

	
public:
	void InitDlgData(LPARAM lParam = 0); //初始化数据
	void UpdateDlgData(LPARAM lParam = 0); //更新数据
	void DestroyDlgData(LPARAM lParam = 0);//销毁数据

private:
	ExeExecutor m_executor;
	CString m_exePath;
	CString m_arguments;
	CString m_statusText;
	CString m_pendingObjPath;
};

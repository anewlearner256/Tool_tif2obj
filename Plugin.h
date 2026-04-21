// MyPlugin.h : tif2obj DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "PluginCore\FxPluginWrapper.h"
#include "PluginCore\FxPluginApp.h"
#include "UtilCore\Kernel\FxModuleState.h"

#define PLUGIN_CLASS_ID sPluginClassID(0xa815313d, 0xb63e498b)

// CMyPluginApp
// 有关此类实现的信息，请参阅 Plugin.cpp
//
class CMyPluginApp : public CMFCPluginApp
{
public:
	CMyPluginApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
	
};

extern CMyPluginApp theApp;


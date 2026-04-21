#include "StdAfx.h"
#include "ExeExecutor.h"
#include <io.h>
#include <sstream>

ExeExecutor::ExeExecutor()
    : m_processRunning(false)
{
    ZeroMemory(&m_processInfo, sizeof(m_processInfo));
}

ExeExecutor::~ExeExecutor()
{
    if (m_processRunning) {
        TerminateProcess();
    }

    if (m_processInfo.hProcess != NULL) {
        CloseHandle(m_processInfo.hProcess);
    }
    if (m_processInfo.hThread != NULL) {
        CloseHandle(m_processInfo.hThread);
    }
}

bool ExeExecutor::ExecuteExe(const std::string& exePath,
    const std::string& arguments,
    bool waitForCompletion)
{
    ClearLastError();

    // 检查 EXE 文件是否存在
    if (_access(exePath.c_str(), 0) == -1) {
        SetLastError("文件不存在: " + exePath);
        return false;
    }

    // 构建完整的命令行
    std::string cmdLine = exePath;
    if (!arguments.empty()) {
        cmdLine += " " + arguments;
    }

    // 转换为宽字符
    int wLen = MultiByteToWideChar(CP_ACP, 0, cmdLine.c_str(), -1, NULL, 0);
    wchar_t* wCmdLine = new wchar_t[wLen];
    MultiByteToWideChar(CP_ACP, 0, cmdLine.c_str(), -1, wCmdLine, wLen);

    // 创建进程
    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    if (!CreateProcessW(
        NULL,                    // 应用程序名称
        wCmdLine,                // 命令行
        NULL,                    // 进程安全属性
        NULL,                    // 线程安全属性
        FALSE,                   // 是否继承句柄
        0,                       // 创建标志
        NULL,                    // 环境变量
        NULL,                    // 当前目录
        &startupInfo,            // 启动信息
        &m_processInfo           // 返回的进程信息
    )) {
        delete[] wCmdLine;
        DWORD dwError = ::GetLastError();
        std::stringstream ss;
        ss << "创建进程失败，错误码: " << dwError;
        SetLastError(ss.str());
        return false;
    }

    delete[] wCmdLine;
    m_processRunning = true;

    if (waitForCompletion) {
        WaitForCompletion();
    }

    return true;
}

bool ExeExecutor::WaitForCompletion(DWORD timeoutMs)
{
    if (!m_processRunning) {
        return false;
    }

    DWORD dwWaitResult = WaitForSingleObject(m_processInfo.hProcess,
        timeoutMs == 0 ? INFINITE : timeoutMs);

    if (dwWaitResult == WAIT_OBJECT_0) {
        m_processRunning = false;
        return true;
    }
    else if (dwWaitResult == WAIT_TIMEOUT) {
        SetLastError("等待超时");
        return false;
    }
    else {
        SetLastError("等待过程中出错");
        return false;
    }
}

DWORD ExeExecutor::GetExitCode()
{
    DWORD dwExitCode = 0;
    if (m_processInfo.hProcess != NULL) {
        GetExitCodeProcess(m_processInfo.hProcess, &dwExitCode);
    }
    return dwExitCode;
}

bool ExeExecutor::TerminateProcess()
{
    if (m_processInfo.hProcess != NULL) {
        if (::TerminateProcess(m_processInfo.hProcess, 1)) {
            m_processRunning = false;
            return true;
        }
    }
    return false;
}

std::string ExeExecutor::GetLastError()
{
    return m_lastError;
}

void ExeExecutor::ClearLastError()
{
    m_lastError.clear();
}

void ExeExecutor::SetLastError(const std::string& error)
{
    m_lastError = error;
}
#include <Windows.h>
#include<string>
/**
 * @brief EXE 执行器类
 */
class ExeExecutor
{
public:
    ExeExecutor();
    ~ExeExecutor();

    /**
     * @brief 执行外部 EXE
     * @param exePath [in] EXE 文件完整路径
     * @param arguments [in] 命令行参数（可选）
     * @param waitForCompletion [in] 是否等待程序完成
     * @return true: 执行成功 false: 执行失败
     */
    bool ExecuteExe(const std::string& exePath,
        const std::string& arguments = "",
        bool waitForCompletion = false);

    /**
     * @brief 获取最后的错误信息
     * @return 错误描述
     */
    std::string GetLastError();

    /**
     * @brief 等待进程完成
     * @param timeoutMs [in] 超时时间（毫秒），0 表示无限等待
     * @return true: 进程完成 false: 超时或出错
     */
    bool WaitForCompletion(DWORD timeoutMs = 0);

    /**
     * @brief 获取进程退出码
     * @return 进程退出码
     */
    DWORD GetExitCode();

    /**
     * @brief 杀死进程
     * @return true: 成功 false: 失败
     */
    bool TerminateProcess();

private:
    PROCESS_INFORMATION m_processInfo;
    std::string m_lastError;
    bool m_processRunning;

    void ClearLastError();
    void SetLastError(const std::string& error);
};
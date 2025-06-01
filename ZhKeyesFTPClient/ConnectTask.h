/**
 * @desc:   连接任务，ConnectTask.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_CONNECTTASK_H_
#define HANDLER_CONNECTTASK_H_

#include "Task.h"

#include <cstdint>
#include <string>

class ConnectTask : public Task
{
public:
    ConnectTask(const std::wstring& ip, uint16_t port,
        const std::wstring& userName, const std::wstring& password, bool isPassiveMode);
    virtual ~ConnectTask() = default;

    virtual void doTask() override;

private:
    std::wstring                m_controlIp;
    uint16_t                    m_controlPort;
    std::wstring                m_userName;
    std::wstring                m_password;
    bool                        m_isPassiveMode{ false };

};


#endif
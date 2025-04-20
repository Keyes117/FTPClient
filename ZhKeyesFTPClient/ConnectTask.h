/**
 * @desc:   连接任务，Logon.h
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
        const std::wstring& userName, const std::wstring& password);
    virtual ~ConnectTask() = default;

    virtual void doTask() override;

private:
    std::wstring             m_ip;
    uint16_t                m_port;
    std::wstring             m_userName;
    std::wstring             m_password;

};


#endif
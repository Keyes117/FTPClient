#include "ConnectTask.h"

#include "FTPServer.h"

ConnectTask::ConnectTask(const std::wstring& ip, uint16_t port,
    const std::wstring& userName, const std::wstring& password)
    :m_ip(ip), m_port(port),
    m_userName(userName),
    m_password(password)
{
}

void ConnectTask::doTask()
{
    //建立socket 连接
    //FTPServer::getInstance().connect(m_ip.t, m_port)
        //建一个
}

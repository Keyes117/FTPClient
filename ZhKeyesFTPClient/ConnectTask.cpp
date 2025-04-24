#include "ConnectTask.h"

#include <codecvt>
#include <locale>

#include "FTPServer.h"
#include "StringUtils.h"


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

    std::wstring_convert<std::codecvt_utf8<wchar_t>> connverter;
    std::string ip = connverter.to_bytes(m_ip);

    //std::string ip = wstringToString(m_ip);
    if (!FTPServer::getInstance().connect(ip, m_port))
    {
        //TODO 链接不成功，报告错误 并且重试
        int k = 0;
    }

    FTPServer::getInstance().recvBuf();

}

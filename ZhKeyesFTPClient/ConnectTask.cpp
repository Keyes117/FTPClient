#include "ConnectTask.h"

#include <codecvt>
#include <locale>

#include "FTPClient.h"
#include "StringUtils.h"


ConnectTask::ConnectTask(const std::wstring& ip, uint16_t port,
    const std::wstring& userName, const std::wstring& password, bool isPassiveMode)
    :m_controlIp(ip), m_controlPort(port),
    m_userName(userName),
    m_password(password),
    m_isPassiveMode(isPassiveMode)
{
}

void ConnectTask::doTask()
{
    //建立socket 连接
    //FTPServer::getInstance().connect(m_ip.t, m_port)
        //建一个

    std::wstring_convert<std::codecvt_utf8<wchar_t>> connverter;
    std::string ip = connverter.to_bytes(m_controlIp);
    std::string username = connverter.to_bytes(m_userName);
    std::string password = connverter.to_bytes(m_password);


    FTPClient::getInstance().setServerInfo(ip, m_controlPort, username, password, m_isPassiveMode);
    FTPClient::getInstance().startNetworkThread();

    FTPClient::getInstance().connectWithResponse();



}

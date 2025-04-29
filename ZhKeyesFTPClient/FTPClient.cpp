#include "FTPClient.h"

#include <chrono>
#include <functional>

#include "AsyncLog.h"
#include "CharChecker.h"
#pragma comment(lib, "Ws2_32.lib")

#define  MAX_RESPONSE_LENGTH 256

FTPClient& FTPClient::getInstance()
{
    static FTPClient client;
    return client;
}

FTPClient::FTPClient()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    ::WSAStartup(wVersionRequested, &wsaData);
}

FTPClient::~FTPClient()
{
    ::WSACleanup();
}

void FTPClient::startNetworkThread()
{
    if (m_networkThreadrunning)
        return;
    m_networkThreadrunning = true;
    m_spNetWorkThread = std::make_unique<std::thread>(std::bind(&FTPClient::networkThreadFunc, this));
}

void FTPClient::stopNetworkThread()
{
    m_networkThreadrunning = false;
    m_spNetWorkThread->join();
}

bool FTPClient::logon(const char* ip, uint16_t port, const char* username, const char* password)
{
    return false;
}

std::string FTPClient::list()
{
    return std::string();
}

bool FTPClient::cwd()
{
    return false;
}

bool FTPClient::upload()
{
    return false;
}

bool FTPClient::download()
{
    return false;
}

bool FTPClient::setMode(FTPMODE mode)
{
    return false;
}

bool FTPClient::connect(int timeout)
{
    m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_hSocket == INVALID_SOCKET)
        return false;

    long tmSend = 3 * 1000L;
    long tmRecv = 3 * 1000L;
    long noDelay = 1;

    setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)&noDelay, sizeof(long));
    setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)&tmSend, sizeof(long));
    setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)&tmRecv, sizeof(long));

    //将socket设置成非阻塞的
    unsigned long on = 1;
    if (::ioctlsocket(m_hSocket, FIONBIO, &on) == SOCKET_ERROR)
        return false;

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    if ((addrSrv.sin_addr.s_addr = inet_addr(m_ip.c_str())) == INADDR_NONE)
    {
        pHostent = ::gethostbyname(m_ip.c_str());
        if (!pHostent)
        {
            //LOG_ERROR("Could not connect server:%s, port:%d.", m_strServer.c_str(), m_nPort);
            return false;
        }
        else
            addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)m_port);
    int ret = ::connect(m_hSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == 0)
    {
        //LOG_INFO("Connect to server:%s, port:%d successfully.", m_strServer.c_str(), m_nPort);
        m_bConnected = true;
        return true;
    }
    else if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(m_hSocket, &writeset);
    struct timeval tv = { timeout, 0 };
    if (::select(m_hSocket + 1, NULL, &writeset, NULL, &tv) != 1)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    m_bConnected = true;

    return true;
}

bool FTPClient::recvBuf()
{
    if (!m_bConnected)
        return false;



    while (true)
    {
        char buf[64] = { 0 };

        int bytesRecv = recv(m_hSocket, buf, 64, 0);

        if (bytesRecv == 0)
            return false;
        else if (bytesRecv == -1)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                //当前没有数据
                break;
            }
            else
            {
                return false;
            }
        }

        m_recvBuf.append(buf, bytesRecv);
    }

    return true;

}

bool FTPClient::sendBuf()
{
    int n;
    while (true)
    {
        n = ::send(m_hSocket, m_sendBuf.c_str(), m_sendBuf.size(), 0);
        if (n == 0)
        {
            //对端关闭了连接
            return false;
        }
        else if (n < 0)
        {
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            else
            {
                // 发送出错了
                return false;
            }
        }
        else
        {
            if (n == static_cast<int>(m_sendBuf.size()))
            {
                m_sendBuf.erase(0, n);
                return true;
            }
            else
            {
                m_sendBuf.erase(0, n);
            }

        }
    }
}

DecodePackageResult FTPClient::decodePackge()
{
    return m_protocolParser.praseFTPResponse(m_recvBuf, m_responseLine);

}

bool FTPClient::parseState()
{
    for (auto& line : m_responseLine)
    {
        if (line.isEnd)
        {
            switch (line.statusCode)
            {
            case SERVICE_READY_FOR_USER:
            m_clientState = FTP_CLIENT_STATE::WELCOMEMSGRECEIVED;
            return true;

            case SERVICE_READY_FOR_PASSWORD:
            m_clientState = FTP_CLIENT_STATE::PASSWORDISNEEDED;
            return true;

            case SERVICE_USER_LOG_IN:
            m_clientState = FTP_CLIENT_STATE::LOGON;
            return true;

            case SERVICE_PATHNAME_CREATED:
            m_clientState = FTP_CLIENT_STATE::LOGON;
            return true;

            case SERVICE_ENTER_PASSIVE_MODE:
            m_clientState = FTP_CLIENT_STATE::LOGON;

            //TODO: 进一步解析被动模式时 约定的IP和端口号
            return true;
            default:
            return false;
            }

        }

    }

    return false;
}

void FTPClient::setServerInfo(const std::string& ip, uint16_t port,
    const std::string& username, const std::string& password, bool isPassiveMode)
{
    m_ip = ip;
    m_port = port;

    m_username = username;
    m_password = password;

    m_isPassiveMode = isPassiveMode;
}

void FTPClient::networkThreadFunc()
{
    while (m_networkThreadrunning)
    {

        //简历socket连接
        if (!m_bConnected)
        {
            LOGI("Connected to %s:%d", m_ip.c_str(), m_port);
            if (!connect())
            {
                //TODO 反馈给UI层,
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                continue;
            }
        }
        else
        {
            //查看 是否有数据需要接受， 有则接受 并且解包，之后通知UI层

            fd_set readset;
            FD_ZERO(&readset);
            FD_SET(m_hSocket, &readset);
            struct timeval tv = { 1, 0 };

            int ret = ::select(m_hSocket + 1, &readset, NULL, NULL, &tv);
            if (ret == 0)
            {
                //select 超时
                LOGI("No Data ");
                continue;
            }
            else if (ret == 1)
            {
                //有读事件
                //收包
                if (!recvBuf())
                {
                    //关闭连接 重连
                    continue;
                }

                DecodePackageResult result = decodePackge();
                if (result == DecodePackageResult::Error)
                {
                    continue;
                }
                else if (result == DecodePackageResult::ExpectMore)
                {
                    continue;
                }
                else
                {
                    //成功拿到了包，给UI层
                    for (const auto& line : m_responseLine)
                    {
                        LOGI("%d,%s", line.statusCode, line.statusText.c_str());
                    }

                    if (!parseState())
                    {
                        //关闭连接 重试 
                        continue;
                    }

                    m_responseLine.clear();
                }
                //解包
            }
            else
            {
                //select 出错， 关闭socket 进行重连
            }



            //查看是否有数据需要发送 有则发送
            switch (m_clientState)
            {
            case FTP_CLIENT_STATE::WELCOMEMSGRECEIVED:
            {
                m_sendBuf.append("USER " + m_username + "\r\n");
                break;
            }
            case FTP_CLIENT_STATE::PASSWORDISNEEDED:
            {
                m_sendBuf.append("PASS " + m_password + "\r\n");
                break;
            }
            case FTP_CLIENT_STATE::LOGON:
            {
                //登录成功
                if (m_isPassiveMode)
                    m_sendBuf.append("PASV\r\n");
                break;
            }

            default:
            break;
            }

            if (!sendBuf())
            {
                //出错, 重试
            }
        }

    }
}






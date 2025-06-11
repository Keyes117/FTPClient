#include "FTPClient.h"

#include <chrono>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "AsyncLog.h"
#include "CharChecker.h"
#include "StringUtil.h"
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
    //m_spNetWorkThread = std::make_unique<std::thread>(std::bind(&FTPClient::networkThreadFunc, this));
}

void FTPClient::stopNetworkThread()
{
    m_networkThreadrunning = false;
    m_spNetWorkThread->join();
}

bool FTPClient::logon()
{
    if (!m_bControlChannelConnected)
        return false;

    //发送用户名
    std::string buf("USER ");
    buf.append(m_username);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_READY_FOR_PASSWORD)
            userRecieved = true;
    }

    if (!userRecieved)
    {
        closeSocket();
        return false;
    }

    //if (responseLines.size() > 1)

    //发送密码
        //发送用户名
    buf.clear();
    buf.append("PASS ");
    buf.append(m_password);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    //std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    bool passwordRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_USER_LOG_IN)
            return true;
    }

    return false;
}

bool FTPClient::list()
{
    if (m_isPassiveMode)
        return listInPassiveMode();

    return listInActiveMode();
}

bool FTPClient::connectWithResponse()
{
    if (!connect())
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_READY_FOR_USER)
            return true;
    }

    return false;

}

std::string FTPClient::pwd()
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf("PWD\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_PATHNAME_CREATED)
        {
            //TODO: 返回的目录是: "/" 的话，则需要进一步解析
            return line.statusText;
        }

    }

}

bool FTPClient::pasv()
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf("PASV\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_ENTER_PASSIVE_MODE)
        {
            LOGI("Enter passive mode");
            parseDataIpAndPort(line.statusText);
            return true;
        }

    }

    return false;
}

bool FTPClient::cwd(const std::string& targetDir)
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf;
    buf.append("CWD ");
    buf.append(targetDir);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_FILE_ACTION_OKAY_COMPLETED)
        {
            return true;
        }

    }

    return false;
}

bool FTPClient::del(const std::string& targetDirOrFile)
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf;
    buf.append("CWD ");
    buf.append(targetDirOrFile);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_FILE_ACTION_OKAY_COMPLETED)
            {

                return true;
            }
            else if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_ACTION_NOT_TOKEN)
            {
                //TODO:细分失败原因
                return false;
            }

        }

    }




    return false;
}

bool FTPClient::port()
{
    if (!m_bControlChannelConnected)
        return false;

    if (!createDataServer())
        return false;

    if (!getDataServerAddr(m_dataIp, m_dataPort))
        return false;

    //127.0.0.1 => 127,0,0,1;
    std::string tmpIpStr(m_dataIp);
    StringUtil::replace(tmpIpStr, ".", ",");

    char requestStr[64] = {};
    sprintf_s(requestStr, sizeof(requestStr), "PORT %s,%d,%d\r\n",
        tmpIpStr.c_str(), m_dataPort / 256, m_dataPort % 256);

    //发送用户名
    std::string buf(requestStr);

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd && line.statusCode == FTP_STATUS_CODE::SERVICE_COMMAND_OK)
        {
            return true;
        }

    }




    return false;
}

bool FTPClient::upload(const std::string& localFilePath, const std::string& serverFileName)
{
    if (m_isPassiveMode)
    {
        return uploadInPassiveMode(localFilePath, serverFileName);
    }

    return uploadInActiveMode(localFilePath, serverFileName);
}

bool FTPClient::download(const std::string& localFilePath, const std::string& serverFileName)
{
    if (m_isPassiveMode)
    {
        return downloadInPassiveMode(localFilePath, serverFileName);
    }

    return uploadInActiveMode(localFilePath, serverFileName);
}

bool FTPClient::mkdir(const std::string& targetDir)
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf;
    buf.append("MKD ");
    buf.append(targetDir);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_PATHNAME_CREATED)
            {

                return true;
            }
            else if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_ACTION_NOT_TOKEN)
            {
                //TODO:细分失败原因
                return false;
            }

        }

    }
    return false;
}

bool FTPClient::rename(const std::string& serverOldFileName, const std::string& serverNewFileName)
{
    if (!m_bControlChannelConnected)
        return "";

    //发送用户名
    std::string buf;
    buf.append("RNFR ");
    buf.append(serverOldFileName);
    buf.append("\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_FILE_ACTION_PENDDING_FURTHRE_INFO)
            {
                break;
            }
            else if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_ACTION_NOT_TOKEN)
            {
                //TODO:细分失败原因
                return false;
            }

        }

    }

    //发送用户名
    std::string buf2;
    buf2.append("RNTO ");
    buf2.append(serverNewFileName);
    buf2.append("\r\n");

    if (!sendBuf(buf2))
    {
        closeSocket();
        return false;
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return false;
    }


    std::vector<ResponseLine> responseLines2;
    if (!recvBuf(responseLines2))
    {
        closeSocket();
        return false;
    }

    for (const auto& line : responseLines2)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_FILE_ACTION_OKAY_COMPLETED)
            {
                return true;
            }
            else if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUESTED_ACTION_NOT_TOKEN)
            {
                //TODO:细分失败原因
                return false;
            }

        }

    }
    return false;
}


bool FTPClient::setMode(FTPMODE mode)
{
    return false;
}

bool FTPClient::connect(int timeout)
{
    m_hControlSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_hControlSocket == INVALID_SOCKET)
        return false;

    long tmSend = 3 * 1000L;
    long tmRecv = 3 * 1000L;
    long noDelay = 1;

    setsockopt(m_hControlSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)&noDelay, sizeof(long));
    setsockopt(m_hControlSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)&tmSend, sizeof(long));
    setsockopt(m_hControlSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)&tmRecv, sizeof(long));

    //将socket设置成非阻塞的
    unsigned long on = 1;
    if (::ioctlsocket(m_hControlSocket, FIONBIO, &on) == SOCKET_ERROR)
    {
        return false;

    }

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    if ((addrSrv.sin_addr.s_addr = inet_addr(m_controlIp.c_str())) == INADDR_NONE)
    {
        pHostent = ::gethostbyname(m_controlIp.c_str());
        if (!pHostent)
        {
            //LOG_ERROR("Could not connect server:%s, port:%d.", m_strServer.c_str(), m_nPort);
            return false;
        }
        else
            addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)m_controlPort);
    int ret = ::connect(m_hControlSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == 0)
    {
        //LOG_INFO("Connect to server:%s, port:%d successfully.", m_strServer.c_str(), m_nPort);
        m_bControlChannelConnected = true;
        return true;
    }
    else if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(m_hControlSocket, &writeset);
    struct timeval tv = { timeout, 0 };
    if (::select(m_hControlSocket + 1, NULL, &writeset, NULL, &tv) != 1)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    m_bControlChannelConnected = true;


    //TODO:测试
    //std::string localIP;
    //uint16_t localport;
    //getLocalIpAndPort(localIP, localport);


    return true;
}

bool FTPClient::recvBuf(std::vector<ResponseLine>& responseLines)
{
    std::string buf;

    if (!m_bControlChannelConnected)
        return false;

    while (true)
    {
        char tmp[64] = { 0 };

        int bytesRecv = ::recv(m_hControlSocket, tmp, 64, 0);

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

        buf.append(tmp, bytesRecv);

        DecodePackageResult result = m_protocolParser.praseFTPResponse(buf, responseLines);
        if (result == DecodePackageResult::Error)
        {
            return false;
        }
        else if (result == DecodePackageResult::ExpectMore)
        {
            continue;
        }
        else
        {
            // 得到一个正确的相应
            return true;
        }
    }

    return true;

}

bool FTPClient::sendBuf(std::string& buf)
{
    int n;
    while (true)
    {
        n = ::send(m_hControlSocket, buf.c_str(), buf.size(), 0);
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
            if (n == static_cast<int>(buf.size()))
            {
                buf.erase(0, n);
                return true;
            }
            else
            {
                buf.erase(0, n);
            }

        }
    }
}

bool FTPClient::checkReadable(SOCKET socket, int timeoutSec)
{
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(socket, &readset);
    struct timeval tv = { timeoutSec, 0 };

    int ret = ::select(socket + 1, &readset, NULL, NULL, &tv);
    if (ret == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool FTPClient::parseDataIpAndPort(const std::string& responseLine)
{
    //227 Entering Passive Mode (127,0,0,1,249,38)

    size_t startBracketPos = responseLine.find("(");
    size_t endBracketPos = responseLine.find(")");

    if (startBracketPos != std::string::npos && endBracketPos != std::string::npos)
    {
        std::string ipAndPortStr = responseLine.substr(startBracketPos + 1, endBracketPos - startBracketPos - 1);

        std::vector<std::string> ipAndPort;
        StringUtil::split(ipAndPortStr, ipAndPort, ",");
        if (ipAndPort.size() != 6)
            return false;

        m_dataIp = ipAndPort[0] + "." + ipAndPort[1] + "." + ipAndPort[2] + "." + ipAndPort[3];

        int portV1 = atoi(ipAndPort[4].c_str());
        int portV2 = atoi(ipAndPort[5].c_str());

        if (portV1 < 0 || portV1 > 65535 || portV2 < 0 || portV2> 65535)
            return false;

        m_dataPort = static_cast<uint16_t>(256 * portV1 + portV2);
        if (m_dataPort <= 0 || m_dataPort > 65535)
            return false;

        return true;
    }
}

bool FTPClient::getDataServerAddr(std::string& localIp, uint16_t& localPort)
{
    if (!m_bDataChannelConnected || !m_bControlChannelConnected)
        return false;

    //通过控制socket 获取监听的Ip地址
    sockaddr_storage addr;
    socklen_t addrLen = sizeof(addr);

    int res = getsockname(m_hControlSocket, (sockaddr*)&addr, &addrLen);
    if (res == SOCKET_ERROR)
    {
        return false;
    }

    char hostbuf[NI_MAXHOST];
    char portbuf[NI_MAXHOST];

    res = getnameinfo((const SOCKADDR*)&addr,
        addrLen,
        hostbuf,
        NI_MAXHOST,
        nullptr, 0, NI_NUMERICHOST | NI_NUMERICSERV);

    if (res != 0)
        return false;


    localIp = hostbuf;


    //通过数据连接的socket 获取监听的端口号
    sockaddr_storage dataServerAddr;
    addrLen = sizeof(dataServerAddr);

    res = getsockname(m_hListenSocket, (sockaddr*)&dataServerAddr, &addrLen);
    if (res == SOCKET_ERROR)
    {
        return false;
    }



    //getsockname 获取到网络字节序，需要转成本机字节序
    localPort = ntohs(((struct sockaddr_in*)&dataServerAddr)->sin_port);


    return true;
}

bool FTPClient::createDataServer()
{
    //1.创建一个侦听socket
    m_hListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_hListenSocket == SOCKET_ERROR)
    {
        LOGE("Failed to Create Data Socket");
        return false;
    }

    //2.初始化服务器地址
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(0);
    if (bind(m_hListenSocket, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
    {
        LOGE("bind listen socket error.");
        closesocket(m_hListenSocket);
        return false;
    }

    //3.启动侦听
    if (listen(m_hListenSocket, SOMAXCONN) == -1)
    {
        LOGE("listen error.");
        closesocket(m_hListenSocket);
        return false;
    }

    m_bDataChannelConnected = true;

    return true;
}

bool FTPClient::parseDirEntires(const std::string& dirInfo, std::vector<DirEntry>& entries)
{
    //解析如下格式
    //type=;modify=size=;1.txt \r\ntype=;modify=size=;1.txt \r\n 

    std::vector<std::string> v;
    StringUtil::split(dirInfo, v, "\r\n");
    if (v.empty())
        return false;

    const int MODIFY_PREFIX_LENGTH = strlen("modify=");
    const int SIZE_PREFIX_LENGTH = strlen("size=");
    const size_t DIR_COUNT_INFO = 3;
    const size_t FILE_COUNT_INFO = 4;
    for (const auto& iter : v)
    {
        //type = ; modify = size = ; 1.tx
        std::vector<std::string> v2;
        StringUtil::split(iter, v2, ";");

        if (v2.size() != DIR_COUNT_INFO && v2.size() != FILE_COUNT_INFO)
            continue;

        DirEntry entry;
        if (v2[0] == "type=file")
        {
            //文件串格式 type=;modify=;size=;1.txt \r\ntype=;modify=;size=;1.txt
            entry.type = FileType::File;
            entry.modify = v2[1].substr(MODIFY_PREFIX_LENGTH);
            std::string sizeStr = v2[2].substr(SIZE_PREFIX_LENGTH);
            entry.size = atoll(sizeStr.c_str());

            entry.name = v2[3].substr(1);
        }
        else if (v2[0] == "type=dir")
        {
            //目录串格式 type=;modify=size=;1.txt \r\ntype=;modify=;1.txt
            entry.type = FileType::Dir;
            entry.modify = v2[1].substr(MODIFY_PREFIX_LENGTH);
            entry.size = 0;
            entry.name = v2[2].substr(1);
        }
        else
            continue;

        entries.push_back(entry);
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

bool FTPClient::sendBytes(SOCKET s, char* buf, int bufLen)
{
    int pos = 0;
    while (true)
    {
        int n = send(s, buf, bufLen - pos, 0);
        if (n == 0)
        {
            if (pos == bufLen)
                return true;
            else
                return false;
        }
        else if (n > 0)
        {
            pos += n;
            if (pos == bufLen)
                return true;

            continue;
        }
        else
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            else
                return false;
        }
    }
}

bool FTPClient::listInActiveMode()
{
    if (m_isPassiveMode)
    {
        //TODO:被动模式下的拉取文件列表信息
        return false;
    }

    if (!m_bDataChannelConnected)
        return false;

    //发送用户名
    std::string buf("MLSD\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION)
            {
                userRecieved = true;
                break;
            }
        }

    }

    if (!userRecieved)
    {
        return true;
    }

    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(clientaddr);
    //4. 接受客户端连接
    m_hDataSocket = accept(m_hListenSocket, (struct sockaddr*)&clientaddr, &clientaddrlen);
    if (m_hDataSocket < 0)
        return false;

    u_long argp = 1;
    ioctlsocket(m_hDataSocket, FIONBIO, &argp);

    int n;
    std::string dataRecvBuf;
    while (true)
    {
        char buf[4096] = { 0 };
        n = recv(m_hDataSocket, buf, sizeof(buf), 0);
        if (n > 0)
        {
            dataRecvBuf.append(buf, n);
        }
        else if (n < 0)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }
            else
            {
                ::closesocket(m_hDataSocket);
                ::closesocket(m_hListenSocket);

                m_bDataChannelConnected = false;

                return false;
            }
        }
        else//n =0 数据收完了
        {
            ::closesocket(m_hDataSocket);
            ::closesocket(m_hListenSocket);

            m_bDataChannelConnected = false;
            break;
        }
    }


    std::vector<DirEntry> entries;
    parseDirEntires(dataRecvBuf, entries);

    //解析目录数据
    LOGI("received dir info:");
    for (const auto& entry : entries)
    {
        LOGI("name: %s, type: %s, size: %lld, modify: %s",
            entry.name.c_str(),
            entry.type == FileType::File ? "file" : "dir",
            entry.size,
            entry.modify.c_str());
    }

    return true;
}

bool FTPClient::listInPassiveMode()
{
    if (m_isPassiveMode)
    {
        //TODO:被动模式下的拉取文件列表信息
        return false;
    }

    if (!m_bDataChannelConnected)
        return false;

    //发送用户名
    std::string buf("MLSD\r\n");

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION)
            {
                userRecieved = true;
                break;
            }
        }

    }

    if (!userRecieved)
    {
        return true;
    }

    m_hDataSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_hDataSocket == INVALID_SOCKET)
        return false;

    long tmSend = 3 * 1000L;
    long tmRecv = 3 * 1000L;
    long noDelay = 1;

    setsockopt(m_hDataSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)&noDelay, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)&tmSend, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)&tmRecv, sizeof(long));

    //将socket设置成非阻塞的
    unsigned long on = 1;
    if (::ioctlsocket(m_hDataSocket, FIONBIO, &on) == SOCKET_ERROR)
    {
        closesocket(m_hDataSocket);
        return false;

    }

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    if ((addrSrv.sin_addr.s_addr = inet_addr(m_dataIp.c_str())) == INADDR_NONE)
    {
        pHostent = ::gethostbyname(m_dataIp.c_str());
        if (!pHostent)
        {
            //LOG_ERROR("Could not connect server:%s, port:%d.", m_strServer.c_str(), m_nPort);
            closesocket(m_hDataSocket);
            return false;
        }
        else
            addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)m_dataPort);
    int ret = ::connect(m_hDataSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == 0)
    {
        //LOG_INFO("Connect to server:%s, port:%d successfully.", m_strServer.c_str(), m_nPort);
        m_bDataChannelConnected = true;
        return true;
    }
    else if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        closesocket(m_hDataSocket);
        return false;
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(m_hDataSocket, &writeset);
    struct timeval tv = { 3, 0 };
    if (::select(m_hDataSocket + 1, NULL, &writeset, NULL, &tv) != 1)
    {
        closesocket(m_hDataSocket);
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    m_bControlChannelConnected = true;

    int n;
    std::string dataRecvBuf;
    while (true)
    {
        char buf[4096] = { 0 };
        n = recv(m_hDataSocket, buf, sizeof(buf), 0);
        if (n > 0)
        {
            dataRecvBuf.append(buf, n);
        }
        else if (n < 0)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }
            else
            {
                ::closesocket(m_hDataSocket);
                m_bDataChannelConnected = false;

                return false;
            }
        }
        else//n =0 数据收完了
        {
            ::closesocket(m_hDataSocket);
            m_bDataChannelConnected = false;
            break;
        }
    }


    std::vector<DirEntry> entries;
    parseDirEntires(dataRecvBuf, entries);

    //解析目录数据
    LOGI("received dir info:");
    for (const auto& entry : entries)
    {
        LOGI("name: %s, type: %s, size: %lld, modify: %s",
            entry.name.c_str(),
            entry.type == FileType::File ? "file" : "dir",
            entry.size,
            entry.modify.c_str());
    }

    return true;
}

bool FTPClient::uploadInActiveMode(const std::string& localFilePath, const std::string& serverFileName)
{
    if (m_isPassiveMode)
    {
        //TODO:被动模式下的拉取文件列表信息
        return false;
    }

    if (!m_bDataChannelConnected)
        return false;

    //发送用户名
    std::string buf("STOR ");
    buf += serverFileName;
    buf += "\r\n";

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION)
            {
                userRecieved = true;
                break;
            }
        }

    }

    if (!userRecieved)
    {
        return true;
    }

    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(clientaddr);
    //4. 主动连接到服务端
    m_hDataSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_hDataSocket == INVALID_SOCKET)
        return false;

    long tmSend = 3 * 1000L;
    long tmRecv = 3 * 1000L;
    long noDelay = 1;

    setsockopt(m_hDataSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)&noDelay, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)&tmSend, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)&tmRecv, sizeof(long));

    //将socket设置成非阻塞的
    unsigned long on = 1;
    if (::ioctlsocket(m_hDataSocket, FIONBIO, &on) == SOCKET_ERROR)
    {
        closesocket(m_hDataSocket);
        return false;

    }

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    if ((addrSrv.sin_addr.s_addr = inet_addr(m_dataIp.c_str())) == INADDR_NONE)
    {
        pHostent = ::gethostbyname(m_dataIp.c_str());
        if (!pHostent)
        {
            //LOG_ERROR("Could not connect server:%s, port:%d.", m_strServer.c_str(), m_nPort);
            closesocket(m_hDataSocket);
            return false;
        }
        else
            addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)m_dataPort);
    int ret = ::connect(m_hDataSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == 0)
    {
        //LOG_INFO("Connect to server:%s, port:%d successfully.", m_strServer.c_str(), m_nPort);
        m_bDataChannelConnected = true;
        return true;
    }
    else if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        closesocket(m_hDataSocket);
        return false;
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(m_hDataSocket, &writeset);
    struct timeval tv = { 3, 0 };
    if (::select(m_hDataSocket + 1, NULL, &writeset, NULL, &tv) != 1)
    {
        closesocket(m_hDataSocket);
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    m_bControlChannelConnected = true;
    //打开文件 读一段 发一段，发完之后关闭数据连接的发通道

    HANDLE hFile = ::CreateFileA(localFilePath.c_str(),
        GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    DWORD fileSizeHeight;
    DWORD fileSizeLow = GetFileSize(hFile, &fileSizeHeight);

    if (fileSizeLow == INVALID_FILE_SIZE)
    {

        CloseHandle(hFile);
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    int64_t fileSize = (static_cast<int64_t>(fileSizeHeight) << 32) | fileSizeLow;

    int64_t eachByteToRead = 2048;
    char fileBuf[2048];
    DWORD bytesRead;
    bool error = false;
    bool retSendBytes;
    int64_t remainingBytes = fileSize;
    while (true)
    {
        if (remainingBytes <= eachByteToRead)
            eachByteToRead = remainingBytes;

        auto res = ReadFile(hFile,
            fileBuf,
            eachByteToRead,
            &bytesRead,
            NULL);
        if (!res || eachByteToRead != bytesRead)
        {
            auto err = GetLastError();
            error = true;
            break;
        }

        retSendBytes = sendBytes(m_hDataSocket, fileBuf, eachByteToRead);
        if (!retSendBytes)
        {
            error = true;
            break;
        }

        remainingBytes = fileSize - eachByteToRead;
        LOGI("fileName: %s, remaining bytes: %lld", localFilePath.c_str(), remainingBytes);
        //数据已经发完
        if (remainingBytes == 0)
            break;
    }

    CloseHandle(hFile);


    //TODO: 这里不能直接关闭。如果直接关闭可能导致m_hDataSocket上的数据 在内核还未发出


    shutdown(m_hDataSocket, SD_SEND);


    if (!checkReadable(m_hDataSocket))
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        LOGI("select error, errno:%u", WSAGetLastError());

        return false;
    }

    m_bDataChannelConnected = false;

    char tem[32];
    int n = ::recv(m_hDataSocket, tem, 32, 0);
    if (n != 0)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        LOGI("upload failed");

        return false;
    }

    ::closesocket(m_hDataSocket);
    ::closesocket(m_hListenSocket);
    m_bDataChannelConnected = false;

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    responseLines.clear();
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUEST_FILE_SUCCESS)
            {
                LOGI("serverFileName: %s upload successfully.", serverFileName.c_str());
                return true;
            }
        }

    }

    LOGE("serverFileName: %s upload failed, response is not expected.", serverFileName.c_str());

    return false;
}

bool FTPClient::uploadInPassiveMode(const std::string& localFilePath, const std::string& serverFileName)
{
    return false;
}

bool FTPClient::downloadInActiveMode(const std::string& localFilePath, const std::string& serverFileName)
{

    if (!m_bDataChannelConnected)
        return false;

    //发送用户名
    std::string buf("STOR ");
    buf += serverFileName;
    buf += "\r\n";

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION)
            {
                userRecieved = true;
                break;
            }
        }

    }

    if (!userRecieved)
    {
        return true;
    }
    //4. 主动连接到服务端
    m_hDataSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_hDataSocket == INVALID_SOCKET)
        return false;

    long tmSend = 3 * 1000L;
    long tmRecv = 3 * 1000L;
    long noDelay = 1;

    setsockopt(m_hDataSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)&noDelay, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)&tmSend, sizeof(long));
    setsockopt(m_hDataSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)&tmRecv, sizeof(long));

    //将socket设置成非阻塞的
    unsigned long on = 1;
    if (::ioctlsocket(m_hDataSocket, FIONBIO, &on) == SOCKET_ERROR)
    {
        closesocket(m_hDataSocket);
        return false;

    }

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    if ((addrSrv.sin_addr.s_addr = inet_addr(m_dataIp.c_str())) == INADDR_NONE)
    {
        pHostent = ::gethostbyname(m_dataIp.c_str());
        if (!pHostent)
        {
            //LOG_ERROR("Could not connect server:%s, port:%d.", m_strServer.c_str(), m_nPort);
            closesocket(m_hDataSocket);
            return false;
        }
        else
            addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons((u_short)m_dataPort);
    int ret = ::connect(m_hDataSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == 0)
    {
        //LOG_INFO("Connect to server:%s, port:%d successfully.", m_strServer.c_str(), m_nPort);
        m_bDataChannelConnected = true;
        return true;
    }
    else if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        closesocket(m_hDataSocket);
        return false;
    }

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(m_hDataSocket, &writeset);
    struct timeval tv = { 3, 0 };
    if (::select(m_hDataSocket + 1, NULL, &writeset, NULL, &tv) != 1)
    {
        closesocket(m_hDataSocket);
        //LOG_ERROR("Could not connect to server:%s, port:%d.", m_strServer.c_str(), m_nPort);
        return false;
    }

    m_bControlChannelConnected = true;

    //打开文件 读一段 发一段，发完之后关闭数据连接的发通道

    HANDLE hFile = ::CreateFileA(localFilePath.c_str(),
        GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    DWORD fileSizeHeight;
    DWORD fileSizeLow = GetFileSize(hFile, &fileSizeHeight);

    if (fileSizeLow == INVALID_FILE_SIZE)
    {

        CloseHandle(hFile);
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    int64_t fileSize = (static_cast<int64_t>(fileSizeHeight) << 32) | fileSizeLow;

    int64_t eachByteToRead = 2048;
    char fileBuf[2048];
    DWORD bytesRead;
    bool error = false;
    bool retSendBytes;
    int64_t remainingBytes = fileSize;
    while (true)
    {
        if (remainingBytes <= eachByteToRead)
            eachByteToRead = remainingBytes;

        auto res = ReadFile(hFile,
            fileBuf,
            eachByteToRead,
            &bytesRead,
            NULL);
        if (!res || eachByteToRead != bytesRead)
        {
            auto err = GetLastError();
            error = true;
            break;
        }

        retSendBytes = sendBytes(m_hDataSocket, fileBuf, eachByteToRead);
        if (!retSendBytes)
        {
            error = true;
            break;
        }

        remainingBytes = fileSize - eachByteToRead;
        LOGI("fileName: %s, remaining bytes: %lld", localFilePath.c_str(), remainingBytes);
        //数据已经发完
        if (remainingBytes == 0)
            break;
    }

    CloseHandle(hFile);


    //TODO: 这里不能直接关闭。如果直接关闭可能导致m_hDataSocket上的数据 在内核还未发出


    shutdown(m_hDataSocket, SD_SEND);


    if (!checkReadable(m_hDataSocket))
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        LOGI("select error, errno:%u", WSAGetLastError());

        return false;
    }

    m_bDataChannelConnected = false;

    char tem[32];
    int n = ::recv(m_hDataSocket, tem, 32, 0);
    if (n != 0)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        LOGI("upload failed");

        return false;
    }

    ::closesocket(m_hDataSocket);
    ::closesocket(m_hListenSocket);
    m_bDataChannelConnected = false;

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    responseLines.clear();
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUEST_FILE_SUCCESS)
            {
                LOGI("serverFileName: %s upload successfully.", serverFileName.c_str());
                return true;
            }
        }

    }

    LOGE("serverFileName: %s upload failed, response is not expected.", serverFileName.c_str());

    return false;
}

bool FTPClient::downloadInPassiveMode(const std::string& localFilePath, const std::string& serverFileName)
{
    if (!m_bDataChannelConnected)
        return false;

    //发送用户名
    std::string buf("RETR ");
    buf += serverFileName;
    buf += "\r\n";

    if (!sendBuf(buf))
    {
        closeSocket();
        return "";
    }

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    std::vector<ResponseLine> responseLines;
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    bool userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION)
            {
                userRecieved = true;
                break;
            }
        }

    }

    if (!userRecieved)
    {
        return true;
    }

    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(clientaddr);
    //4. 接受客户端连接
    m_hDataSocket = accept(m_hListenSocket, (struct sockaddr*)&clientaddr, &clientaddrlen);
    if (m_hDataSocket < 0)
        return false;

    u_long argp = 1;
    ioctlsocket(m_hDataSocket, FIONBIO, &argp);

    //打开文件 读一段 发一段，发完之后关闭数据连接的发通道

    HANDLE hFile = ::CreateFileA(localFilePath.c_str(),
        GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    DWORD fileSizeHeight;
    DWORD fileSizeLow = GetFileSize(hFile, &fileSizeHeight);

    if (fileSizeLow == INVALID_FILE_SIZE)
    {

        CloseHandle(hFile);
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);

        m_bDataChannelConnected = false;
        return false;
    }

    bool error = false;
    while (true)
    {
        char buf[2048];
        int n = recv(m_hDataSocket, buf, sizeof(buf), 0);
        if (n == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                error = true;
                break;
            }
            else
            {
                continue;
            }
        }
        else if (n > 0)
        {
            DWORD dwBytesWritten;
            WriteFile(hFile, buf, n, &dwBytesWritten, nullptr);
            if (dwBytesWritten != static_cast<DWORD>(n))
            {
                error = true;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (error)
    {
        ::CloseHandle(hFile);

        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        return false;
    }

    FlushFileBuffers(hFile);
    ::CloseHandle(hFile);
    ::closesocket(m_hDataSocket);
    ::closesocket(m_hListenSocket);
    m_bDataChannelConnected = false;

    shutdown(m_hDataSocket, SD_SEND);


    if (!checkReadable(m_hControlSocket))
    {
        ::closesocket(m_hControlSocket);

        return false;
    }

    m_bDataChannelConnected = false;

    char tem[32];
    int n = ::recv(m_hDataSocket, tem, 32, 0);
    if (n != 0)
    {
        ::closesocket(m_hDataSocket);
        ::closesocket(m_hListenSocket);
        m_bDataChannelConnected = false;

        LOGI("upload failed");

        return false;
    }

    ::closesocket(m_hDataSocket);
    ::closesocket(m_hListenSocket);
    m_bDataChannelConnected = false;

    if (!checkReadable(m_hControlSocket))
    {
        closeSocket();
        return "";
    }


    responseLines.clear();
    if (!recvBuf(responseLines))
    {
        closeSocket();
        return "";
    }

    userRecieved = false;
    for (const auto& line : responseLines)
    {
        if (line.isEnd)
        {
            if (line.statusCode == FTP_STATUS_CODE::SERVICE_REQUEST_FILE_SUCCESS)
            {
                LOGI("serverFileName: %s download successfully.", serverFileName.c_str());
                return true;
            }
        }

    }

    LOGE("serverFileName: %s upload failed, response is not expected.", serverFileName.c_str());

    return false;
}

void FTPClient::setServerInfo(const std::string& ip, uint16_t port,
    const std::string& username, const std::string& password, bool isPassiveMode)
{
    m_controlIp = ip;
    m_controlPort = port;

    m_username = username;
    m_password = password;

    m_isPassiveMode = isPassiveMode;
}

void FTPClient::networkThreadFunc()
{
    while (m_networkThreadrunning)
    {

        //建立socket连接
        if (!m_bControlChannelConnected)
        {
            LOGI("Connected to %s:%d", m_controlIp.c_str(), m_controlPort);
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
            FD_SET(m_hControlSocket, &readset);
            struct timeval tv = { 1, 0 };

            int ret = ::select(m_hControlSocket + 1, &readset, NULL, NULL, &tv);
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
                //if (!recvBuf())
                //{
                //    //关闭连接 重连
                //    continue;
                //}

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

            //if (!sendBuf())
            //{
            //    //出错, 重试
            //}
        }

    }
}

void FTPClient::closeSocket()
{
    ::closesocket(m_hControlSocket);

    m_bControlChannelConnected = false;

}






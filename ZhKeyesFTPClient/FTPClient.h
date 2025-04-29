/**
 * @desc:   网络接口API，FTPServer.h
 * @author: zhangyl
 * @date:   2025/4/17
 */

#ifndef NET_FTPCLIENT_H_
#define NET_FTPCLIENT_H_

#include <WinSock2.h>

#include <cstdint>
#include <string>

#include "ProtocolParser.h"
#include <memory>
#include <thread>


enum class FTPMODE
{
    ModeActive = 0,
    ModePassive
};

enum FTP_STATUS_CODE
{
    SERVICE_READY_FOR_USER = 220,
    SERVICE_READY_FOR_PASSWORD = 331,
    SERVICE_USER_LOG_IN = 230,
    SERVICE_PATHNAME_CREATED = 257, //显示当前路径成功
    SERVICE_ENTER_PASSIVE_MODE = 227
};

enum FTP_CLIENT_STATE
{
    DISCONNECTED,
    CONNECTED,
    WELCOMEMSGRECEIVED,
    PASSWORDISNEEDED,
    LOGON
};

class FTPClient final
{
public:
    static FTPClient& getInstance();

public:

    void startNetworkThread();
    void stopNetworkThread();

    bool logon(const char* ip, uint16_t port, const char* username, const char* password);
    std::string list();

    bool cwd();

    bool upload();
    bool download();

    bool setMode(FTPMODE mode = FTPMODE::ModePassive);

    void setServerInfo(const std::string& ip, uint16_t port,
        const std::string& username, const std::string& password, bool isPassiveMode);

private:
    void networkThreadFunc();

    bool connect(int timeoutMs = 3);

    bool recvBuf();

    bool sendBuf();

    DecodePackageResult decodePackge();

    bool parseState();
private:
    SOCKET                              m_hSocket;

    //收发缓冲区
    std::string                         m_recvBuf;
    std::string                         m_sendBuf;

    std::string                         m_ip;
    std::string                         m_username;
    std::string                         m_password;
    uint16_t                            m_port;


    FTP_CLIENT_STATE                    m_clientState{ FTP_CLIENT_STATE::DISCONNECTED };

    bool                                m_bConnected{ false };
    bool                                m_networkThreadrunning{ false };
    bool                                m_isPassiveMode;

    ProtocolParser                      m_protocolParser;

    std::unique_ptr<std::thread>        m_spNetWorkThread;

    std::vector<ResponseLine>           m_responseLine;
private:
    FTPClient();
    ~FTPClient();

    FTPClient(const FTPClient& rhs) = delete;
    FTPClient& operator=(const FTPClient& rhs) = delete;

    FTPClient(FTPClient&& rhs) = delete;
    FTPClient& operator=(FTPClient&& rhs) = delete;
};




#endif  //! NET_FTPSERVER_H_
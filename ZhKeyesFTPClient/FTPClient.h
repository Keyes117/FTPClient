/**
 * @desc:   网络接口API，FTPClient.h
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
    FILE_STATUS_OKAY_ABOUT_TO_OPEN_DATA_CONNECTION = 150,
    SERVICE_COMMAND_OK = 200,
    SERVICE_READY_FOR_USER = 220,
    SERVICE_READY_FOR_PASSWORD = 331,
    SERVICE_USER_LOG_IN = 230,
    SERVICE_PATHNAME_CREATED = 257, //显示当前路径成功
    SERVICE_ENTER_PASSIVE_MODE = 227,
    SERVICE_REQUESTED_FILE_ACTION_OKAY_COMPLETED = 250,// 进入目录成功
    SERVICE_REQUESTED_ACTION_NOT_TOKEN = 550
};

enum FTP_CLIENT_STATE
{
    DISCONNECTED,
    CONNECTED,
    WELCOMEMSGRECEIVED,
    PASSWORDISNEEDED,
    LOGON
};

enum class FileType
{
    File = 0,
    Dir
};

struct DirEntry
{
    std::string name;
    int64_t     size;
    std::string modify;
    FileType    type;
};

class FTPClient final
{
public:
    static FTPClient& getInstance();

public:

    void startNetworkThread();

    void stopNetworkThread();

    bool logon();

    bool list();

    bool connectWithResponse();

    std::string pwd();

    bool pasv();

    bool cwd(const std::string& targetDir);

    bool del(const std::string& targetDirOrFile);

    bool port();

    bool upload(const std::string& localFilePath, const std::string& serverFileName);

    bool uploadDir(const std::string& path);

    bool download();

    bool mkdir(const std::string& path);

    bool setMode(FTPMODE mode = FTPMODE::ModePassive);

    void setServerInfo(const std::string& ip, uint16_t port,
        const std::string& username, const std::string& password, bool isPassiveMode);

private:
    void networkThreadFunc();

    bool connect(int timeoutMs = 3);

    void closeSocket();

    bool recvBuf(std::vector<ResponseLine>& responseLines);

    bool sendBuf(std::string& buf);

    //判断数据是否需要接受
    bool checkReadable(int timeoutSec = 3);

    bool parseDataIpAndPort(const std::string& responseLine);

    bool getDataServerAddr(std::string& localIp, uint16_t& localPort);

    //创建数据通道监听socket
    bool createDataServer();

    bool parseDirEntires(const std::string& dirInfo, std::vector< DirEntry>& entries);

    DecodePackageResult decodePackge();

    bool parseState();

    //用于非阻塞socket把数据发完 
    bool sendBytes(SOCKET s, char* buf, int bufLen);

private:
    //控制通道
    SOCKET                              m_hControlSocket;

    //收发缓冲区
    std::string                         m_recvBuf;
    std::string                         m_sendBuf;

    std::string                         m_controlIp;
    std::string                         m_username;
    std::string                         m_password;
    uint16_t                            m_controlPort;

    FTP_CLIENT_STATE                    m_clientState{ FTP_CLIENT_STATE::DISCONNECTED };

    bool                                m_bControlChannelConnected{ false };
    bool                                m_networkThreadrunning{ false };
    bool                                m_isPassiveMode;

    ProtocolParser                      m_protocolParser;

    std::unique_ptr<std::thread>        m_spNetWorkThread;

    std::vector<ResponseLine>           m_responseLine;

    //监听数据通道
    SOCKET                              m_hListenSocket{ INVALID_SOCKET };

    bool                                m_bDataChannelConnected{ false };

    //用于数据传输
    SOCKET                              m_hDataSocket{ INVALID_SOCKET };

    std::string                         m_dataIp;
    uint16_t                            m_dataPort;

private:
    FTPClient();
    ~FTPClient();

    FTPClient(const FTPClient& rhs) = delete;
    FTPClient& operator=(const FTPClient& rhs) = delete;

    FTPClient(FTPClient&& rhs) = delete;
    FTPClient& operator=(FTPClient&& rhs) = delete;
};




#endif  //! NET_FTPSERVER_H_
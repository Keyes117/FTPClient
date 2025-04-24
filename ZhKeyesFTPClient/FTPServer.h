/**
 * @desc:   ÍøÂç½Ó¿ÚAPI£¬FTPServer.h
 * @author: zhangyl
 * @date:   2025/4/17
 */

#ifndef NET_FTPSERVER_H_
#define NET_FTPSERVER_H_

#include <WinSock2.h>

#include <cstdint>
#include <string>

enum class FTPMODE
{
    ModeActive = 0,
    ModePassive
};

enum class DecodePackageResult
{
    Success = 0,
    Failed,
    ExpectMore
};

class FTPServer final
{
public:
    static FTPServer& getInstance();

public:

    bool logon(const char* ip, uint16_t port, const char* username, const char* password);
    std::string list();

    bool cwd();

    bool upload();
    bool download();

    bool setMode(FTPMODE mode = FTPMODE::ModePassive);

    bool connect(const std::string& ip, uint16_t port, int timeoutMs = 3);

    bool recvBuf();

private:
    bool decodePackage(std::string& recvBuf);

private:
    SOCKET      m_hSocket;

    bool        m_bConnected{ false };

private:
    FTPServer();
    ~FTPServer();

    FTPServer(const FTPServer& rhs) = delete;
    FTPServer& operator=(const FTPServer& rhs) = delete;

    FTPServer(FTPServer&& rhs) = delete;
    FTPServer& operator=(FTPServer&& rhs) = delete;
};




#endif  //! NET_FTPSERVER_H_
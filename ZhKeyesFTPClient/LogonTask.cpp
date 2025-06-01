#include "LogonTask.h"

LogonTask::LogonTask()
{
}

void LogonTask::doTask()
{
    //组装登录数据，交给网络层
    FTPClient::getInstance().logon();
}

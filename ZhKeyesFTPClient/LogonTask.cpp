#include "LogonTask.h"

LogonTask::LogonTask()
{
}

void LogonTask::doTask()
{
    //��װ��¼���ݣ����������
    FTPClient::getInstance().logon();
}

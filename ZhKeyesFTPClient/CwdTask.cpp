#include "CwdTask.h"

#include "FTPClient.h"

CwdTask::CwdTask(const std::string& targetDir)
    :m_targetDir(targetDir)
{
}

void CwdTask::doTask()
{
    FTPClient::getInstance().cwd(m_targetDir);
}



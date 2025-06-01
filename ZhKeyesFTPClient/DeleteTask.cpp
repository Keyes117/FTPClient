
/**
 * @desc:   ɾ���ļ�orĿ¼�����࣬DeleteTask.h
 * @author: ZhKeyes
 * @date:  2025/5/22
 */


#include "DeleteTask.h"
#include "FTPClient.h"
DeleteTask::DeleteTask(const std::string& targetDir)
    :m_targetDir(targetDir)
{
}

void DeleteTask::doTask()
{
    FTPClient::getInstance().del(m_targetDir);
}



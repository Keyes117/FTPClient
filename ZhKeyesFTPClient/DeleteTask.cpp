
/**
 * @desc:   删除文件or目录任务类，DeleteTask.h
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



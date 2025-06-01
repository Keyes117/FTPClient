
/**
 * @desc:   上传文件任务类，UploadTask.cpp
 * @author: ZhKeyes
 * @date:   2025/6/1
 */

#include "UploadTask.h"

#include "FTPClient.h"

UploadTask::UploadTask(const std::string& localPath, const std::string& serverFileName) :
    m_localFilePath(localPath),
    m_serverFileName(serverFileName)
{
}

void UploadTask::doTask()
{
    FTPClient::getInstance().upload(m_localFilePath, m_serverFileName);
}

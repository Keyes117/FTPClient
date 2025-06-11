/**
 * @desc:   下载文件任务类，DownloadTask.cpp
 * @author: ZhKeyes
 * @date:   2025/6/1
 */

#include "DownloadTask.h"

#include "FTPClient.h"

DownloadTask::DownloadTask(const std::string& localPath, const std::string& serverFileName) :
    m_localFilePath(localPath),
    m_serverFileName(serverFileName)
{
}

void DownloadTask::doTask()
{
    FTPClient::getInstance().download(m_localFilePath, m_serverFileName);
}

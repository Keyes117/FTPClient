/**
 * @desc:   下载文件任务类，DownloadTask.h
 * @author: ZhKeyes
 * @date:  2025/6/1
 */

#ifndef HANDLER_DOWNLOAD_H_
#define HANDLER_DOWNLOAD_H_

#include "Task.h"
#include <string>
class DownloadTask : public Task
{
public:
    DownloadTask(const std::string& localPath, const std::string& serverFileName);
    virtual ~DownloadTask() = default;

    virtual void doTask() override;

private:
    std::string     m_localFilePath;
    std::string     m_serverFileName;
};

#endif

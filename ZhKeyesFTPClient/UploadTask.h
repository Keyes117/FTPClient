/**
 * @desc:   上传文件任务类，UploadTask.h
 * @author: ZhKeyes
 * @date:  2025/6/1
 */

#ifndef HANDLER_UPLOADTASK_H_
#define HANDLER_UPLOADTASK_H_

#include "Task.h"
#include <string>
class UploadTask : public Task
{
public:
    UploadTask(const std::string& localPath, const std::string& serverFileName);
    virtual ~UploadTask() = default;

    virtual void doTask() override;

private:
    std::string     m_localFilePath;
    std::string     m_serverFileName;
};

#endif

/**
 * @desc:   删除文件or目录任务类，DeleteTask.h
 * @author: ZhKeyes
 * @date:  2025/5/22
 */

#ifndef HANDLER_DELETETASK_H_
#define HANDLER_DELETETASK_H_

#include "Task.h"
#include <string>

class DeleteTask : public Task
{
public:
    DeleteTask(const std::string& targetDir);
    virtual ~DeleteTask() = default;

    virtual void doTask() override;

private:
    std::string     m_targetDir;
};


#endif
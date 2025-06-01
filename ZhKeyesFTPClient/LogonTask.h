/**
 * @desc:   µÇÂ¼ÈÎÎñÀà£¬LogonTask.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_LOGONTASK_H_
#define HANDLER_LOGONTASK_H_
#include "FTPClient.h"
#include "Task.h"

class LogonTask : public Task
{
public:
    LogonTask();
    virtual ~LogonTask() = default;

    virtual void doTask() override;
};


#endif  //!HANDLER_LOGONTASK_H_
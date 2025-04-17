/**
 * @desc:   µÇÂ¼ÈÎÎñÀà£¬Logon.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_LOGONTASK_H_
#define HANDLER_LOGONTASK_H_
#include "Task.h"

class LogonTask : public Task
{
public:
    LogonTask();
    virtual ~LogonTask() = default;

    virtual void doTask() override;
};


#endif  //!HANDLER_LOGONTASK_H_
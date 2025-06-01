/**
 * @desc:   显示当前目录任务类，PwdTask.h
 * @author: ZhKeyes
 * @date:  2025/5/12
 */

#ifndef HANDLER_PWDTASK_H_
#define HANDLER_PWDTASK_H_

#include "Task.h"

class PwdTask : public Task
{
public:
    PwdTask() = default;
    virtual ~PwdTask() = default;

    virtual void doTask() override;
};


#endif
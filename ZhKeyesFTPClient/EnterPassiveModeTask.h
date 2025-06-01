/**
 * @desc:   输入进行被动模式任务，EnterPassiveModeTask.h
 * @author: ZhKeyes
 * @date:   2025/5/12
 */

#ifndef HANDLER_ENTERPASSIVEMODETASK_H_
#define HANDLER_ENTERPASSIVEMODETASK_H_

#include "Task.h"

class EnterPassiveModeTask : public Task
{
public:
    EnterPassiveModeTask() = default;
    virtual ~EnterPassiveModeTask() = default;

    virtual void doTask() override;
};


#endif
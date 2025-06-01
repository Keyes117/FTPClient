/**
 * @desc:   主动模式下与FTPServer协商ip地址与端口号任务类，PortTask.h
 * @author: ZhKeyes
 * @date:  2025/5/25
 */

#ifndef HANDLER_PORTTASK_H_
#define HANDLER_PORTTASK_H_

#include "Task.h"

class PortTask : public Task
{
public:
    PortTask() = default;
    virtual ~PortTask() = default;

    virtual void doTask() override;
};


#endif
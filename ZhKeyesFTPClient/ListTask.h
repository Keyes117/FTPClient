/**
 * @desc:   ��ȡ��ǰĿ¼�б������࣬ListTask.h
 * @author: ZhKeyes
 * @date:  2025/5/25
 */

#ifndef HANDLER_LISTTASK_H_
#define HANDLER_LISTTASK_H_

#include "Task.h"

class ListTask : public Task
{
public:
    ListTask() = default;
    virtual ~ListTask() = default;

    virtual void doTask() override;
};


#endif
/**
 * @desc:   数据任务接口类，Task.h
 * @author: ZhKeyes
 * @date:   2025/4/17
 */

#ifndef HANDLER_TASK_H_
#define HANDLER_TASK_H_



class Task
{
public:
    Task() = default;
    virtual ~Task() = default;

public:
    virtual void doTask() = 0;
};

#endif // !HANDLER_TASK_H_
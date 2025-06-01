/**
 * @desc:   ����ѡ��Ŀ¼�����࣬CwdTask.h
 * @author: ZhKeyes
 * @date:  2025/5/12
 */

#ifndef HANDLER_CWDTASK_H_
#define HANDLER_CWDTASK_H_

#include <string>

#include "Task.h"


class CwdTask : public Task
{
public:
    CwdTask(const std::string& targetDir);
    virtual ~CwdTask() = default;

    virtual void doTask() override;

private:
    std::string     m_targetDir;
};


#endif
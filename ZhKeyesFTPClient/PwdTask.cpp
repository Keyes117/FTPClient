/**
 * @desc:   显示当前目录任务类，PwdTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/12
 */

#include "PwdTask.h"

#include "FTPClient.h"

void PwdTask::doTask()
{
    FTPClient::getInstance().pwd();
}

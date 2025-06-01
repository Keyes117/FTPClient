/**
 * @desc:   输入进行被动模式任务，EnterPassiveModeTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/12
 */
#include "EnterPassiveModeTask.h"
#include "FTPClient.h"

void EnterPassiveModeTask::doTask()
{
    FTPClient::getInstance().pasv();
}

/**
 * @desc:   ������б���ģʽ����EnterPassiveModeTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/12
 */
#include "EnterPassiveModeTask.h"
#include "FTPClient.h"

void EnterPassiveModeTask::doTask()
{
    FTPClient::getInstance().pasv();
}

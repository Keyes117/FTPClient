/**
 * @desc:   ��ʾ��ǰĿ¼�����࣬PwdTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/12
 */

#include "PwdTask.h"

#include "FTPClient.h"

void PwdTask::doTask()
{
    FTPClient::getInstance().pwd();
}

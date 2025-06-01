/**
 * @desc:   主动模式下与FTPServer协商ip地址与端口号任务类，PortTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/25
 */

#include "PortTask.h"

#include "FTPClient.h"

void PortTask::doTask()
{
    FTPClient::getInstance().port();
}


/**
 * @desc:   拉取当前目录列表任务类，ListTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/25
 */

#include "ListTask.h"

#include "FTPClient.h"

void ListTask::doTask()
{
    FTPClient::getInstance().list();
}

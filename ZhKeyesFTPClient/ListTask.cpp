
/**
 * @desc:   ��ȡ��ǰĿ¼�б������࣬ListTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/25
 */

#include "ListTask.h"

#include "FTPClient.h"

void ListTask::doTask()
{
    FTPClient::getInstance().list();
}

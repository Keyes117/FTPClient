/**
 * @desc:   ����ģʽ����FTPServerЭ��ip��ַ��˿ں������࣬PortTask.cpp
 * @author: ZhKeyes
 * @date:   2025/5/25
 */

#include "PortTask.h"

#include "FTPClient.h"

void PortTask::doTask()
{
    FTPClient::getInstance().port();
}

#include "UIProxy.h"

#include <memory>

#include "ConnectTask.h"
#include "CwdTask.h"
#include "EnterPassiveModeTask.h"
#include "ListTask.h"
#include "LogonTask.h"
#include "PortTask.h"
#include "PwdTask.h"
#include "TaskHandler.h"
#include "UploadTask.h"
UIProxy& UIProxy::getInstance()
{
    // TODO: insert return statement here
    static UIProxy proxy;
    return proxy;
}

void UIProxy::connect(const std::wstring& ip, uint16_t port,
    const std::wstring& userName, const std::wstring& password, bool isPassiveMode)
{
    std::shared_ptr<ConnectTask> spConnectTask = std::make_shared<ConnectTask>(ip, port, userName, password, isPassiveMode);

    TaskHandler::getInstance().registerSendTask(std::move(spConnectTask));

    auto spLogonTask = std::make_shared<LogonTask>();
    TaskHandler::getInstance().registerSendTask(std::move(spLogonTask));

    auto spPwdTask = std::make_shared<PwdTask>();
    TaskHandler::getInstance().registerSendTask(std::move(spPwdTask));

    if (isPassiveMode)
    {
        auto spPasvTask = std::make_shared<EnterPassiveModeTask>();
        TaskHandler::getInstance().registerSendTask(std::move(spPasvTask));
    }

    auto spPortTask = std::make_shared<PortTask>();
    TaskHandler::getInstance().registerSendTask(std::move(spPortTask));

    /*auto spListTask = std::make_shared<ListTask>();
    TaskHandler::getInstance().registerSendTask(std::move(spListTask));*/
    /*  */

    auto spUploadTask = std::make_shared<UploadTask>("F:\\redis-8.0\\redis-8.0\\TLS.md", "TLS.md");
    TaskHandler::getInstance().registerSendTask(std::move(spUploadTask));
    /*  */
}

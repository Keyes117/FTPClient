#include "UIProxy.h"

#include <memory>

#include "ConnectTask.h"
#include "Handler.h"

UIProxy& UIProxy::getInstance()
{
    // TODO: insert return statement here
    static UIProxy proxy;
    return proxy;
}

void UIProxy::connect(const std::wstring& ip, uint16_t port, const std::wstring& userName, const std::wstring& password)
{
    std::shared_ptr<ConnectTask> ptask = std::make_shared<ConnectTask>(ip, port, userName, password);

    Handler::getInstance().registerSendTask(std::move(ptask));
}

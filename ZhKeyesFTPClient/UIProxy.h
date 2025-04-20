/**
 * @desc:   处理UI层所有处理任务的单例类，Handler.h
 * @author: ZhKeyes
 * @date:   2025/4/20
 */

#ifndef HANDLER_HANDLER_H_
#define HANDLER_HANDLER_H_

#include <string>

class UIProxy
{

public:
    static UIProxy& getInstance();

    void connect(const std::wstring& ip, uint16_t port,
        const std::wstring& userName, const std::wstring& password);

private:
    UIProxy() = default;
    ~UIProxy() = default;
};

#endif

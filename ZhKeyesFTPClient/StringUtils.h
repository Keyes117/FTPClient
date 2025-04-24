#pragma once
#include <cwchar>
#include <iostream>
#include <locale>
#include <string>

std::string wstringToString(const std::wstring& wstr) {
    std::string str;
    str.reserve(wstr.size());  // 预分配空间，优化性能

    std::mbstate_t state = std::mbstate_t();  // 保存转换状态
    for (wchar_t wc : wstr) {
        char buf[2];  // 存储多字节字符
        std::wcrtomb(buf, wc, &state);  // 将宽字符转换为多字节字符
        str.append(buf);  // 将转换结果添加到目标字符串
    }

    return str;
}
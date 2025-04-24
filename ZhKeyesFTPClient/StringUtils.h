#pragma once
#include <cwchar>
#include <iostream>
#include <locale>
#include <string>

std::string wstringToString(const std::wstring& wstr) {
    std::string str;
    str.reserve(wstr.size());  // Ԥ����ռ䣬�Ż�����

    std::mbstate_t state = std::mbstate_t();  // ����ת��״̬
    for (wchar_t wc : wstr) {
        char buf[2];  // �洢���ֽ��ַ�
        std::wcrtomb(buf, wc, &state);  // �����ַ�ת��Ϊ���ֽ��ַ�
        str.append(buf);  // ��ת�������ӵ�Ŀ���ַ���
    }

    return str;
}
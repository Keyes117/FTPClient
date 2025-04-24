/**
 * @desc:   检查字符类 不能被实例化，CharChecker.h
 * @author: ZhKeyes
 * @date:   2025/4/24
 */

#ifndef UTILS_CHARCHECKER_H_
#define UTILS_CHARCHECKER_H_


class CharChecker final
{
public:
    static bool isDigit(char c);

private:
    CharChecker() = delete;
    ~CharChecker() = delete;


    CharChecker(const CharChecker&) = delete;
    CharChecker& operator=(const CharChecker&) = delete;

    CharChecker(CharChecker&&) = delete;
    CharChecker& operator=(CharChecker&&) = delete;

};

#endif
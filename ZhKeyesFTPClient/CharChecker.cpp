#include "CharChecker.h"

bool CharChecker::isDigit(char ch)
{
    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    return true;
    break;

    default:
    return false;
    }
}

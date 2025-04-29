
/**
 * @desc:   协议解析类，ProtocolParser.cpp
 * @author: ZhKeyes
 * @date:   2025/4/26
 */

#include "ProtocolParser.h"

#define STATUS_CODE_LENGTH                  3
#define SPACE_OR_MINUS_LENGTH               1 
#define FTP_RESPONSE_LINE_END_FLAG_LAENGTH  2 

 //允许的最大的Response相应行的文本长度
#define MAX_REPONSE_LINE_TEXT_LENGTH        256


#define MAKE_STATUS_CODE(ch1,ch2,ch3)  \
                (static_cast<int16_t>(m_statusFirstLetter- '0') * 100  +\
            static_cast<int16_t>(m_statusSecondLetter - '0') * 10    +\
            static_cast<int16_t>(m_statusThirdLetter- '0'))



DecodePackageResult ProtocolParser::praseFTPResponse(std::string& buf, std::vector<ResponseLine>& responseLines)
{
    char p;
    size_t bufLen = buf.length();
    size_t currentTextLength = 0;

    for (; m_prasePos < bufLen; m_prasePos++)
    {
        p = buf[m_prasePos];

        switch (m_parseStatus)
        {
        case FTP_PARSE_STATUS::FTP_PARSE_START:
        {
            if (p >= '0' && p <= '9')
            {
                m_statusFirstLetter = p;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_FIRST_LETTER;
            }
            else
            {
                return DecodePackageResult::Error;
            }
            break;
        }//end case FTP_PARSE_START
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_FIRST_LETTER:
        {
            if (p >= '0' && p <= '9')
            {
                m_statusSecondLetter = p;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_SECOND_LETTER;
            }
            else
            {
                return DecodePackageResult::Error;
            }
            break;
        }
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_SECOND_LETTER:
        {
            if (p >= '0' && p <= '9')
            {
                m_statusThirdLetter = p;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_THIRD_LETTER;
            }
            else
            {
                return DecodePackageResult::Error;
            }
            break;
        }
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_CODE_THIRD_LETTER:
        {
            if (p == ' ')
            {
                if (m_isParsingMultilinePackage)
                {
                    //多行响应的结束行
                    m_isMultiLineEnd = true;
                    m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_SPACE_BEFORE_TEXT;
                }
                else
                {
                    m_isSingleResponse = true;
                    m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_SPACE_BEFORE_TEXT;
                }

            }
            else if (p == '-')
            {
                m_isSingleResponse = false;
                m_isMultiLineEnd = false;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_MINUS_BEFORE_TEXT;

                m_isParsingMultilinePackage = true;
            }
            else
            {
                return DecodePackageResult::Error;
            }
            break;
        }
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_SPACE_BEFORE_TEXT:
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_MINUS_BEFORE_TEXT:
        {
            if (p != '\r' && p != '\n')
            {
                m_reponseTextStart = m_prasePos;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_TEXT;
            }
            else
            {
                return DecodePackageResult::Error;
            }
            break;
        }
        case FTP_PARSE_STATUS::FTP_PARSE_STATUS_TEXT:
        {

            if (p == '\r')
            {

                m_reponseTextEnd = m_prasePos - 1;
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_ALMOST_END;
            }
            else
            {
                currentTextLength++;
                if (currentTextLength > MAX_REPONSE_LINE_TEXT_LENGTH)
                    return DecodePackageResult::Error;
            }
            break;
        }
        case  FTP_PARSE_STATUS::FTP_PARSE_STATUS_ALMOST_END:
        {
            if (p != '\n')
            {
                resetStatus();
                return DecodePackageResult::Error;
            }
            else
            {
                m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_STATUS_END;
                if (m_isSingleResponse)
                {
                    ResponseLine reponseLine;
                    reponseLine.statusCode =
                        MAKE_STATUS_CODE(m_statusFirstLetter, m_statusSecondLetter, m_statusThirdLetter);

                    reponseLine.statusText = buf.substr(m_reponseTextStart, m_reponseTextEnd + 1);
                    reponseLine.isEnd = true;
                    responseLines.push_back(reponseLine);

                    resetStatus();
                    //继续解下一个包
                }
                else
                {
                    ResponseLine reponseLine;
                    reponseLine.statusCode =
                        MAKE_STATUS_CODE(m_statusFirstLetter, m_statusSecondLetter, m_statusThirdLetter);

                    reponseLine.statusText = buf.substr(m_reponseTextStart, m_reponseTextEnd + 1);

                    reponseLine.isEnd = m_isMultiLineEnd ? true : false;
                    responseLines.push_back(reponseLine);

                    resetStatus();
                }
            }
            break;
        }
        default:
        return DecodePackageResult::Error;
        }//end switch
    }//end for-loop

    size_t charCount = 0;
    for (const auto& line : responseLines)
    {
        charCount += line.statusText.length() + STATUS_CODE_LENGTH +
            SPACE_OR_MINUS_LENGTH + FTP_RESPONSE_LINE_END_FLAG_LAENGTH;

        if (line.isEnd)
        {
            buf.erase(0, charCount);
            m_prasePos = 0;
            return DecodePackageResult::Success; //至少解出一个包
        }
    }

    return DecodePackageResult::ExpectMore;
}
void ProtocolParser::resetStatus()
{
    m_parseStatus = FTP_PARSE_STATUS::FTP_PARSE_START;
    m_isMultiLineEnd = false;

}
//end func

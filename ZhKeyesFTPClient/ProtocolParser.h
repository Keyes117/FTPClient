/**
 * @desc:   协议解析类，ProtocolParser.h
 * @author: ZhKeyes
 * @date:   2025/4/24
 */

#ifndef NET_PROTOCOLPARSER_H_
#define NET_PROTOCOLPARSER_H_

#include <string>
#include <vector>

enum class DecodePackageResult
{
    Success = 0,
    Error,
    ExpectMore
};

enum class FTP_PARSE_STATUS
{
    FTP_PARSE_START,
    FTP_PARSE_STATUS_CODE_FIRST_LETTER,
    FTP_PARSE_STATUS_CODE_SECOND_LETTER,
    FTP_PARSE_STATUS_CODE_THIRD_LETTER,
    FTP_PARSE_STATUS_SPACE_BEFORE_TEXT,
    FTP_PARSE_STATUS_MINUS_BEFORE_TEXT,
    FTP_PARSE_STATUS_TEXT,
    FTP_PARSE_STATUS_ALMOST_END, // \r
    FTP_PARSE_STATUS_END        // \n
};

struct ResponseLine
{
    int16_t     statusCode;
    std::string statusText;
    bool        isEnd;

public:
    ResponseLine() = default;
    ~ResponseLine() = default;
};

class ProtocolParser
{
public:
    ProtocolParser() = default;
    ~ProtocolParser() = default;

    /**
     * @brief 解析协议
     * @param buf 待解析的协议数据，解析成功后buf 变成剩余的数据，如没有剩余则为空
     * @param resultLines 解析成功后，存放每一行解析行的FTP消息
     * @return
     */
     //DecodePackageResult praseFTPResponse(std::string& buf, std::vector<std::string>& resultLines);


     /**
      * @brief 原位解析FTP报文
      * @param buf
      * @param resultLines
      * @return
      */
    DecodePackageResult praseFTPResponse(std::string& buf, std::vector<ResponseLine>& responseLines);

private:
    void resetStatus();

private:
    FTP_PARSE_STATUS m_parseStatus{ FTP_PARSE_STATUS::FTP_PARSE_START };
    size_t          m_prasePos{ 0 };


    char            m_statusFirstLetter;
    char            m_statusSecondLetter;
    char            m_statusThirdLetter;

    bool            m_isSingleResponse;
    bool            m_isParsingMultilinePackage;
    bool            m_isMultiLineEnd{ false };

    size_t          m_reponseTextStart{ 0 };
    size_t          m_reponseTextEnd{ 0 };


};

#endif //!NET_PROTOCOLPARSER_H_

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

// |   all message length     |  flags  |  nick length  |     nickname      |   message    |
// |      0     |      1      |    2    |       4       |    5    |    M    |   N   |      |

class Message
{
public:
    Message(const char *_data, const int _length);
    ~Message();

    std::string body() const { if (false == m_isParsed) parse(); return m_body; }
    std::string nickname() const { if (false == m_isParsed) parse(); return m_nick; }

    bool isClose() const   { return m_rawData[3] & 0x4; }
    bool isLogin() const   { return m_rawData[3] & 0x2; }
    bool isMessage() const { return m_rawData[3] & 0x1; }

    const char *rawData() const { return m_rawData; }
    int rawDataLength() const { return m_rawDataLength;}

private:
    void parse() const;

private:
    mutable bool m_isParsed;

    mutable std::string m_body;
    mutable std::string m_nick;
    mutable int m_nickRawDataLength;

    char *m_rawData;
    int m_rawDataLength;
};

#endif // MESSAGE_H

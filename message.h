#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include <memory>

// |   all message length     |  flags  |  nick length  |     nickname      |   message    |
// |      0     |      1      |    2    |       4       |    5    |    M    |   N   |      |

/// 1 - login service type
/// 2 - close service type


class Message
{
public:
    Message(const char *_data, const int _length);
    Message(const std::string &_nickname, const std::string &_body, const bool isMessageType = true, const int _serviceType = 0);
    ~Message();

    std::string body() const { if (false == m_isParsed) parse(); return m_body; }
    std::string nickname() const { if (false == m_isParsed) parse(); return m_nick; }

    bool isService() const   { return !isMessage(); }
    bool isMessage() const { return (m_rawData[2] & 0x01) == 1; }
    bool isLogin() const { return isService() && ((m_rawData[2] & 0x10) > 0); }
    bool isClose() const { return isService() && ((m_rawData[2] & 0x20) > 0); }

    const char *rawData() const { return m_rawData; }
    int rawDataLength() const { return m_rawDataLength;}

//private:
    void parse() const;

private:
    mutable bool m_isParsed;

    mutable std::string m_body;
    mutable std::string m_nick;
    mutable int m_nickRawDataLength;

    std::shared_ptr<char> ptr;

    char *m_rawData;
    int m_rawDataLength;
};

Message createLoginMessage(const std::string &_nickname);
Message createCloseMessage(const std::string &_nickname);

#endif // MESSAGE_H

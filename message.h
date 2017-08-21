#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message
{
public:
    Message(const char _type, const std::string& _body);

    std::string body() const;

    bool isClose() const;
    bool isLogin() const;
    bool isMessage() const;

private:
    std::string m_body;
    char m_type;
};

#endif // MESSAGE_H

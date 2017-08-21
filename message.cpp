#include "message.h"


Message::Message(const char _type, const std::string &_body):
    m_type(_type), m_body(_body)
{

}

std::string Message::body() const
{
    return m_body;
}

bool Message::isClose() const
{
    return m_type == 0x1;
}

bool Message::isLogin() const
{
    return m_type == 0x2;
}

bool Message::isMessage() const
{
    return m_type == 0x4;
}

#include "message.h"

#include <cstring>

Message::Message(const char *_data, const int _length):
    m_isParsed(false)
{
    m_rawDataLength = _length;

    m_rawData = (char *)malloc(m_rawDataLength);
    std::memcpy((void*)m_rawData, _data, m_rawDataLength);
}

Message::Message(const int type, const std::string &_nickname, const std::string &_body):
    m_isParsed(true), m_body(_body), m_nick(_nickname)
{
    m_rawDataLength = 4 + _nickname.length() + 1 + _body.length() + 1;

    m_rawData = (char*)malloc(m_rawDataLength);
    m_rawData[0] = m_rawDataLength >> 8 & 0xFF;
    m_rawData[1] = m_rawDataLength & 0xFF;
    /// TODO: type
    m_rawData[2] = 0x04;
    m_rawData[3] = _nickname.length() + 1;
    std::memcpy((void*)m_rawData+4, (const void*) _nickname.c_str(), _nickname.length()+1);
    std::memcpy((void*)m_rawData+5 +_nickname.length(), (const void*)_body.c_str(), _body.length()+1);

}

Message::~Message()
{
    free(m_rawData);
}

void Message::parse() const
{
    m_isParsed = true;
    if (isClose()) {
        return;
    }

    m_nickRawDataLength = (int)m_rawData[3];

    m_nick = std::string((const char*)(m_rawData+4));
    m_body = std::string((const char*)(m_rawData + 4 + m_nickRawDataLength));
}

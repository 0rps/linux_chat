#include "message.h"

#include <iostream>

#include <cstring>

Message::Message(const char *_data, const int _length):
    m_isParsed(false)
{
    m_rawDataLength = _length;

    m_rawData = (char *)malloc(m_rawDataLength);
    std::memcpy((void*)m_rawData, _data, _length);

    ptr = std::shared_ptr<char>(NULL);
}

Message::Message(const int type, const std::string &_nickname, const std::string &_body):
    m_isParsed(false), m_body(_body), m_nick(_nickname)
{

    m_nickRawDataLength = std::strlen(_nickname.c_str())+1;
    m_rawDataLength = 4 + std::strlen(_body.c_str()) +1 + std::strlen(_nickname.c_str()) +1;

    const char * rawNick = _nickname.c_str();
    const char *rawBody = _body.c_str();

    m_rawData = (char*)malloc(m_rawDataLength);

    int a = (m_rawDataLength >> 8) & 0xFF;



    int b = m_rawDataLength & 0xFF;
    int c = std::strlen(_nickname.c_str());

        std::cout << a << "  ----   " << b <<   "                   = " << m_rawDataLength  << std::endl;

    m_rawData[0] = a;
    m_rawData[1] = m_rawDataLength & 0xFF;
    m_rawData[2] = (char)type;
    m_rawData[3] = std::strlen(_nickname.c_str()) + 1;
    std::memcpy((void*)m_rawData+4, (const void*) _nickname.c_str(), std::strlen(_nickname.c_str())+1);
    std::memcpy((void*)m_rawData+4 + std::strlen(_nickname.c_str()) + 1, (const void*)_body.c_str(), std::strlen(_body.c_str()) + 1);

    ptr = std::shared_ptr<char>(NULL);
}

Message::~Message()
{
    if (ptr.unique()) {
        free(m_rawData);
    }
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

#include "message.h"

#include <iostream>

#include <cstring>

const int constHeaderLength = 4;

Message::Message(const char *_data, const int _length):
    m_isParsed(false)
{
    m_rawDataLength = _length;

    m_rawData = (char *)malloc(m_rawDataLength);
    std::memcpy((void*)m_rawData, _data, _length);

    ptr = std::shared_ptr<char>(NULL);
}

Message::Message(const std::string &_nickname, const std::string &_body, const bool isMessageType, const int _serviceType):
    m_isParsed(true), m_body(_body), m_nick(_nickname)
{

    int bodyLength = m_body.length() + 1;
    m_nickRawDataLength = m_nick.length()+1;

    m_rawDataLength = constHeaderLength + m_nickRawDataLength + bodyLength;

    const char * rawNick = _nickname.c_str();
    const char *rawBody = _body.c_str();

    m_rawData = (char*)malloc(m_rawDataLength);

    m_rawData[0] = (m_rawDataLength >> 8) & 0xFF;
    m_rawData[1] = m_rawDataLength & 0xFF;
    m_rawData[2] = isMessageType ? 1:0;
    m_rawData[3] = m_nickRawDataLength;

    if (!isMessageType) {
        if (_serviceType == 1) {
            m_rawData[2] |= (char)0x10;
        } else if (_serviceType == 2) {
//            std::cout << "Close message  nick  = "<< _nickname << std::endl;
            m_rawData[2] |= (char)0x20;
        }
    }

    std::memcpy((void*)m_rawData + constHeaderLength, rawNick, m_nickRawDataLength);
    std::memcpy((void*)m_rawData + constHeaderLength + m_nickRawDataLength, rawBody, bodyLength);

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

    m_nickRawDataLength = (int)m_rawData[3];

    m_nick = std::string((const char*)(m_rawData + constHeaderLength));
    m_body = std::string((const char*)(m_rawData + constHeaderLength + m_nickRawDataLength));
}


Message createCloseMessage(const std::string &_nickname)
{
    std::string body;
    return Message(_nickname, body, false, 2);
}

Message createLoginMessage(const std::string &_nickname)
{
    std::string body;
    return Message(_nickname, body, false, 1);
}

#include "message.h"

#include <cstring>

Message::Message(const char *_data, const int _length):
    m_isParsed(false)
{
    m_rawDataLength = _length;

    m_rawData = malloc(m_rawDataLength);
    std::memcpy((void*)m_rawData, _data, m_rawDataLength);
}

void Message::parse()
{
    m_isParsed = true;
    if (isClose()) {
        return;
    }

    m_nickRawDataLength = (int)data[4];

    m_nick = std::string((const char*)(m_rawData+5));
    m_body = std::string((const char*)(m_rawData + 5 + m_nickRawDataLength));
}

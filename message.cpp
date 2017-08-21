#include "message.h"

#include <cstring>

Message::Message(const char *_data, const int _length):
    m_isParsed(false)
{
    m_rawDataLength = _length;

    m_rawData = (char *)malloc(m_rawDataLength);
    std::memcpy((void*)m_rawData, _data, m_rawDataLength);
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

    m_nickRawDataLength = (int)m_rawData[4];

    m_nick = std::string((const char*)(m_rawData+5));
    m_body = std::string((const char*)(m_rawData + 5 + m_nickRawDataLength));
}

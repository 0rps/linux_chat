#include "messageparser.h"

#include <cstring>

const int constMessageHeaderLength = 3;
const char loginType = 0x1;
const char messageType = 0x2;
const char closeType = 0x4;

MessageParser::MessageParser(): m_bufferCurrentLength(0), m_bufferCapacity(50)
{
    m_buffer = (char*)malloc(sizeof(char) * m_bufferEmptySpace);
    m_bufferEmptySpace = m_bufferCapacity;
    m_bufferRealStart = m_buffer;
}

MessageParser::~MessageParser()
{
    free(m_buffer);
}

void MessageParser::addData(const char *_data, const int _length)
{
    if (m_bufferEmptySpace < _length) {

        if (m_buffer != m_bufferRealStart) {
            std::memmove(m_bufferRealStart, m_buffer, m_bufferCurrentLength);
            m_bufferEmptySpace = m_bufferCapacity - m_bufferCurrentLength;
        }

        if (m_bufferEmptySpace < _length) {

            int newBufferCapacity = 2 * m_bufferCapacity;
            if (newBufferCapacity  - m_bufferCurrentLength < _length) {
                newBufferCapacity = m_bufferCurrentLength + _length;
            }

            m_bufferCapacity = newBufferCapacity;
            m_bufferEmptySpace = newBufferCapacity - m_bufferEmptySpace;
            m_bufferRealStart = (char*)realloc((void*)m_bufferRealStart, m_bufferCapacity * sizeof(char));
            m_buffer = m_bufferRealStart + m_bufferCurrentLength;
        }
    }

    parseBuffer();
}


bool MessageParser::hasMessage() const
{
    return m_messageQueue.size() > 0;
}

Message MessageParser::nextMessage()
{
    Message result = m_messageQueue.front();
    m_messageQueue.pop_front();
    return result;
}

void MessageParser::parseBuffer()
{
    bool m_isHandled = false;

    if (m_bufferCurrentLength >= constMessageHeaderLength) {
        char flags = m_buffer[0];

        if (flags & closeType) {
            m_messageQueue.push_back(Message(loginType, std::string()));
            m_bufferCurrentLength = 0;
            return;
        } else {
            int bodyLength = m_buffer[1] << 8 + m_buffer[2];

            if (bodyLength >= m_bufferCurrentLength) {
                m_buffer += constMessageHeaderLength;

                std::string messageBody((const char*)m_buffer );
                m_messageQueue.push_back(Message(m_buffer[0] & (char)0x0F, messageBody));

                m_bufferCurrentLength -= constMessageHeaderLength + bodyLength;

                if (m_bufferCurrentLength == 0) {
                    m_buffer = m_bufferRealStart;
                    m_bufferEmptySpace = m_bufferCapacity;
                }
            }
        }
    }

    if (m_isHandled && m_bufferCurrentLength > 0) {
        parseBuffer();
    }
}

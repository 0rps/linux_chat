#include "messageparser.h"

#include <iostream>
#include <cstring>

const int constMessageHeaderLength = 4;

MessageParser::MessageParser(): m_bufferCurrentLength(0), m_bufferCapacity(52)
{
    m_buffer = (char*)malloc(sizeof(char) * m_bufferCapacity);

    m_bufferEmptySpace = m_bufferCapacity;
    m_curBuffer = m_buffer;
}

MessageParser::~MessageParser()
{
    free(m_buffer);
}

void MessageParser::addData(const char *_data, const int _length)
{
    if (m_bufferEmptySpace < _length) {

        char *msgStart = m_curBuffer-m_bufferCurrentLength;
        if (msgStart != m_buffer) {
            std::memmove(m_buffer, msgStart, m_bufferCurrentLength);

            m_bufferEmptySpace = m_bufferCapacity - m_bufferCurrentLength;
            m_curBuffer = m_buffer + m_bufferCurrentLength;
        }

        if (m_bufferEmptySpace < _length) {

            int newBufferCapacity = 2 * m_bufferCapacity;
            int newEmptySpace = newBufferCapacity - m_bufferCurrentLength;
            if (newEmptySpace < _length) {
                newBufferCapacity = m_bufferCurrentLength + _length;
            }

            m_bufferCapacity = newBufferCapacity;
            m_bufferEmptySpace = newBufferCapacity - m_bufferCurrentLength;
            m_buffer = (char*)realloc((void*)m_buffer, m_bufferCapacity * sizeof(char));
            m_curBuffer = m_buffer + m_bufferCurrentLength;
        }
    }

    std::memcpy((void*)m_curBuffer, _data, _length);
    m_bufferCurrentLength += _length;
    m_bufferEmptySpace -= _length;
    m_curBuffer += _length;

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
    bool isHandled = false;

    if (m_bufferCurrentLength >= constMessageHeaderLength) {
        char *msgStart = m_curBuffer - m_bufferCurrentLength;
        int msgLength = (((int)msgStart [0]) << 8) | (int)msgStart [1];
        if (msgLength <= m_bufferCurrentLength) {
            m_messageQueue.push_back(Message(msgStart , msgLength));
            m_bufferCurrentLength -= msgLength;
            if (m_bufferCurrentLength == 0) {
                m_curBuffer = m_buffer;
                m_bufferEmptySpace = m_bufferCapacity;
            } else {
                isHandled = true;
            }

        }
    }

    if (isHandled) {
        parseBuffer();
    }
}

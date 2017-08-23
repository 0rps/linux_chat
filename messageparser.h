#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <string>
#include <list>

#include <message.h>

class MessageParser
{
public:
    MessageParser();
    ~MessageParser();

    void addData(const char *_data, const int _length);

    bool hasMessage() const;
    Message nextMessage();

private:
    void parseBuffer();

private:
    int m_bufferCurrentLength;
    int m_bufferCapacity;
    char *m_buffer;


    char *m_curBuffer;
    int m_bufferEmptySpace;



    std::list<Message> m_messageQueue;

};

#endif // MESSAGEPARSER_H

#include <iostream>

#include "linxer/StringRingBuffer.h"

using namespace std;

StringRingBuffer::StringRingBuffer(int capacity)
    : buffer(capacity)
    , lines(0)
{}

void StringRingBuffer::push_back(const std::string& str)
{
    if (buffer.capacity() == 0)
    {
        return;
    }
    buffer[lines % buffer.capacity()] = str;
    ++lines;
}

void StringRingBuffer::push_back(std::string&& str)
{
    if (buffer.capacity() == 0)
    {
        return;
    }
    buffer[lines % buffer.capacity()] = move(str);
    ++lines;
}

void StringRingBuffer::append_back(const std::string& str)
{
    if (buffer.capacity() == 0)
    {
        return;
    }
    buffer[(lines + buffer.capacity() - 1) % buffer.capacity()].append(str);
}

std::string StringRingBuffer::pop_back()
{
    if (buffer.capacity() == 0)
    {
        return "";
    }
    auto res = move(buffer[(lines + buffer.capacity() - 1) % buffer.capacity()]);
    --lines;
    return res;
}

const std::string* StringRingBuffer::peek(int index)
{
    if (buffer.capacity() == 0)
    {
        return nullptr;
    }

    // Index to high (i.e. line does not exist)
    if (index >= lines)
    {
        return nullptr;
    }

    // Positive indexes are made negative, to handle a single type of index (i.e. negative ones)
    if (index >= 0)
    {
        index -= lines;
    }

    // Lines that are no longer in the buffer
    if (index < -buffer.capacity())
    {
        return nullptr;
    }

    return &buffer[(lines + index) % buffer.capacity()];
}

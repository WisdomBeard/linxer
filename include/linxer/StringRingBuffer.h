#pragma once

#include <string>
#include <vector>

class StringRingBuffer
{
protected:
    std::vector< std::string > buffer;
    int lines;

public:
    StringRingBuffer(int capacity);

    void push_back(const std::string& str);
    void push_back(std::string&& str);
    void append_back(const std::string& str);
    std::string pop_back();

    const std::string* peek(int index);
};

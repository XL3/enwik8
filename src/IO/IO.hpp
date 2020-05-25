#pragma once
#include <fstream>

typedef unsigned int u32;
typedef unsigned char u8;

class OutputStream
{
    bool m_closed;
    u32 m_buffer;
    u8 m_freeBit;
    u8 m_BUFFER_SIZE;
    std::ofstream m_stream;

public:
    OutputStream(const char* filename, u8 bufferSize = 32);
    ~OutputStream();
    void write(u32 value, u8 bits);
    void close();
};

class InputStream
{
    bool m_closed;
    u32 m_buffer;
    u8 m_freeBit;
    u8 m_BUFFER_SIZE;
    std::ifstream m_stream;

public:
    InputStream(const char* filename, u8 bufferSize = 32);
    ~InputStream();
    bool read(u32 &value, u8 bits);
    void close();
};

void test_IO();

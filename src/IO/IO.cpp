#include "IO.hpp"

#define MIN(a, b) ((a < b) ? a : b)

OutputStream::OutputStream(const char* filename, u8 bufferSize) :
    m_closed(false), m_buffer(0), m_freeBit(0), m_BUFFER_SIZE(bufferSize)
{
    m_stream = std::ofstream(filename, std::ios_base::binary | std::ios_base::out);
}

OutputStream::~OutputStream() {
    if (!m_closed) close();
}

void OutputStream::write(u32 value, u8 bits)
{
    const u32 BUFFER_MASK = (1ull << m_BUFFER_SIZE) - 1;
    u32 bitsToWrite = 0;
    u8 numBitsWritten = 0;
    while (bits) {
        // Place as many bits into the buffer from the value as you can
        // Mask by amount of bits needed to read
        bitsToWrite = value & BUFFER_MASK & ((1ull << bits) - 1);
        m_buffer += bitsToWrite << m_freeBit;

        // Shift the bits that were read out of the value
        // Read only remaining bits
        numBitsWritten = MIN(bits, m_BUFFER_SIZE - m_freeBit);
        value >>= numBitsWritten;
        bits -= numBitsWritten;
        m_freeBit += numBitsWritten;

        // If the buffer is full,
        // write it to stream and clear it
        if (m_freeBit == m_BUFFER_SIZE) {
            m_stream.write((char*)&m_buffer, m_BUFFER_SIZE/8);
            m_buffer = 0;
            m_freeBit = 0;
        }
    }
}

void OutputStream::close() {
    if (!m_closed) {
        m_closed = true;
        if (m_freeBit) {
            // TODO Maybe only write as many bytes as we need
            m_stream.write((char*)&m_buffer, m_BUFFER_SIZE/8);
        }
        m_stream.close();
    }
}

InputStream::InputStream(const char* filename, u8 bufferSize) :
    m_closed(false), m_buffer(0), m_freeBit(0), m_BUFFER_SIZE(bufferSize)
{
    m_stream = std::ifstream(filename, std::ios_base::binary | std::ios_base::in);
}

InputStream::~InputStream()
{
    if (!m_closed) close();
}

bool InputStream::read(u32 &value, u8 bits)
{
    if (m_stream.eof()) return false;

    value = 0;
    u32 bitsToRead = 0;
    u8 numBitsRead = 0;

    while (bits) {
        // Fill up buffer from input stream
        if (!m_freeBit) {
            m_stream.read((char*)&m_buffer, m_BUFFER_SIZE/8);
            if (m_stream.eof()) return false;
            m_freeBit = m_BUFFER_SIZE;
        }

        // Read as many bits as you can from the buffer
        // Place read bits properly into value
        bitsToRead = m_buffer & ((1ull << MIN(bits, m_freeBit)) - 1);
        value += bitsToRead << numBitsRead;
        numBitsRead = MIN(bits, m_freeBit);

        // Shift read bits out of buffer
        // Read only remaining bits
        m_buffer >>= numBitsRead;
        m_freeBit -= numBitsRead;
        bits -= numBitsRead;
    }
    return true;
}

void InputStream::close()
{
    if (!m_closed) {
        m_closed = true;
        m_stream.close();
    }
}

void test_IO()
{
    const char* filename = "../bin/io";
    OutputStream ostr(filename);
    ostr.write(0xFAC688, 24);
    ostr.close();

    InputStream istr(filename, 24);
    u32 value = 0;
    while (istr.read(value, 3))
        printf("%X\n", value);
    istr.close();
}

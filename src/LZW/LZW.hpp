#pragma once
#include <unordered_map>
#include <fstream>

typedef unsigned char u8;
typedef unsigned int u32;
typedef std::unordered_map<std::string, u32> msu32;
typedef std::unordered_map<u32, std::string> mu32s;

class LZW
{
public:
    // Encodes a given buffer
    static bool encode(const char* buffer, const char* encoded);

    // Decodes a given buffer
    static bool decode(const char* buffer, std::ofstream &decoded);
};

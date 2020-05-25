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
    static bool encode(std::string buffer, std::string encoded);

    // Decodes a given buffer
    static bool decode(std::string buffer, std::ofstream &decoded);
};

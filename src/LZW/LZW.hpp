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
    // Maximum dictionary size
    static unsigned int MAX_CODE;

    // Encodes a given buffer
    static bool encode(std::ifstream &buffer, std::ofstream &encoded);

    // Decodes a given buffer
    static bool decode(std::ifstream &buffer, std::ofstream &decoded);
};

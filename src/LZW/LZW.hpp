#pragma once
#include <unordered_map>
#include <iostream>
#include <fstream>

typedef unsigned int u32;
typedef unsigned short u16;
typedef std::unordered_map<std::string, u16> msu16;
typedef std::unordered_map<u16, std::string> mu16s;
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

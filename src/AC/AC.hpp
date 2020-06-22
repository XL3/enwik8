#pragma once
#include <unordered_map>
#include <fstream>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

// Arithmetic Coding
class AC
{
public:
    // Encodes a given buffer
    static bool encode(const char* buffer, const char* encoded);

    // Decodes a given buffer
    static bool decode(const char* buffer, const char* decoded);
};

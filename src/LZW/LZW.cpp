#include "LZW.hpp"

unsigned int LZW::MAX_CODE = 1u << 15;

bool LZW::encode(std::ifstream &buffer, std::ofstream &encoded)
{
    printf("Encoding..\n");
    msu32 table;
    // Initialize lookup table with all symbols
    for (u32 i = 0; i < 0x100; i++) {
        table[std::string(1, char(i))] = i;
    }
    u32 lastCode = 0x100;

    char c;
    if (!buffer.get(c)) return false;

    std::string match(1, c);
    u32 progress = buffer.tellg() >> 20;
    while (buffer.get(c)) {
        // Progress
        if (buffer.tellg() >> 20 > progress) {
            progress = buffer.tellg() >> 20;
            printf("%d MiB done\n", progress);
        }

        // Find match
        std::string nextMatch = match;
        nextMatch.push_back(c);
        if (table.find(nextMatch) != table.end()) {
            match = nextMatch;
        }
        else {
            // Encode match
            u32 code = table[match];
            encoded.put(code & 0xFF);
            encoded.put((code >> 8) & 0xFF);
            encoded.put((code >> 16) & 0xFF);
            encoded.put((code >> 24) & 0xFF);

            // Append match + next undefined symbol
            table[nextMatch] = lastCode++;

            // Start next match with symbol
            match = std::string(1, c);
        }
    }
    // Encode last match
    u32 code = table[match];
    encoded.put(code & 0xFF);
    encoded.put((code >> 8) & 0xFF);
    encoded.put((code >> 16) & 0xFF);
    encoded.put((code >> 24) & 0xFF);
    return true;
}

bool LZW::decode(std::ifstream &buffer, std::ofstream &decoded)
{
    printf("Decoding...\n");
    mu32s table;
    // Initialize lookup table with all symbols
    for (u32 i = 0; i < 0x100; i++) {
        table[i] = std::string(1, char(i));
    }
    u32 lastCode = 0x0100;

    // Start with first code
    u32 previousCode = 0x0000;
    if (!buffer.read((char*) &previousCode, 4)) return false;

    // Decode first symbol
    decoded << table[previousCode];

    u32 code = 0x0000;                    // Current read code
    char c = table[previousCode][0];      // First read character in current match
    u32 progress = buffer.tellg() >> 20;
    while (buffer.read((char*) &code, 4)) {
        // Progress
        if (buffer.tellg() >> 20 > progress) {
            progress = buffer.tellg() >> 20;
            printf("%d MiB done\n", progress);
        }

        // Match the code
        std::string match;
        auto itr = table.find(code);
        if (itr == table.end()) {
            match = table[previousCode];
            match.push_back(c);
        }
        else {
            match = table[code];
        }

        // Decode the match
        decoded << match;
        c = match[0];

        // Append previous match + first read character
        table[lastCode] = table[previousCode];
        table[lastCode++].push_back(c);
        previousCode = code;
    }
    return true;
}

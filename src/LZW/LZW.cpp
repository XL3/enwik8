#include "LZW.hpp"
#include "../IO/IO.hpp"

bool LZW::encode(const char* buffer, const char* encoded)
{
    InputStream buf(buffer);
    OutputStream enc(encoded);

    printf("Encoding..\n");
    msu32 table;
    // Initialize lookup table with all symbols
    for (u32 i = 0; i < 0x100; i++) {
        table[std::string(1, char(i))] = i;
    }
    table[std::string("\x01\x00")] = 0x100;
    u32 lastCode = 0x101;
    u8 bitsPerCode = 9;

    u32 c = 0;
    if (!buf.read(c, 8)) return false;

    std::string match(1, c);
    while (buf.read(c, 8)) {
        // Find match
        std::string nextMatch = match;
        nextMatch.push_back(c);
        if (table.find(nextMatch) != table.end()) {
            match = nextMatch;
        }
        else {
            // Encode match
            u32 code = table[match];
            enc.write(code, bitsPerCode);

            // Append match + next undefined symbol
            table[nextMatch] = lastCode++;
            if (lastCode == (1u << bitsPerCode)) bitsPerCode++;

            // Start next match with symbol
            match = std::string(1, c);
        }
    }
    // Encode last match
    u32 code = table[match];
    enc.write(code, bitsPerCode);

    // Encode EOF
    u32 eof = 0x100;
    enc.write(eof, bitsPerCode);
    
    return true;
}

bool LZW::decode(const char* buffer, std::ofstream &decoded)
{
    InputStream buf(buffer);

    printf("Decoding...\n");
    mu32s table;
    // Initialize lookup table with all symbols
    for (u32 i = 0; i < 0x100; i++) {
        table[i] = std::string(1, char(i));
    }
    table[0x100] = std::string("\x01\x00");
    u32 lastCode = 0x101;
    u8 bitsPerCode = 9;

    // Start with first code
    u32 previousCode = 0x0000;
    if (!buf.read(previousCode, bitsPerCode)) return false;

    // Decode first symbol
    decoded << table[previousCode];

    u32 code = 0x0000;                    // Current read code
    char c = table[previousCode][0];      // First read character in current match
    while (buf.read(code, bitsPerCode)) {
        // EOF
        if (code == 0x100) return true;

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
        if (lastCode+1 == (1u << bitsPerCode)) bitsPerCode++;

        previousCode = code;
        code = 0x0000;
    }
    return true;
}

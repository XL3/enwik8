#include "AC.hpp"
#include "../IO/IO.hpp"

bool AC::encode(const char* buffer, const char* encoded)
{
    OutputStream enc(encoded);

    // Collect symbol occurence frequency
    printf("Encoding..\n");
    u32 symbol = 0x00000000;
    u32 freq[0x100] = {};

    {
        InputStream buf(buffer);
        while (buf.read(symbol, 8)) freq[symbol]++;
        buf.close();
    }

    for (u32 c = 1; c < 0x100; c++) {
        freq[c] += freq[c - 1];
    }
    const u32 totalCount = freq[0xFF];

    struct {
        const u32 low = 0x40000000u;
        const u32 half = 0x80000000u;
        const u32 high = 0xC0000000u;
    } LIMITS;

    u32 upper = 0xFFFFFFFFu;
    u32 lower = 0x00000000u;
    u64 range;

    u8 numHangingBits = 0;
    const auto write_bits = [&](u8 bit) {
        enc.write(bit, 1);
        while (numHangingBits--) enc.write(bit ? 0 : 1, 1);
        numHangingBits = 0;
    };

    InputStream buf(buffer);
    while (buf.read(symbol, 8)) {
        range = upper - lower + (u64)1;
        upper = lower + (range * freq[symbol]) / totalCount - 1;
        lower = lower + (range * freq[symbol-1]) / totalCount;

        while (true) {
            if (upper < LIMITS.half) {
                write_bits(0);
                lower <<= 1;
                upper <<= 1; upper |= 1;
            }
            else if (lower >= LIMITS.half) {
                write_bits(1);
                lower <<= 1;
                upper <<= 1; upper |= 1;
            }
            else if (lower >= LIMITS.low && upper < LIMITS.high) {
                numHangingBits++;
                lower <<= 1; lower &= (LIMITS.half-1);
                upper <<= 1; upper |= (LIMITS.half+1);
            }
            else break;
        }
    }
    return true;
}

bool AC::decode(const char* buffer, const char* decoded)
{
    InputStream buf(buffer);
    OutputStream dec(decoded);

    u32 tag;
    buf.read(tag, 32);
    dec.write(tag, 32);

    printf("Decoding..\n");
    return true;
}
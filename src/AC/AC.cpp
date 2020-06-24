#include "AC.hpp"
#include "../IO/IO.hpp"

bool AC::encode(const char* buffer, const char* encoded)
{
    InputStream buf(buffer);
    OutputStream enc(encoded);

    // Initialize cumulative frequency
    u32 cfreq[0x101] = {};
    const u32 __EOF = 0x100u;
    for (u32 i = 1; i < 0x101; ++i) cfreq[i] = 1 + cfreq[i-1];

    const auto update_cfreq = [&](u32 symbol) {
        for (u32 i = symbol; i < 0x101; ++i) {
            cfreq[i]++;
        }
    };

    struct {
        const u32 low = 0x40000000u;
        const u32 half = 0x80000000u;
        const u32 high = 0xC0000000u;
    } LIMITS;

    u32 upper = 0xFFFFFFFFu;
    u32 lower = 0x00000000u;

    u32 numHangingBits = 0;
    const auto write_bits = [&](u32 bit) {
        enc.write(bit, 1);
        enc.write(bit ? 0 : ~bit, numHangingBits);
        numHangingBits = 0;
    };

    u32 symbol;
    const auto encode_symbol = [&](u32 s) {
        const u64 range = upper - lower + (u64)1;
        upper = lower + (range * cfreq[s]) / cfreq[__EOF] - 1;
        lower = lower + (range * cfreq[s-1]) / cfreq[__EOF];

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

                upper <<= 1; upper |= (LIMITS.half + 1);
                lower <<= 1; lower &= (LIMITS.half - 1);
            }
            else break;
        }
    };

    printf("Encoding..\n");
    while (buf.read(symbol, 8)) {
        encode_symbol(symbol);
        update_cfreq(symbol);
    }
    encode_symbol(__EOF);

    // Terminate encoding
    for (u32 i = 0; i < 32; i++) {
        u32 b = (lower & LIMITS.half) >> 31; // MSB to LSB
        write_bits(b);

        lower <<= 1;
    }

    return true;
}

bool AC::decode(const char* buffer, const char* decoded)
{
    InputStream buf(buffer);
    OutputStream dec(decoded);

    // Initialize cumulative frequency
    u32 cfreq[0x101] = {};
    const u32 __EOF = 0x100;
    for (u32 i = 1; i < 0x101; ++i) cfreq[i] = 1 + cfreq[i-1];

    const auto update_cfreq = [&](u32 symbol) {
        for (u32 i = symbol; i < 0x101; ++i) {
            cfreq[i]++;
        }
    };
    const auto get_symbol = [&](u64 subfreq) {
        for (u32 s = 1; s < 0x101; s++) {
            if (subfreq < cfreq[s]) return s;
        }
        return 0x101u;
    };


    struct {
        const u32 low = 0x40000000u;
        const u32 half = 0x80000000u;
        const u32 high = 0xC0000000u;
    } LIMITS;

    u32 upper = 0xFFFFFFFFu;
    u32 lower = 0x00000000u;

    u32 tag;
    const auto update_tag = [&]() {
        u32 bit = 0;
        if (buf.read(bit, 1)) {
            tag <<= 1;
            if (bit & 1) tag |= 1;
            return true;
        }
        return false;
    };

    // Reverse tag bits to proper order
    buf.read(tag, 32); {
        u32 rev = 0;
        for (u32 i = 0; i < 32; i++) {
            rev <<= 1;
            if (tag & 1) rev |= 1;
            tag >>= 1;
        }
        tag = rev;
    }

    printf("Decoding..\n");
    bool readingFromFile = true;
    while (readingFromFile) {
        const u64 range = upper - lower + (u64)1;
        const u64 subrange = tag - lower + (u64)1;
        const u64 subfreq = (subrange * cfreq[__EOF] - 1) / range;

        u32 symbol = get_symbol(subfreq);

        if (symbol == __EOF) return true;
        dec.write(symbol, 8);

        upper = lower + (range * cfreq[symbol]) / cfreq[__EOF] - 1;
        lower = lower + (range * cfreq[symbol-1]) / cfreq[__EOF];
        update_cfreq(symbol);

        while (true) {
            if (upper < LIMITS.half || lower >= LIMITS.half) {
                upper <<= 1; upper |= 1;
                lower <<= 1;

                readingFromFile = update_tag();
            } 
            else if (lower >= LIMITS.low && upper < LIMITS.high) {
                upper <<= 1; upper |= (LIMITS.half+1);
                lower <<= 1; lower &= (LIMITS.half-1);

                readingFromFile = update_tag();
                tag ^= LIMITS.half;
            }
            else break;
        }
    }
    return true;
}
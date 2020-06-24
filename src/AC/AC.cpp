#include "AC.hpp"
#include "../IO/IO.hpp"

const u32 __EOF = 0x100;

ModelInterface *AC::model = nullptr;

bool AC::encode(const char* buffer, const char* encoded)
{
    InputStream buf(buffer);
    OutputStream enc(encoded);
    model->reset_model();

    // Range limits, bounds
    struct {
        const u32 low = 0x40000000u;
        const u32 half = 0x80000000u;
        const u32 high = 0xC0000000u;
    } LIMITS;
    u32 upper = 0xFFFFFFFFu;
    u32 lower = 0x00000000u;


    u32 numHangingBits = 0;
    // Writes hanging bits to the bitstream
    const auto write_bits = [&](u32 bit) {
        enc.write(bit, 1);
        enc.write(bit ? 0 : ~bit, numHangingBits);
        numHangingBits = 0;
    };

    u32 symbol;
    // Encodes a given symbol, updates range bounds
    const auto encode_symbol = [&](u32 s) {
        const u64 range = upper - lower + (u64)1;
        upper = lower + (range * model->get_frequency(s)) / model->get_frequency(__EOF) - 1;
        lower = lower + (range * model->get_frequency(s-1)) / model->get_frequency(__EOF);

        while (true) {
            // Converging above
            if (upper < LIMITS.half) {
                write_bits(0);

                lower <<= 1;
                upper <<= 1; upper |= 1;
            }
            // Converging above
            else if (lower >= LIMITS.half) {
                write_bits(1);

                lower <<= 1;
                upper <<= 1; upper |= 1;
            }
            // Almost converging
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
        model->update_model(symbol);
    }
    encode_symbol(__EOF);

    // Terminate encoding by writing the range's lower bound
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
    model->reset_model();

    // Range limits, bounds
    struct {
        const u32 low = 0x40000000u;
        const u32 half = 0x80000000u;
        const u32 high = 0xC0000000u;
    } LIMITS;
    u32 upper = 0xFFFFFFFFu;
    u32 lower = 0x00000000u;

    u32 tag;
    // Reads from the bitstream into the tag
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
        const u64 subfreq = (subrange * model->get_frequency(__EOF) - 1) / range;

        u32 symbol = model->get_symbol(subfreq);
        if (symbol == __EOF) return true;
        dec.write(symbol, 8);

        upper = lower + (range * model->get_frequency(symbol)) / model->get_frequency(__EOF) - 1;
        lower = lower + (range * model->get_frequency(symbol-1)) / model->get_frequency(__EOF);
        model->update_model(symbol);

        while (true) {
            // Converging
            if (upper < LIMITS.half || lower >= LIMITS.half) {
                upper <<= 1; upper |= 1;
                lower <<= 1;

                readingFromFile = update_tag();
            } 
            // Almost converging
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
#include <fstream>
#include "LZW/LZW.hpp"

int main(int argc, char** argv)
{
    if (argc > 1 && std::string(argv[1]) == std::string("encode")) {
        // Encode file
        auto buffer = std::ifstream ("../bin/enwik8");
        std::ofstream encoded("../bin/encoded", std::ios_base::binary | std::ios_base::out);
        if (!LZW::encode(buffer, encoded)) return -1;
        buffer.close();
        encoded.close();
    }
    else if (argc > 1 && std::string(argv[1]) == std::string("decode")) {
        // Decode file
        auto buffer = std::ifstream("../bin/encoded", std::ios_base::binary | std::ios_base::in);
        std::ofstream decoded("../bin/decoded", std::ios_base::binary | std::ios_base::out);
        if (!LZW::decode(buffer, decoded)) return -1;
        buffer.close();
        decoded.close();
    }
    else {
        printf("USAGE: enwik8 encode|decode\n");
    }

    return 0;
}

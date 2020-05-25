#include <fstream>
#include "LZW/LZW.hpp"
#include "IO/IO.hpp"

int main(int argc, char** argv)
{
    if (argc > 1 && std::string(argv[1]) == std::string("encode")) {
        // Encode file
        if (!LZW::encode(std::string("../bin/enwik8"), std::string("../bin/encoded"))) return -1;
    }
    else if (argc > 1 && std::string(argv[1]) == std::string("decode")) {
        // Decode file
        std::ofstream decoded("../bin/decoded", std::ios_base::binary | std::ios_base::out);
        if (!LZW::decode(std::string("../bin/encoded"), decoded)) return -1;
        decoded.close();
    }
    else {
        printf("USAGE: enwik8 encode|decode\n");
    }

    return 0;
}

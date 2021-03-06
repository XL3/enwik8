#include <fstream>
/* #include "LZW/LZW.hpp" */
#include "AC/AC.hpp"
#include "IO/IO.hpp"

int main(int argc, char** argv)
{
    if (argc > 3 && !strcmp(argv[1], "encode")) {
        // Encode file
        if (!AC::encode(argv[2], argv[3])) return 0xFF;

        // if (!LZW::encode(argv[2], argv[3])) return -1;
    }
    else if (argc > 3 && !strcmp(argv[1], "decode")) {
        // Decode file
        if (!AC::decode(argv[2], argv[3])) return 0xFF;

        // std::ofstream decoded(argv[3], std::ios_base::binary | std::ios_base::out);
        // if (!LZW::decode(argv[2], decoded)) return -1;
        // decoded.close();
    }
    else {
        printf("USAGE: enwik8 encode|decode path/to/input path/to/output\n");
    }

    return 0;
}

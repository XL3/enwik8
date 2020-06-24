#include <fstream>
#include "Model/StaticModel.hpp"
#include "IO/IO.hpp"
#include "AC/AC.hpp"

int main(int argc, char** argv)
{
    AC::model = new StaticModel();
    if (argc > 3 && !strcmp(argv[1], "encode")) {
        if (!AC::encode(argv[2], argv[3])) return 0xFF;
    }
    else if (argc > 3 && !strcmp(argv[1], "decode")) {
        if (!AC::decode(argv[2], argv[3])) return 0xFF;
    }
    else {
        printf("USAGE: enwik8 encode|decode path/to/input path/to/output\n");
    }

    return 0;
}

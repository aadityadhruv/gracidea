#include "rs/api.h"
#include <string>
#include "cli.h"
#include "api.h"
#include <assert.h>
#include <stdio.h>

extern std::string file_path;
std::string file_path;

int main(int argc, char** argv) {
    fprintf(stderr, "Starting gracidea...%s\n", argv[1]);
    file_path = std::string(argv[1]);
    rs::RSAPI rsapi = rs::RSAPI();
    assert(parse_args(argc - 1, argv + 1, rsapi) == 0);
    return 0;
}

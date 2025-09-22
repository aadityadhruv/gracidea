#include "rs/api.h"
#include "cli.h"
#include "api.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

char* file_path;
struct pokeapi* api;
int main(int argc, char** argv) {
    fprintf(stderr, "Starting gracidea...%s", argv[1]);
    file_path = argv[1];
    struct pokeapi rsapi = {
        .box_view = rs_box_view,
        .box_edit = NULL,
        .box_new = NULL,
        .party_view = NULL,
        .party_new = NULL,
        .party_edit = NULL,
        .bag_new = NULL,
        .bag_new = NULL,
    };

    api = &rsapi;

    assert(parse_args(argc - 1, argv + 1) == 0);

    return 0;
}

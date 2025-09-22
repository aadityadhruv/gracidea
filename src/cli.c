//
// gracidea
//   box
//      view <ID>
//      edit <box> <ID>
//      new <species> <box> <id>
//   party
//     view
//     edit <ID>
//     new <species> <id>
//   bag
//     view <section>
//     new <item> <quantity>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "api.h"

extern struct pokeapi* api;


int parse_args(int argc, char** argv) {
    assert(argc >= 3);

    char* mode = argv[1];
    char* action = argv[2];
    char** params = argv + 3;

    if (strcmp(mode, "box") == 0) {
        return handle_box(argc - 3, action, params);
    }

    if (strcmp(mode, "party") == 0) {
        return handle_party(argc - 3, action, params);
    }

    if (strcmp(mode, "bag") == 0) {
        return handle_bag(argc - 3, action, params);
    }
    return 0;
}

int handle_box(int argc, char *action, char **params) {

    if (strcmp(action, "view")) {
        assert(argc == 1);
        int box = atoi(params[0]);
        api->box_view(box);
    }

    return 0;
}

int handle_party(int argc, char *action, char **params) {return 0;}
int handle_bag(int argc, char *action, char **params) {return 0;}

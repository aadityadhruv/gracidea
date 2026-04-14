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
#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "api.h"

int parse_args(const int argc, char **argv, struct PokeAPI &api) {
    assert(argc >= 3);

    char *mode = argv[1];
    char *action = argv[2];
    char **params = argv + 3;
    fprintf(stderr, "MODE: %s, ACTION: %s\n", mode, action);

    if (strcmp(mode, "box") == 0) {
        return handle_box(argc - 3, action, params, api);
    }

    if (strcmp(mode, "party") == 0) {
        return handle_party(argc - 3, action, params, api);
    }

    if (strcmp(mode, "bag") == 0) {
        return handle_bag(argc - 3, action, params, api);
    }
    fprintf(stderr, "Invalid mode and action - %s, %s\n", mode, action);
    return -1;
}

int handle_box(int argc, char *action, char **params, struct PokeAPI &api) {
    if (strcmp(action, "view") == 0) {
        assert(argc == 1);
        int box = atoi(params[0]);
        api.box_view(box);
    } else {
        fprintf(stderr, "Invalid fields to box mode - action \"%s\"\n", action);
    }

    return 0;
}

int handle_party(int argc, char *action, char **params, struct PokeAPI &api) {
    if (strcmp(action, "view") == 0) {
        assert(argc == 0);
        api.party_view();
    } 
    else if (strcmp(action, "edit") == 0) {
        assert(argc == 1);
        int idx = atoi(params[0]);
        api.party_edit(idx);
    } else {
        fprintf(stderr, "Invalid fields to party mode - action \"%s\"\n", action);
    }
    return 0;
}

int handle_bag(int argc, char *action, char **params, struct PokeAPI &api) {
    if (strcmp(action, "view") == 0) {
        assert(argc == 1);
        api.bag_view(params[0]);
    } else if (strcmp(action, "edit") == 0) {
        assert(argc == 2);
        int quantity = atoi(params[1]);
        api.bag_edit(params[0], quantity);
    } else {
        fprintf(stderr, "Invalid fields to party mode - action \"%s\"\n", action);
    }
    return 0;
}

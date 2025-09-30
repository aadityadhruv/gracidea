#include "api.h"
#include "rs.h"
#include "util.h"
#include <stdio.h>
#include <string.h>


extern char* file_path;
extern char* pokemon_name_list[];
extern struct rs_item items_names_list[];
extern int items_size;

void rs_box_view(int num) {
    if (num < 0 || num > 14) {
        fprintf(stderr, "ERROR! Box Number must be between 0 and 14!\n");
        return;
    }
    struct file* fp;
    load_save_file(file_path, &fp);
    // struct trainer_info* trainer = get_trainer_info(fp);
    // struct player_team* team = get_player_team(fp);
    struct pc_buffer* pc = get_pc(fp);
    struct pc_pokemon* offset = pc->pokemon + (30 * (num - 1));

    for (int i = 0; i < 30; i++) {
        struct pc_pokemon pkmn = offset[i];
        __u16 id = pkmn.ot_id >> 16;
        __u16 sid = pkmn.ot_id & 0x00ff;
        struct poke_growth* info = get_poke_growth((struct pokemon*) &pkmn);
        // Not a real pokemon
        if (info->species == 0x00) {
            return;
        }
        char nickname[sizeof(pkmn.nickname) + 1];
        nickname[sizeof(pkmn.nickname)] = 0;
        char ot[sizeof(pkmn.ot) + 1];
        ot[sizeof(pkmn.ot)] = 0;
        decode_string(pkmn.nickname, sizeof(pkmn.nickname), nickname);
        decode_string(pkmn.ot, sizeof(pkmn.ot), ot);
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", i, nickname, ot, id, sid);
        char* name = pokemon_name_list[info->species];
        struct rs_item* item = &items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name, item->name);
    }
}
void rs_party_view() {
    struct file* fp;
    load_save_file(file_path, &fp);
    // struct trainer_info* trainer = get_trainer_info(fp);
    // struct player_team* team = get_player_team(fp);
    struct player_team* team = get_player_team(fp);

    for (int i = 0; i < 6; i++) {
        struct pokemon pkmn = team->pokemon[i];
        __u16 id = pkmn.ot_id >> 16;
        __u16 sid = pkmn.ot_id & 0x00ff;
        struct poke_growth* info = get_poke_growth((struct pokemon*) &pkmn);
        if (info->species == 0x00) {
            return;
        }
        char nickname[sizeof(pkmn.nickname) + 1];
        nickname[sizeof(pkmn.nickname)] = 0;
        char ot[sizeof(pkmn.ot) + 1];
        ot[sizeof(pkmn.ot)] = 0;
        decode_string(pkmn.nickname, sizeof(pkmn.nickname), nickname);
        decode_string(pkmn.ot, sizeof(pkmn.ot), ot);
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", i, nickname, ot, id, sid);
        char* name = pokemon_name_list[info->species];
        struct rs_item* item = &items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name, item->name);
    }
}

void rs_bag_view(char* section) {
    char* sections[5] = { "items", "keyitems", "balls", "tms", "berries"};

    int num = BAD_CATEGORY;
    if (strcmp(section, "items") == 0) {
        num = ITEM_CATEGORY;
    }
    else if (strcmp(section, "keyitems") == 0) {
        num = KEY_ITEM_CATEGORY;
    }
    else if (strcmp(section, "balls") == 0) {
        num = BALL_CATEGORY;
    }
    else if (strcmp(section, "tms") == 0) {
        num = TM_CATEGORY;
    }
    else if (strcmp(section, "berries") == 0) {
        num = BERRY_CATEGORY;
    }
    else {
        fprintf(stderr, "Invalid section! Options are: 'items', 'keyitems', 'balls', 'tms', 'berries'\n");
        return;
    }
    struct file* fp;
    load_save_file(file_path, &fp);
    struct player_team* team = get_player_team(fp);
    switch (num) {
        case ITEM_CATEGORY:
            fprintf(stdout, "Items\n");
            for (int i = 0; i < sizeof(team->items); i++) {
                if (team->items[i].index == 0x00) {
                    break;
                }
                fprintf(stdout, "Item: %s, Quantity: %d\n", (&items_names_list[team->items[i].index])->name, team->items[i].quantity);
            }
            break;

        case KEY_ITEM_CATEGORY:
            fprintf(stdout, "Key Items\n");
            for (int i = 0; i < sizeof(team->key_items); i++) {
                if (team->key_items[i].index == 0x00) {
                    break;
                }
                fprintf(stdout, "Item: %s, Quantity: %d\n", (&items_names_list[team->key_items[i].index])->name, team->key_items[i].quantity);
            }
            break;
        case BALL_CATEGORY:
            fprintf(stdout, "Balls\n");
            for (int i = 0; i < sizeof(team->balls); i++) {
                if (team->balls[i].index == 0x00) {
                    break;
                }
                fprintf(stdout, "Item: %s, Quantity: %d\n", (&items_names_list[team->balls[i].index])->name, team->balls[i].quantity);
            }
            break;
        case TM_CATEGORY:
            fprintf(stdout, "TMs\n");
            for (int i = 0; i < sizeof(team->tms); i++) {
                if (team->tms[i].index == 0x00) {
                    break;
                }
                fprintf(stdout, "Item: %s, Quantity: %d\n", (&items_names_list[team->tms[i].index])->name, team->tms[i].quantity);
            }
            break;
        case BERRY_CATEGORY:
            fprintf(stdout, "Berries\n");
            for (int i = 0; i < sizeof(team->berries); i++) {
                if (team->berries[i].index == 0x00) {
                    break;
                }
                fprintf(stdout, "Item: %s, Quantity: %d\n", (&items_names_list[team->berries[i].index])->name, team->berries[i].quantity);
            }
            break;

        default:
            fprintf(stdout, "Incorrect bag index number specified.\n");
    }
}

void rs_bag_new(char *item, int quantity) {
    struct rs_item* target = NULL;
    for (int i = 0; i < items_size; i++) {
        struct rs_item* list_item = &items_names_list[i];
        if (strcmp(list_item->name, item) == 0) {
            target = list_item;
            break;
        }
    }
    if (target == NULL) {
        fprintf(stderr, "Invalid item!\n");
        return;
    }
    struct file* fp;
    load_save_file(file_path, &fp);
    struct player_team* team = get_player_team(fp);

    struct item* list = NULL;
    int list_size = 0;
    if (target->bag_category == BAD_CATEGORY) {
        fprintf(stderr, "Invalid item with a bad category!\n");
        return;
    }
    else if (target->bag_category == ITEM_CATEGORY) {
        list = team->items;
        list_size = sizeof(team->items) / sizeof(struct item);
    }
    else if (target->bag_category == KEY_ITEM_CATEGORY) {
        list = team->key_items;
        list_size = sizeof(team->key_items) / sizeof(struct item);
    }
    else if (target->bag_category == BALL_CATEGORY) {
        list = team->balls;
        list_size = sizeof(team->balls) / sizeof(struct item);
    }
    else if (target->bag_category == TM_CATEGORY) {
        list = team->tms;
        list_size = sizeof(team->tms) / sizeof(struct item);
    }
    else if (target->bag_category == BERRY_CATEGORY) {
        list = team->berries;
        list_size = sizeof(team->berries) / sizeof(struct item);
    } else {
        fprintf(stderr, "Invalid category detected!\n");
        return;
    }
    for (int i = 0; i < list_size; i++) {
        struct item* item = &list[i];
        if (item->index == target->id) {
            item->quantity = quantity;
            return;
        }
        // Blank slot
        else if (item->index == 0x00) {
            item->index = target->id;
            item->quantity = quantity;
            return;
        }
    }
    fprintf(stderr, "No space left in bag. Please delete/overwrite an existing item\n");
}

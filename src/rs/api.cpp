#include "api.h"
#include "core.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>


extern std::string file_path;
extern std::string pokemon_name_list[];
extern const struct rs_item items_names_list[];

void RSAPI::box_view(int num) {
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
        std::string name = pokemon_name_list[info->species];
        const struct rs_item* item = &items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name.c_str(), item->name);
    }
    free(pc);
}
void RSAPI::party_view() {
    struct file* fp;
    load_save_file(file_path, &fp);
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
        std::string name = pokemon_name_list[info->species];
        const struct rs_item* item = &items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name.c_str(), item->name);
    }
}

void RSAPI::bag_view(std::string section) {
    enum category category = BAD_CATEGORY;
    if (section == "items") {
        category = ITEM_CATEGORY;
    }
    else if (section == "keyitems") {
        category = KEY_ITEM_CATEGORY;
    }
    else if (section == "balls") {
        category = BALL_CATEGORY;
    }
    else if (section == "tms") {
        category = TM_CATEGORY;
    }
    else if (section == "berries") {
        category = BERRY_CATEGORY;
    }
    else {
        fprintf(stderr, "Invalid section! Options are: 'items', 'keyitems', 'balls', 'tms', 'berries'\n");
        return;
    }

    struct rs_item* items;
    struct file* fp;
    load_save_file(file_path, &fp);
    int item_count = get_bag_items(fp, category, &items);

    for (int i = 0; i < item_count; i++) {
        printf("Item: %s, Quantity: %d\n", items[i].name, items[i].quantity);
    }

    free(items);
}

void RSAPI::bag_new(std::string item, int quantity) {
    struct rs_item* target = (struct rs_item*) malloc(sizeof(struct rs_item));
    for (int i = 0; i < 0; i++) {
        const struct rs_item* list_item = &items_names_list[i];
        if (list_item->name == item) {
            memcpy(target, list_item, sizeof(struct rs_item));
            break;
        }
    }
    if (target == NULL) {
        fprintf(stderr, "Invalid item!\n");
        return;
    }
    struct file* fp;
    load_save_file(file_path, &fp);
    target->quantity = quantity;
    set_bag_item(fp, target);
    save_file(fp);
    //fprintf(stderr, "No space left in bag. Please delete/overwrite an existing item\n");
}

#include "api.h"
#include <array>
#include "data.h"
#include "core.h"
#include "util.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>



extern std::string file_path;
namespace rs {
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
    void RSAPI::party_edit(int idx) {
        struct file* fp;
        load_save_file(file_path, &fp);
        struct player_team* team = get_player_team(fp);

        if (idx < 0 || idx > 5) {
            std::cerr << "BAD INDEX: Please give a value between 0 and 5!" << std::endl;
            return;
        }
        struct pokemon* pkmn = &(team->pokemon[idx]);
        __u16 id = pkmn->ot_id >> 16;
        __u16 sid = pkmn->ot_id & 0x00ff;
        struct poke_growth* info = get_poke_growth((struct pokemon*) pkmn);
        if (info->species == 0x00) {
            return;
        }
        // Read input
        std::cout << "Nickname: ";
        std::string new_nickname = "";
        std::getline(std::cin, new_nickname);
        if (new_nickname != "") { 
            char out_nickname[10];
            memset(out_nickname, 0xff, 10);
            encode_string((char*) new_nickname.c_str(), new_nickname.size(), out_nickname);
            memcpy(&pkmn->nickname, out_nickname, 10);
        }
        char nickname[sizeof(pkmn->nickname) + 1];
        nickname[sizeof(pkmn->nickname)] = 0;
        char ot[sizeof(pkmn->ot) + 1];
        ot[sizeof(pkmn->ot)] = 0;
        decode_string(pkmn->nickname, sizeof(pkmn->nickname), nickname);
        decode_string(pkmn->ot, sizeof(pkmn->ot), ot);
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", idx, nickname, ot, id, sid);
        std::string name = pokemon_name_list[info->species];
        const struct rs_item* item = &items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name.c_str(), item->name);
        save_file(fp);
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

    void RSAPI::bag_edit(std::string item, int quantity) {
        struct rs_item target;
        bool found = false;
        for (int i = 0; i < items_names_list.size(); i++) {
            const struct rs_item* list_item = &items_names_list[i];
            if (std::string(list_item->name) == item) {
                target = *list_item;
                found = true;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Invalid item!\n");
            return;
        }
        struct file* fp;
        load_save_file(file_path, &fp);
        target.quantity = quantity;
        set_bag_item(fp, &target);
        save_file(fp);
        //fprintf(stderr, "No space left in bag. Please delete/overwrite an existing item\n");
    }
}

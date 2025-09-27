#include "api.h"
#include "rs.h"
#include "util.h"
#include <stdio.h>
#include <string.h>


extern char* file_path;
extern char* pokemon_name_list[];
extern char* items_names_list[];

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
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", i, pkmn.nickname, pkmn.ot, id, sid);
        char* name = pokemon_name_list[info->species];
        char* item = items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name, item);
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
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", i, pkmn.nickname, pkmn.ot, id, sid);
        char* name = pokemon_name_list[info->species];
        char* item = items_names_list[info->held_item];
        fprintf(stderr, "Species: %s, Item: %s\n", name, item);
    }
}

#include "api.h"
#include "rs.h"
#include "util.h"
#include <stdio.h>


extern char* file_path;
extern char* pokemon_name_list[];

void rs_box_view(int num) {
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
        fprintf(stderr, "ID: %d: Name: %s, OT: %s, ID/SID: %05d/%05d | ", i, pkmn.nickname, pkmn.ot, id, sid);
        struct poke_growth* info = get_poke_growth((struct pokemon*) &pkmn);
        char* name = pokemon_name_list[info->species];
        fprintf(stderr, "Species: %s, Item: %x\n", name, info->held_item);
    }
}

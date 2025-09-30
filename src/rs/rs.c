#include "rs.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <endian.h>
#include <string.h>
#include "util.h"


void load_save_file(char* path, struct file** filep) {
    fprintf(stderr, "Opening file\n");
    FILE *f = fopen(path, "rb");
    fprintf(stderr, "Getting length of file\n");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
    fprintf(stderr, "Read %lu bytes, expecting %lu\n", fsize, sizeof(struct file));
    fprintf(stderr, "reading complete\n");

    char buffer[sizeof(struct file)];
    fread(buffer, sizeof(struct file), 1, f);
    fclose(f);
    fprintf(stderr, "reading complete\n");

    struct file* fp = (struct file*) buffer;

    // Save index
    int idx = fp->save_a[0].section_id;
    // Trainer stuff
    // Calculate offset

    int trainer_idx = __get_section_offset_step(0, idx);
    struct trainer_info* ti = (struct trainer_info*) (fp->save_a[trainer_idx].data);
    ti->trainer_id = htole32(ti->trainer_id);
    // Decode name string
    for (int i = 0; i < sizeof(ti->name); i++) {
        ti->name[i] = __character_map(ti->name[i]);
    }

    // Player team stuff
    // Calculate offset
    int team_idx = __get_section_offset_step(1, idx);
    struct player_team* team = (struct player_team*) (fp->save_a[team_idx].data + 0x0234);
    team->team_size = htole32(team->team_size);
    team->money = htole32(team->money);
    team->coins = htole16(team->coins);
    for (int i = 0; i < 40; i++) {
        __itemtole(&team->pc_items[i]);
    }
    for (int i = 0; i < 20; i++) {
        __itemtole(&team->items[i]);
    }
    for (int i = 0; i < 20; i++) {
        __itemtole(&team->key_items[i]);
    }
    for (int i = 0; i < 16; i++) {
        __itemtole(&team->balls[i]);
    }
    for (int i = 0; i < 64; i++) {
        __itemtole(&team->tms[i]);
    }
    for (int i = 0; i < 46; i++) {
        __itemtole(&team->berries[i]);
    }
    // Pokemon conversion
    for (int i = 0; i < 6; i++) {
        // Convert to LE
        __pokemontole(&team->pokemon[i]);
        __decrypt_poke_data(&team->pokemon[i]);
        __check_pokemon_chksum(&team->pokemon[i]);
    }

    // PC Box stuff
    // 9 sections, each 4096 bytes long
    char buf[4096 * 9];
    memset(buf, 0, sizeof(buf));
    size_t offset = 0;
    for (int i = 5; i <= 13; i++) {
        int box_idx = __get_section_offset_step(i, idx);
        memcpy(buf + offset, &fp->save_a[box_idx], 3968);
        offset += 3968; //TODO: Need to remove hardcode, all sections except
                        //last is 3968, last is 2000. We cannot use 4096 bytes
                        //because padding towards end messes with pkmn data
    }
    struct pc_buffer* pc = (struct pc_buffer*) buf;
    pc->box_idx = htole32(pc->box_idx);
    for (int i = 0; i < sizeof(pc->pokemon) / sizeof(struct pc_pokemon); i++) {
        // fprintf(stderr, "Processing Pokemon %d\n", i);
        char tmp[sizeof(struct pokemon)];
        memset(tmp, 0, sizeof(struct pokemon));
        memcpy(tmp, &pc->pokemon[i], sizeof(struct pc_pokemon));
        __pokemontole((struct pokemon*)tmp);
        memcpy(&pc->pokemon[i], tmp, sizeof(struct pc_pokemon));
        __decrypt_poke_data((struct pokemon*)&pc->pokemon[i]);
        __check_pokemon_chksum((struct pokemon*) &pc->pokemon[i]);
    }
    // Write back after decrypt/conversion
    offset = 0;
    for (int i = 5; i <= 13; i++) {
        int box_idx = __get_section_offset_step(i, idx);
        memcpy(&fp->save_a[box_idx],buf + offset, 3968);
        offset += 3968; //TODO: Need to remove hardcode, all sections except
                        //last is 3968, last is 2000. We cannot use 4096 bytes
                        //because padding towards end messes with pkmn data
    }


    // After conversion, point filep to fp
    *filep = fp;
}


// Get the trainer info of a save file in the form of a trainer_info struct
struct trainer_info* get_trainer_info(struct file* fp) {
    // Calculate offset
    int idx = fp->save_a[0].section_id;
    int trainer_idx = __get_section_offset_step(0, idx);
    struct trainer_info* ti = (struct trainer_info*) (fp->save_a[trainer_idx].data);
    return ti;
}

// Get the trainer info of a save file in the form of a trainer_info struct
struct player_team* get_player_team(struct file* fp) {
    // Calculate offset
    int idx = fp->save_a[0].section_id;
    int trainer_idx = __get_section_offset_step(1, idx);
    struct player_team* team = (struct player_team*) (fp->save_a[trainer_idx].data + 0x0234);
    return team;
}

struct pc_buffer* get_pc(struct file* fp) {
    // Calculate offset
    int idx = fp->save_a[0].section_id;
    // 9 sections, each 4096 bytes long
    char buf[4096 * 9];
    memset(buf, 0, sizeof(buf));
    size_t offset = 0;
    for (int i = 5; i <= 13; i++) {
        int box_idx = __get_section_offset_step(i, idx);
        memcpy(buf + offset, &fp->save_a[box_idx], 3968);
        offset += 3968; //TODO: Need to remove hardcode, all sections except
                        //last is 3968, last is 2000. We cannot use 4096 bytes
                        //because padding towards end messes with pkmn data
    }
    struct pc_buffer* pc = (struct pc_buffer*) buf;
    return pc;
}


int save_pc_buffer(struct pc_buffer* pc, struct file *fp) {
    // PC Box stuff
    for (int i = 0; i < sizeof(pc->pokemon) / sizeof(struct pc_pokemon); i++) {
        char tmp[sizeof(struct pokemon)];
        memset(tmp, 0, sizeof(struct pokemon));
        memcpy(tmp, &pc->pokemon[i], sizeof(struct pc_pokemon));
        __pokemontole((struct pokemon*)tmp);
        memcpy(&pc->pokemon[i], tmp, sizeof(struct pc_pokemon));
        __decrypt_poke_data((struct pokemon*)&pc->pokemon[i]);
        __check_pokemon_chksum((struct pokemon*) &pc->pokemon[i]);
    }
    return 0;
}

int save_file(struct file *fp) {
    return 0;
}

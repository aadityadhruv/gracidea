#include "rs.h"
#include <assert.h>
#include <stdio.h>
#include <endian.h>
#include "util.h"


void load_save_file(char* path, struct file** fp) {
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

    *fp = (struct file*) buffer;
}


// Get the trainer info of a save file in the form of a trainer_info struct
struct trainer_info* get_trainer_info(struct file* fp) {
    // Calculate offset
    int idx = fp->save_a[0].section_id;
    int trainer_idx = __get_section_offset_step(0, idx);
    struct trainer_info* ti = (struct trainer_info*) (fp->save_a[trainer_idx].data);
    ti->trainer_id = htole32(ti->trainer_id);
    // Decode name string
    for (int i = 0; i < sizeof(ti->name); i++) {
        ti->name[i] = __character_map(ti->name[i]);
    }
    return ti;
}

// Get the trainer info of a save file in the form of a trainer_info struct
struct player_team* get_player_team(struct file* fp) {
    // Calculate offset
    int idx = fp->save_a[0].section_id;
    int trainer_idx = __get_section_offset_step(1, idx);
    struct player_team* team = (struct player_team*) (fp->save_a[trainer_idx].data + 0x0234);
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
    return team;
}


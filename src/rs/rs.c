#include "rs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include <string.h>


void __itemtole(struct item* item);
void __pokemontole(struct pokemon* pokemon);
char __character_map(__u8 c);
int __get_section_offset_step(int target, int cur);

void __itemtole(struct item* item) {
    item->index = htole16(item->index);
    item->quantity = htole16(item->quantity);
}

void __pokemontole(struct pokemon* pokemon) {
    pokemon->personality = htole32(pokemon->personality);
    pokemon->ot_id = htole32(pokemon->ot_id);
    pokemon->checksum = htole16(pokemon->checksum);
    pokemon->status = htole32(pokemon->status);
    pokemon->hp = htole16(pokemon->hp);
    pokemon->total_hp = htole16(pokemon->total_hp);
    pokemon->atk = htole16(pokemon->atk);
    pokemon->def = htole16(pokemon->def);
    pokemon->spd = htole16(pokemon->spd);
    pokemon->sp_atk = htole16(pokemon->sp_atk);
    pokemon->sp_def = htole16(pokemon->sp_def);

    for (int i = 0; i < sizeof(pokemon->nickname); i++) {
        pokemon->nickname[i] = __character_map(pokemon->nickname[i]);
    }
    for (int i = 0; i < sizeof(pokemon->ot); i++) {
        pokemon->ot[i] = __character_map(pokemon->ot[i]);
    }
}
// Convert Gen III RS character encoding (Western boards)
char __character_map(__u8 c) {
    char map[256];
    memset(map, 0, 256);
    // Number 0-9 conversion
    for (int i = 0; i < 10; i++) {
        map[0xA1 + i] = ('0' + i);
    }
    // Alphabet conversion
    for (int i = 0; i < 26; i++) {
        // Upper case indices
        map[0xBB + i] = ('A' + i);
        // Lower case indices
        map[0xD5 + i] = ('a' + i);
    }
    // Symbols
    map[0xAB] = '!';
    map[0xAC] = '?';
    map[0xAD] = '.';
    map[0xAE] = '-';
    map[0xB1] = '"';
    map[0xB2] = '"'; //TODO: Region difference
    map[0xB3] = '`';
    map[0xB4] = '\'';
    // NULL TERMINATION OF STRINGS
    map[0xff] = 0;
    return map[c];
}

// Get position in save file for a target section
int __get_section_offset_step(int target, int cur) {
    // [5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 1, 2, 3, 4]
    // ((target - current) + 14) % 14
    return ((target - cur) + NUMBER_OF_SECTIONS) % NUMBER_OF_SECTIONS;
}


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
    for (int i = 0; i < 6; i++) {
        __pokemontole(&team->pokemon[i]);
    }
    return team;
}


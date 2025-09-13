#include "util.h"
#include <string.h>

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


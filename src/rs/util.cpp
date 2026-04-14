#include "util.h"
#include <assert.h>
#include <endian.h>
#include <string.h>

namespace rs {
    void encode_string(char* input, int size, char* output) {
        for (int i = 0; i < size; i++) {
            output[i] = __character_map_encode(input[i]);
        }
    }
    void decode_string(char* input, int size, char* output) {
        for (int i = 0; i < size; i++) {
            output[i] = __character_map_decode(input[i]);
        }
    }

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

    }
    char __character_map_encode(__u8 c) {
        char map[256];
        memset(map, 0, 256);
        // Number 0-9 conversion
        for (int i = 0; i < 10; i++) {
            map['0' + i] = (0xA1 + i);
        }
        // Alphabet conversion
        for (int i = 0; i < 26; i++) {
            // Upper case indices
            map['A' + i] = (0xBB + i);
            // Lower case indices
            map['a'+ i] = (0xD5 + i);
        }
        // Symbols
        map['!'] = 0xAB;
        map['?'] = 0xAC;
        map['.'] = 0xAD;
        map['-'] = 0xAE;
        map['"'] = 0xB1;
        map['"'] = 0xB2; //TODO: Region difference
        map['`'] = 0xB3;
        map['\''] = 0xB4;
        // NULL TERMINATION OF STRINGS
        map[0] = 0xff;
        return map[c];
    }
    // Convert Gen III RS character encoding (Western boards)
    char __character_map_decode(__u8 c) {
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

    void __poke_growthtole(struct poke_growth* poke_growth) {
        poke_growth->species = htole16(poke_growth->species);
        poke_growth->held_item = htole16(poke_growth->held_item);
        poke_growth->exp = htole32(poke_growth->exp);
    }
    void __poke_attacktole(struct poke_attack* poke_attack) {
        for (int i = 0; i < sizeof(poke_attack->moves); i++) {
            poke_attack->moves[i] = htole16(poke_attack->moves[i]);
        }
    }
    void __poke_misctole(struct poke_misc* poke_misc) {
        poke_misc->origin = htole16(poke_misc->origin);
        poke_misc->iv_egg_ability = htole32(poke_misc->iv_egg_ability);
        poke_misc->ribbons = htole32(poke_misc->ribbons);
    }

    /**
     * @brief Validate a Pokemon's data
     *
     * @param pokemon: The pokemon to verify
     * @return 0 on valid Pokemon, non-zero value if "Bad egg"
     */
    __u8 __check_pokemon_chksum(struct pokemon* pokemon) {
        __u16 chk = 0;
        __u16* data = (__u16*)&pokemon->data;
        for (int i = 0; i < 3*4*2; i++) {
            chk += htole16(data[i]);
        }
        return !(chk == pokemon->checksum);
    }
    void __gen_pokemon_chksum(struct pokemon* pokemon) {
        __u16 chk = 0;
        __u16* data = (__u16*)&pokemon->data;
        for (int i = 0; i < 3*4*2; i++) {
            chk += htole16(data[i]);
        }
        pokemon->checksum = chk;
    }

    void __decrypt_poke_data(struct pokemon* pokemon) {
        __u32 key = pokemon->ot_id ^ pokemon->personality;
        __u32* data = (__u32*)&pokemon->data;
        for (int i = 0; i < 3*4; i++) {
            data[i] ^= key;
        }
    }
    void __encrypt_poke_data(struct pokemon* pokemon) {
        __u32 key = pokemon->ot_id ^ pokemon->personality;
        __u32* data = (__u32*)&pokemon->data;
        for (int i = 0; i < 3*4; i++) {
            data[i] ^= key;
        }
    }
    struct poke_growth* get_poke_growth(struct pokemon* pokemon) {
        __u8 map[24] = {
            0, 0, 0, 0, 0, 0,
            1, 1, 2, 3, 2, 3,
            1, 1, 2, 3, 2, 3,
            1, 1, 2, 3, 2, 3
        };
        __u8 idx = map[pokemon->personality % 24];
        struct poke_growth* pg = (struct poke_growth*) &pokemon->data[idx];
        __poke_growthtole(pg);
        return pg;
    }
    struct poke_attack* get_poke_attack(struct pokemon* pokemon) {
        __u8 map[24] = {
            1, 1, 2, 3, 2, 3,
            0, 0, 0, 0, 0, 0,
            2, 3, 1, 1, 3, 2,
            2, 3, 1, 1, 3, 2,
        };
        __u8 idx = map[pokemon->personality % 24];
        struct poke_attack* patk = (struct poke_attack*) &pokemon->data[idx];
        __poke_attacktole(patk);
        return patk;
    }
    struct poke_ev* get_poke_ev(struct pokemon* pokemon) {
        __u8 map[24] = {
            2, 3, 1, 1, 3, 2, 
            2, 3, 1, 1, 3, 2, 
            0, 0, 0, 0, 0, 0, 
            3, 3, 3, 2, 1, 1, 
        };
        __u8 idx = map[pokemon->personality % 24];
        struct poke_ev* pev = (struct poke_ev*) &pokemon->data[idx];
        return pev;
    }
    struct poke_misc* get_poke_misc(struct pokemon* pokemon) {
        __u8 map[24] = {
            3, 2, 3, 2, 1, 1, 
            3, 2, 3, 2, 1, 1, 
            3, 2, 3, 2, 1, 1, 
            0, 0, 0, 0, 0, 0,
        };
        __u8 idx = map[pokemon->personality % 24];
        struct poke_misc* pmisc = (struct poke_misc*) &pokemon->data[idx];
        __poke_misctole(pmisc);
        return pmisc;
    }
}

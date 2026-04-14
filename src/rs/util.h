#pragma once
#include <endian.h>
#include "core.h"

namespace rs {
#define NUMBER_OF_SECTIONS 14

    // Internal functions to manipulate/acess/decode struct file structure
    void __itemtole(struct item* item);
    void __pokemontole(struct pokemon* pokemon);
    void decode_string(char* input, int size, char* output);
    void __poke_growthtole(struct poke_growth* poke_growth);
    void __poke_attacktole(struct poke_attack* poke_attack);
    void __poke_misctole(struct poke_misc* poke_misc);
    void __decrypt_poke_data(struct pokemon* pokemon);
    void __encrypt_poke_data(struct pokemon* pokemon);
    void __gen_pokemon_chksum(struct pokemon* pokemon);
    int __get_section_offset_step(int target, int cur);
    char __character_map(__u8 c);
    __u8 __check_pokemon_chksum(struct pokemon* pokemon);
    struct poke_growth* get_poke_growth(struct pokemon* pokemon);
    struct poke_attack* get_poke_attack(struct pokemon* pokemon);
    struct poke_ev* get_poke_ev(struct pokemon* pokemon);
    struct poke_misc* get_poke_misc(struct pokemon* pokemon);
}

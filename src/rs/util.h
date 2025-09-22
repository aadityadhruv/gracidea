#pragma once
#include <endian.h>
#include "rs.h"

void __itemtole(struct item* item);
void __pokemontole(struct pokemon* pokemon);
void __poke_growthtole(struct poke_growth* poke_growth);
void __poke_attacktole(struct poke_attack* poke_attack);
void __poke_misctole(struct poke_misc* poke_misc);
void __decrypt_poke_data(struct pokemon* pokemon);
__u8 __check_pokemon_chksum(struct pokemon* pokemon);
struct poke_growth* get_poke_growth(struct pokemon* pokemon);
struct poke_attack* get_poke_attack(struct pokemon* pokemon);
struct poke_ev* get_poke_ev(struct pokemon* pokemon);
struct poke_misc* get_poke_misc(struct pokemon* pokemon);
char __character_map(__u8 c);
int __get_section_offset_step(int target, int cur);

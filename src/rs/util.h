#pragma once
#include <endian.h>
#include "rs.h"

void __itemtole(struct item* item);
void __pokemontole(struct pokemon* pokemon);
char __character_map(__u8 c);
int __get_section_offset_step(int target, int cur);

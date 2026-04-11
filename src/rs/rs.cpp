#include <string>
#include "rs.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include "cstring"
#include "util.h"

extern char* pokemon_name_list[];
extern struct rs_item items_names_list[];

/*
 * Minimum steps to get a save file loaded into memory. We convert required
 * values into LE notation. Data is NOT decoded or decrypted. That task is left
 * to other methods fetching information from specific sections
 */
void load_save_file(std::string path, struct file** filep) {
    fprintf(stderr, "Opening file\n");
    FILE *f = fopen(path.c_str(), "rb");
    fprintf(stderr, "Getting length of file\n");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); 
    fprintf(stderr, "Read %lu bytes, expecting %lu\n", fsize, sizeof(struct file));
    fprintf(stderr, "reading complete\n");

    //TODO Malloc
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
        // Decrypt mon temporarily to check the checksum, and then once checked,
        // encrypt again
        __decrypt_poke_data(&team->pokemon[i]);
        int chksm = __check_pokemon_chksum(&team->pokemon[i]);
        if (chksm) fprintf(stderr,"WARNING: Bad egg detected\n");
        __encrypt_poke_data(&team->pokemon[i]);
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
        int chksm = __check_pokemon_chksum((struct pokemon*) &pc->pokemon[i]);
        if (chksm) fprintf(stderr,"WARNING: Bad egg detected\n");
        __encrypt_poke_data((struct pokemon*)&pc->pokemon[i]);
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
    char* buf = (char*)malloc(4096 * 9);
    memset(buf, 0, 4096 * 9);
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


int save_pc(struct pc_buffer* pc, struct file *fp) {
    // PC Box stuff
    struct pc_buffer* pctmp = (struct pc_buffer*) malloc(sizeof(struct pc_buffer));
    if (pctmp == NULL) {
        perror("malloc");
        return -1;
    }
    memcpy(pctmp, pc, sizeof(struct pc_buffer));
    for (int i = 0; i < sizeof(pc->pokemon) / sizeof(struct pc_pokemon); i++) {
        char tmp[sizeof(struct pokemon)];
        memset(tmp, 0, sizeof(struct pokemon));
        memcpy(tmp, &pctmp->pokemon[i], sizeof(struct pc_pokemon));
        __pokemontole((struct pokemon*)tmp);
        memcpy(&pctmp->pokemon[i], tmp, sizeof(struct pc_pokemon));
        __encrypt_poke_data((struct pokemon*)&pctmp->pokemon[i]);
    }

    int idx = fp->save_a[0].section_id;
    size_t offset = 0;
    char* c_pctmp = (char*) pctmp;
    for (int i = 5; i <= 13; i++) {
        int box_idx = __get_section_offset_step(i, idx);
        memset(&fp->save_a[box_idx], 0, sizeof(fp->save_a[box_idx]));
        memcpy(&fp->save_a[box_idx], c_pctmp + offset, 3968);
        offset += 3968; //TODO: Need to remove hardcode, all sections except
                        //last is 3968, last is 2000. We cannot use 4096 bytes
                        //because padding towards end messes with pkmn data
    }

    free(pctmp);
    return 0;
}

int __save_team(struct player_team* t, struct file *fp) {
    struct player_team* team = (struct player_team*) malloc(sizeof(struct player_team));
    memset(team, 0, sizeof(struct player_team));
    memcpy(team, t, sizeof(struct player_team));
    int idx = fp->save_a[0].section_id;
    int team_idx = __get_section_offset_step(1, idx);
    // TODO: Recalc checksum
//    struct player_team* team = (struct player_team*) (fp->save_a[team_idx].data + 0x0234);
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
        __gen_pokemon_chksum(&team->pokemon[i]);
        __check_pokemon_chksum(&team->pokemon[i]);
        __encrypt_poke_data(&team->pokemon[i]);
    }
    memcpy(fp->save_a[team_idx].data + 0x0234, (char*)team, sizeof(struct player_team));
    free(team);
    return 0;
}

int save_trainer_info(struct trainer_info *pc, struct file *fp) {
    return 0;
}

/*
 * Encode, Encrypt and write out a fp.
 * A fp, which is typically loaded into memory with a load_save_file call,
 * EXTRACTS all the data in a save file in a decoded and decrypted format.
 * We apply the the reverse here and save it
 */
int save_file(struct file *fp) {
    fprintf(stderr, "Opening tmp file\n");
    FILE *f = fopen("/tmp/rs.sav", "w");
    char* save_data = (char*) fp;
    fwrite(save_data, sizeof(struct file), 1, f);
    return 0;
}

int get_bag_items(struct file* fp, enum category category, struct rs_item** items) {
    int count = 0;
    struct item* list = NULL;
    int list_total = 0;
    int rs_item_size = sizeof(struct rs_item);
    struct player_team* team = get_player_team(fp);

    switch (category) {
        case ITEM_CATEGORY:
            list = team->items;
            list_total = sizeof(team->items);
            break;
        case KEY_ITEM_CATEGORY:
            list = team->key_items;
            list_total = sizeof(team->key_items);
            break;
        case BALL_CATEGORY:
            list = team->balls;
            list_total = sizeof(team->balls);
            break;
        case TM_CATEGORY:
            list = team->tms;
            list_total = sizeof(team->tms);
            break;
        case BERRY_CATEGORY:
            list = team->berries;
            list_total = sizeof(team->berries);
            break;

        default:
            fprintf(stdout, "Incorrect bag index number specified.\n");
    }
    for (int i = 0; i < list_total; i++) {
        if (list[i].index == 0x00) {
            break;
        }
        count += 1;
    }
    *items = (struct rs_item*) malloc(count * rs_item_size);
    for (int i = 0; i < count; i++) {
        struct item raw = list[i];
        struct rs_item item = items_names_list[raw.index];
        item.quantity = raw.quantity;
        memcpy((*items) + i, &item, rs_item_size);
    }
    return count;
}

int set_bag_item(struct file* fp, struct rs_item *target) {
    // This is a reference, not a copy
    struct player_team* team = get_player_team(fp);

    struct item* list = NULL;
    int list_size = 0;
    if (target->bag_category == BAD_CATEGORY) {
        return -1;
    }
    else if (target->bag_category == ITEM_CATEGORY) {
        list = team->items;
        list_size = sizeof(team->items) / sizeof(struct item);
    }
    else if (target->bag_category == KEY_ITEM_CATEGORY) {
        list = team->key_items;
        list_size = sizeof(team->key_items) / sizeof(struct item);
    }
    else if (target->bag_category == BALL_CATEGORY) {
        list = team->balls;
        list_size = sizeof(team->balls) / sizeof(struct item);
    }
    else if (target->bag_category == TM_CATEGORY) {
        list = team->tms;
        list_size = sizeof(team->tms) / sizeof(struct item);
    }
    else if (target->bag_category == BERRY_CATEGORY) {
        list = team->berries;
        list_size = sizeof(team->berries) / sizeof(struct item);
    } else {
        return -1;
    }
    for (int i = 0; i < list_size; i++) {
        struct item* item = &list[i];
        if (item->index == target->id) {
            item->quantity = htole16(target->quantity);
            return 0;
        }
        // Blank slot
        else if (item->index == 0x00) {
            item->index = htole16(target->id);
            item->quantity = htole16(target->quantity);
            return 0;
        }
    }
    // No space left
    return -1;
}

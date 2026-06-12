#include <iostream>
#include <string>
#include "core.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include "cstring"
#include "util.h"
#include <format>
#include "data.h"

namespace rs {
    struct file_raw load_save_file(const std::string &path) {
        std::cout << "Opening file" << std::endl;
        FILE *f = fopen(path.c_str(), "rb");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 
        std::cout << std::format("Read {} bytes, expecting {}", fsize, sizeof(struct file_raw)) << std::endl;

        char buffer[sizeof(struct file_raw)];
        fread(buffer, sizeof(struct file_raw), 1, f);
        fclose(f);

        const auto fp = reinterpret_cast<file_raw*>(buffer);

        // Save index
        const int idx = fp->save_a[0].section_id;
        // Trainer stuff
        // Calculate offset
        //TODO: Maybe refactor below func to take struct savefile and get idx from
        //there instead of defining it above and passing it here
        const int trainer_idx = __get_section_offset_step(0, idx);
        const auto ti = reinterpret_cast<struct trainer_info_raw *>(fp->save_a[trainer_idx].data);
        ti->trainer_id = htole32(ti->trainer_id);

        // Player team stuff
        // Calculate offset
        const int team_idx = __get_section_offset_step(1, idx);
        const auto team = reinterpret_cast<struct player_team_raw *>(fp->save_a[team_idx].data + 0x0234);
        team->team_size = htole32(team->team_size);
        team->money = htole32(team->money);
        team->coins = htole16(team->coins);
        for (auto & item : team->pc_items) {
            __itemtole(&item);
        }
        for (auto & item : team->items) {
            __itemtole(&item);
        }
        for (auto & key_item : team->key_items) {
            __itemtole(&key_item);
        }
        for (auto & ball : team->balls) {
            __itemtole(&ball);
        }
        for (auto & tm : team->tms) {
            __itemtole(&tm);
        }
        for (auto & berry: team->berries) {
            __itemtole(&berry);
        }
        // Pokemon conversion
        for (auto & i : team->pokemon) {
            // Convert to LE
            __pokemontole(&i);
            // Decrypt mon to check the checksum
            __decrypt_poke_data(&i);
            if (__check_pokemon_chksum(&i)) fprintf(stderr,"WARNING: Bad egg detected\n");
        }

        // PC Box stuff
        // 9 sections, each 4096 bytes long
        char buf[4096 * 9] = {};
        size_t offset = 0;
        for (int i = 5; i <= 13; i++) {
            const int box_idx = __get_section_offset_step(i, idx);
            memcpy(buf + offset, &fp->save_a[box_idx], 3968);
            offset += 3968; //TODO: Need to remove hardcode, all sections except
                            //last is 3968, last is 2000. We cannot use 4096 bytes
                            //because padding towards end messes with pkmn data
        }
        const auto pc = reinterpret_cast<struct pc_buffer_raw *>(buf);
        pc->box_idx = htole32(pc->box_idx);
        for (auto & i : pc->pokemon) {
            char tmp[sizeof(struct pokemon_raw)] = {};
            memcpy(tmp, &i, sizeof(struct pc_pokemon));
            __pokemontole(reinterpret_cast<struct pokemon_raw *>(tmp));
            memcpy(&i, tmp, sizeof(struct pc_pokemon));
            __decrypt_poke_data(reinterpret_cast<struct pokemon_raw *>(&i));
            if (__check_pokemon_chksum(reinterpret_cast<struct pokemon_raw *>(&i))) fprintf(stderr,"WARNING: Bad egg detected\n");
        }
        return *fp;
    }


    // Get the trainer info of a save file in the form of a trainer_info_raw struct
    struct trainer_info_raw* get_trainer_info(struct file_raw* fp) {
        // Calculate offset
        const int idx = fp->save_a[0].section_id;
        const int trainer_idx = __get_section_offset_step(0, idx);
        const auto ti = reinterpret_cast<struct trainer_info_raw *>(fp->save_a[trainer_idx].data);
        return ti;
    }

    // Get the trainer info of a save file in the form of a trainer_info_raw struct
    struct player_team_raw* get_player_team(struct file_raw* fp) {
        // Calculate offset
        const int idx = fp->save_a[0].section_id;
        const int trainer_idx = __get_section_offset_step(1, idx);
        const auto team = reinterpret_cast<struct player_team_raw *>(fp->save_a[trainer_idx].data + 0x0234);
        return team;
    }

    struct pc_buffer_raw* get_pc(struct file_raw* fp) {
        // Calculate offset
        int idx = fp->save_a[0].section_id;
        // 9 sections, each 4096 bytes long
        auto buf = static_cast<char *>(malloc(9 * 4096));
        memset(buf, 0, 4096 * 9);
        size_t offset = 0;
        for (int i = 5; i <= 13; i++) {
            int box_idx = __get_section_offset_step(i, idx);
            memcpy(buf + offset, &fp->save_a[box_idx], 3968);
            offset += 3968; //TODO: Need to remove hardcode, all sections except
                            //last is 3968, last is 2000. We cannot use 4096 bytes
                            //because padding towards end messes with pkmn data
        }
        auto pc = reinterpret_cast<struct pc_buffer_raw *>(buf);
        return pc;
    }

    int save_file(const struct file_raw &save) {
        file_raw out{};
        // ========= Save Player Team ==========
        const player_team_raw team = save.team;
        // Pokemon conversion
        for (const auto & i : team.pokemon) {
            // Convert to LE
            if (__check_pokemon_chksum(&i)) fprintf(stderr, "ERROR: Bad egg in saving!\n");
            __encrypt_poke_data(&i);
        }
        // Trainer Info needs no modifications, it's already directly save-able

        // ========= Save PC ==========
        // Save index
        int idx = fpc.save_a[0].section_id;
        struct pc_buffer_raw* pc = get_pc(&fpc);
        for (int i = 0; i < sizeof(pc->pokemon) / sizeof(struct pc_pokemon); i++) {
            pokemon_raw tmp_pokemon{};
            memset(&tmp_pokemon, 0, sizeof(struct pokemon_raw));
            memcpy(&tmp_pokemon, &pc->pokemon[i], sizeof(struct pc_pokemon));
            if (__check_pokemon_chksum(&tmp_pokemon)) fprintf(stderr, "ERROR: Bad egg in saving!\n");
            __encrypt_poke_data(&tmp_pokemon);
            memcpy(&pc->pokemon[i], &tmp_pokemon, sizeof(struct pc_pokemon));
        }
        // Write back after decrypt/conversion
        size_t offset = 0;
        const auto buf = reinterpret_cast<char *>(pc);
        for (int i = 5; i <= 13; i++) {
            int box_idx = __get_section_offset_step(i, idx);
            int size = (i == 13) ? 2000 : 3968;
            memcpy(&fpc.save_a[box_idx],buf + offset, size);
            offset += 3968; //TODO: Need to remove hardcode, all sections except
                            //last is 3968, last is 2000. We cannot use 4096 bytes
                            //because padding towards end messes with pkmn data
        }
        fprintf(stderr, "Opening tmp file\n");
        FILE *f = fopen("/tmp/rs.sav", "w");
        const auto save_data = reinterpret_cast<char *>(&fpc);
        fwrite(save_data, sizeof(struct file_raw), 1, f);
        return 0;
    }

    int get_bag_items(struct file_raw* fp, enum category category, struct rs_item** items) {
        int count = 0;
        struct item* list = NULL;
        int list_total = 0;
        int rs_item_size = sizeof(struct rs_item);
        struct player_team_raw* team = get_player_team(fp);

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

    int set_bag_item(struct file_raw* fp, struct rs_item *target) {
        // This is a reference, not a copy
        struct player_team_raw* team = get_player_team(fp);

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
}

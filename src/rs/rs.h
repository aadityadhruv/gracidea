#include <linux/types.h>
/* Gen III Save Data Structure
 * This includes Pokemon Ruby and Pokemon Sapphire
 * Versions.
 *
 *
 * File Structure
 * 4 KB section
 * 0x000000 Save A
 * 0x00E000 Save B
 * 0x01C000 Hall of Fame
 * 0x01E000 Mystery Gift
 * 0x01F000 Recorded Battle
 *
 */

/**
 * struct section_t - A save file has 14 sections, each containing data about a
 * part of a save file, such as the trainer information, teams, PC buffers, etc.
 * The particular data the section represents is determined by the section_id.
 * Each section in a save file will have the same same_index
 */
struct section_t {
    // The data for a particular section, such as Trainer Info
    char data[3968];
    // Padding for 4KB
    char padding[116];
    // Section ID corresponding to above data. Ranges from 0 to 13
    __u16 section_id;
    // Checksum for above data
    __u16 checksum;
    // Hardcoded value of 0x08012025 in little-endian
    __u32 signature;
    // Monotonically increasing index that is same for all sections in a save
    // index
    __u32 save_index;
};


// Pokemon Structure
struct pokemon {
    __u32 personality;
    __u32 ot_id;
    char nickname[10];
    __u8 lang;
    __u8 flags;
    char ot[7];
    __u8 markings;
    __u16 checksum;
    __u16 blank;
    __u8 data[48];
    __u32 status;
    __u8 level;
    __u8 mail_id;
    __u16 hp;
    __u16 total_hp;
    __u16 atk;
    __u16 def;
    __u16 spd;
    __u16 sp_atk;
    __u16 sp_def;
    char padding[2];
};


// SECTION 0
struct trainer_info {
    // Namme of the player
    char name[7];
    // 0x00 is boy 0x01 is girl
    char gender[1];
    // Unused byte
    char blank[1];
    // Upper 16 bits are public ID, lower 16 bits are secret ID
    __u32 trainer_id[4];
    // Hours, minutes, seconds and frames (1/60th of sec). Hours is 16 bit
    char time_played[5];
    // Menu Options
    char options[3];
    // FRLG -> 0x00000001, RS -> Battle Tower name, Emerald -> Security Key
    // location
    char game_code[4];
    // Key to encrypt money and item quantities
    char security_key[4];
};



// SECTION 1
struct item {
    __u16 index;
    __u16 quantity;
};
struct player_team {
    __u32 team_size;
    struct pokemon pokemon[6];
    // XOR with security key to get real value
    __u32 money;
    // XOR with lower 2 bytes of security key to get real value
    __u16 coins;
    struct item pc_items[40];
    struct item items[20];
    struct item key_items[20];
    struct item balls[16];
    struct item tms[64];
    struct item berries[46];
};


// SECTION 2

struct game_state {
    __u32 mirage_island;
};

// SECTION 3-4

struct game_data {
    __u16 blank;
};

// SECTION 5 - 13

struct pc_buffer {
    // zero indexed
    __u32 box_idx;
    // Left to right, top to bottom order
    char pokemon[33600];
    // 9 bytes each, each name can be from 1 to 8 chars
    char box_names[126];
    // 1 byte for each wallpaper
    char wallpaper[14];

};


struct hof_pokemon {
    __u32 trainer_id;
    __u32 personality;
    __u16 pokemon;
    char name[10];
};
struct hall_of_fame {
    // 50 teams of 6 pokemon can be stored
    struct hof_pokemon teams[300];
};
struct mystery_gift {};
struct recorded_battle {};

struct file {
    struct section_t save_a[14];
    struct section_t save_b[14];
    struct hall_of_fame hof;
    struct mystery_gift gifts;
    struct recorded_battle battle;
};

void load_save_file(char* path, struct file** fp);

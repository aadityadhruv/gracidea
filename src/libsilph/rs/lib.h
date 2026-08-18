#pragma once
#include <array>
#include <vector>
#include "core.h"

namespace rs {
    class Pokemon {
    public:
        int32_t personality;
        int32_t ot_id;
        std::string nickname;
        int8_t lang;
        bool is_bad_egg;
        bool is_egg;
        bool is_block_box;
        std::string ot;
        int8_t markings;
        int16_t species;
        int16_t held_item;
        int32_t exp;
        int8_t pp_bonus;
        int8_t friendship;
        int8_t hp;
        int8_t atk;
        int8_t def;
        int8_t spd;
        int8_t spatk;
        int8_t spdef;
        int8_t cool;
        int8_t beaut;
        int8_t cute;
        int8_t smart;
        int8_t tough;
        int8_t feel;
    private:
        int16_t moves[4];
        int8_t pp[4];
        int16_t checksum;
        int8_t pokerus;
        int8_t met;
        int16_t origin;
        int32_t iv_egg_ability;
        int32_t ribbons;

    };
    class TrainerInfo {
    public:
        std::string name;
        bool gender;
        int32_t trainer_id;
        int16_t hours;
        int8_t mins;
        int8_t secs;
        int8_t frames;
        std::string battle_tower_name;
    private:
        char security_key[4];
    };
    class BagItem {
    public:
        int16_t index;
        int16_t quantity;
    };
    class PlayerBag {
    public:
        int32_t money;
        int16_t coins;
        std::array<BagItem, 50> pc_items;
        std::array<BagItem, 20> items;
        std::array<BagItem, 20> key_items;
        std::array<BagItem, 16> balls;
        std::array<BagItem, 64> tms;
        std::array<BagItem, 46> berries;

    };
    class PlayerTeam {
        std::vector<Pokemon> pokemon;
    };

    class PCBox {
    public:
        std::array<Pokemon, 30> pokemon;
        std::string box_name;
        int8_t wallpaper;
    };
    class PC {
    public:
        int32_t box_index;
        std::array<PCBox, 14> boxes;
    };

    class Savefile {
    public:
        TrainerInfo trainer_info;
        PlayerBag player_bag;
        PlayerTeam player_team;
        PC pc;
        explicit Savefile(const std::string& path);
        void save(std::string path = "/tmp/rs.sav");
    private:
        file_raw raw_source;
    };
}

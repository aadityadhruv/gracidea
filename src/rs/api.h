#pragma once
#include "../api.h"

struct RSAPI : PokeAPI { 
    void box_view(int num) override;
    // void box_edit(int num, int idx) override;
    // void box_new(int num, int idx, const struct G_Pokemon &pokemon) override;

    void party_view() override;
    // void party_edit(int idx) override;
    // void party_new(int idx, const struct G_Pokemon &pokemon) override;

    void bag_view(std::string section) override;
    void bag_new(std::string item, int quantity) override;
};

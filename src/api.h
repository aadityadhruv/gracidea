#pragma once
#include <string>

struct G_Pokemon {
};

struct PokeAPI {
    virtual void box_view(int num) {};
    virtual void box_edit(int num, int idx) {};
    virtual void box_new(int num, int idx, const struct G_Pokemon &pokemon) {};

    virtual void party_view() {};
    virtual void party_edit(int idx) {};
    virtual void party_new(int idx, const struct G_Pokemon &pokemon) {};

    virtual void bag_view(std::string section) {};
    virtual void bag_edit(std::string item, int quantity) {};
};

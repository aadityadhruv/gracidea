struct g_pokemon {
};

struct pokeapi {
    void (*box_view)(int num);
    void (*box_edit)(int num, int idx);
    void (*box_new)(int num, int idx, struct g_pokemon*);

    void (*party_view);
    void (*party_edit)(int idx);
    void (*party_new)(int idx, struct g_pokemon*);

    void (*bag_view)(int section);
    void (*bag_new)(int item, int quantity);
};

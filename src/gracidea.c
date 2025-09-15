#include "rs/rs.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char** argv) {
    fprintf(stderr, "Starting gracidea...%s", argv[1]);
    struct file* fp;
    load_save_file(argv[1], &fp);
    struct trainer_info* trainer = get_trainer_info(fp);
    struct player_team* team = get_player_team(fp);
    struct pc_buffer* pc = get_pc(fp);
}

#include "rs/rs.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char** argv) {
    fprintf(stderr, "Starting gracidea...%s", argv[1]);
    struct file* fp;
    load_save_file(argv[1], &fp);
    struct trainer_info* trainer = (struct trainer_info*) &fp->save_a[0];
}

#include "rs/rs.h"
#include <stdio.h>
int main(int argc, char** argv) {
    fprintf(stderr, "Starting gracidea...%s", argv[1]);
    load_file(argv[1]);
}

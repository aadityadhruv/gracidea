#include "rs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void load_save_file(char* path, struct file** fp) {
    fprintf(stderr, "Opening file\n");
    FILE *f = fopen(path, "rb");
    fprintf(stderr, "Getting length of file\n");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
    fprintf(stderr, "Read %lu bytes, expecting %lu\n", fsize, sizeof(struct file));
    fprintf(stderr, "reading complete\n");

    char buffer[sizeof(struct file)];
    fread(buffer, sizeof(struct file), 1, f);
    fclose(f);
    fprintf(stderr, "reading complete\n");

    *fp = (struct file*) buffer;
}

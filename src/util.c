#include <stdio.h>

#include "util.h"

void flush_stdin(void) {
    int c;
    while((c = getchar()) != '\n' && c != EOF) {}
}

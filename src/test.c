#include "fat32/fat.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char fbuf[512];
FILE *f;
int getSector(uint32_t k) {
  fseek(f, k * 512, SEEK_SET);
  fread(fbuf, 512, 1, f);

  return 1;
}

void ifree(void *a, uint32_t n) { free(a); }

int main() {
  f = fopen("./fat32.img", "r");

  int out = initSimpleFat32(malloc, ifree, fbuf, getSector, 512);

  printf("RETURN: %d\n", out);
  return 0;
}
#include "fat32/fat.h"
#include <stdio.h>

char fbuf[512];
FILE *f;
int getSector(uint32_t k) {
  fseek(f, k * 512, SEEK_SET);
  fgets(fbuf, 512, f);

  return 1;
}

int main() {
  f = fopen("./fat32.img", "r");

  int out = initSimpleFat32(fbuf, getSector, 512);

  printf("RETURN: %d\n", out);
  return 0;
}
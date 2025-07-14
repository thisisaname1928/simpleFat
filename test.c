#include "src/fat.h"
#include <stdio.h>
#include <string.h>

char buffer[512];
char *printl(char *i, int l) {

  memcpy(buffer, i, l);
  buffer[l] = 0;

  return &buffer[0];
}

int main() {
  FILE *f = fopen("./fat32.img", "r");

  char buffer[512];
  fgets(buffer, 512, f);

  BiosParamaterBlock *b = (BiosParamaterBlock *)buffer;

  printf("OEM ID: %s\n", b->OEMidentifier);
  printf("Begin LBA: %d\n", b->beginSector);
  printf("TTS s: %d\n", b->totalSectorsFat);
  printf("TTS s2: %d\n", b->totalSectorsFatLarge);
  printf("Root Dir: %d\n", b->numberOfRootDir);
  printf("MBR SIZE: %d\n", b->mbrSizeInSector);
  printf("ID: %s\n", printl(b->fat32SystemIDString, 8));
  return 0;
}
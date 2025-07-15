/*
   Copyright 2025 QUOC TRUNG NGUYEN

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
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
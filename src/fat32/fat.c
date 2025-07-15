#include "fat.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buffer[512];
char *printl(char *i, int l) {

  memcpy(buffer, i, l);
  buffer[l] = 0;

  return &buffer[0];
}

char *readBuffer;
uint32_t bufferSize;
int (*readDiskSimpleFat32)(uint32_t sector) = 0;
int isInitialized = 0;
uint32_t currentSector = 0;

BiosParamaterBlock BPB;
FSInfo infoFS;
uint32_t clusterSize;
uint32_t numberOfSectors;
uint32_t fatOffset;
uint32_t firstDataSector;

// bufferSize should be bigger than 512 bytes
#define IS_INITALIZED(val)                                                     \
  if (isInitialized != 1)                                                      \
    return val;                                                                \
  if (bufferSize < 512)                                                        \
  return val

#define readAndCheck(sector, val)                                              \
                                                                               \
  if (!readDiskSimpleFat32(sector))                                            \
    return val;

int readHelper(uint32_t sector) {
  if (currentSector != sector) {
    currentSector = sector;
    return readDiskSimpleFat32(sector);
  }

  return 1;
}

uint32_t cluster2sector(uint32_t index) { return firstDataSector + index - 2; }

uint32_t readFAT32(uint32_t Offset, uint8_t index) {
  uint32_t sector = index * 4 / BPB.BPB_BytsPerSec;
  uint32_t base = index * 4;
  readAndCheck(Offset + sector, 0);

  uint32_t *ptr = (uint32_t *)(readBuffer + base);
  return *ptr & 0x0fffffff; // trunc out last 4 bit
}

int parseFAT32Entry(uint32_t f) {

  if (f >= 0x0ffffff8)
    return FAT32_NO_MORE_CLUSTER;

  if (f == 0x0ffffff7)
    return FAT32_BAD_CLUSTER;

  if (f == 0)
    return FAT32_FREE_CLUSTER;

  return FAT32_NEXT_CLUSTER_PTR;
}

int getFSInfo() {
  readAndCheck(BPB.BPB_FSInfo, FAT32_READ_FAIL);

  FSInfo *f = (FSInfo *)readBuffer;

  if (f->signature1 != FIRST_SIGNATURE || f->signature2 != SECCOND_SIGNATURE) {
    return FAT32_VERIFY_FAILED;
  }

  return 1;
}

int checkAttrInfo(uint8_t attr, uint8_t attrType) {
  return (attr & attrType) == attrType;
}

int parseDirEntry(uint32_t cluster) {
  readAndCheck(cluster2sector(cluster), 1991);

  FAT32Dir *d = (FAT32Dir *)readBuffer;
  if (d->name[0] == 0) {
    printf("ISN'T a valid");
    return -1;
  }

  if ((uint8_t)d->name[0] == 0xe5) {
    if (checkAttrInfo(d->attr, LONG_FILE_NAME_ATTRIBUTE)) {
    }
  }

  return 1;
}

int verifyFat32() {
  readDiskSimpleFat32(512);
  currentSector = 0;
  readAndCheck(0, 0);
  BiosParamaterBlock *b = (BiosParamaterBlock *)readBuffer;

  if (b->BS_BootSig != FAT32_SIGNATURE1 && b->BS_BootSig != FAT32_SIGNATURE2) {

    return 0;
  }
  BPB = *b;

  // get number of sectors
  if (BPB.BPB_TotSec16 == 0)
    numberOfSectors = BPB.BPB_TotSec32;
  else
    numberOfSectors = BPB.BPB_TotSec16;

  printf("Volume string: %s\n", printl(BPB.BS_VolLab, 8));
  printf("Bytes per sector: %d\n", BPB.BPB_BytsPerSec);
  printf("reversed sector: %d\n", BPB.BPB_RsvdSecCnt);
  printf("Cluster size: %d\n", BPB.BPB_SecPerClus * BPB.BPB_BytsPerSec);

  printf("Found FSInfo at sector: %d\n", BPB.BPB_FSInfo);

  fatOffset = BPB.BPB_RsvdSecCnt;

  if (BPB.BPB_BytsPerSec != 512)
    return FAT32_TOO_SIMPLE_NOT_SUPPORT; // this only handle 512 bytes sector

  clusterSize = BPB.BPB_SecPerClus * BPB.BPB_BytsPerSec;

  int s = getFSInfo();
  if (s != 1)
    return s;

  printf("first cluster: %d\n", infoFS.searchOffsetCluster);
  printf("usable space: %d\n", clusterSize * numberOfSectors / 1024 / 1024);
  printf("number of FAT: %d\n", BPB.BPB_NumFATs);
  printf("sector per fat: %d\n", BPB.BPB_FATSz32);
  printf("FAT entries: %d\n", BPB.BPB_FATSz32 * (BPB.BPB_BytsPerSec / 4));

  printf("%x\n", readFAT32(fatOffset, 2));

  firstDataSector = fatOffset + BPB.BPB_FATSz32 * BPB.BPB_NumFATs;

  printf("FFF: %x\n", firstDataSector * 512);

  // some test
  printf("Root Dir at cluster: %d\n", BPB.BPB_RootClus);

  FAT32Dir *d;
  readAndCheck(cluster2sector(BPB.BPB_RootClus), -100);

  d = (FAT32Dir *)readBuffer;
  printf("%s\nfile size: %d\n%x\n", d->name, d->fileSize, d->attr);

  printf("Root Dir FAT32 ENTRY: %x\n", readFAT32(fatOffset, BPB.BPB_RootClus));

  printf("cluster l: %x\n", (d->nextClusterHigh) | (d->nextClusterLow << 16));

  return parseDirEntry(BPB.BPB_RootClus);

  // for (int i = 0; i < BPB.BPB_TotSec16 Large; i++) {
  //   readAndCheck(cluster2sector(i), -182828292);
  //   FAT32Dir *d = (FAT32Dir *)readBuffer;
  //   if (checkAttrInfo(d->attr, FAT32_SYSTEM)) {
  //     printf("name: %s\n", d->name);
  //   }
  // }

  return 1;
}

int initSimpleFat32(char *rB, void *readFuncPtr, uint32_t sizeOfBuffer) {
  readBuffer = rB;
  readDiskSimpleFat32 = readFuncPtr;
  bufferSize = sizeOfBuffer;

  isInitialized = verifyFat32();
  return isInitialized;
}

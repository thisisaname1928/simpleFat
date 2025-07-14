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
uint32_t currentSector;

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
  if (!readDiskSimpleFat32(sector))                                            \
    return val;                                                                \
  else                                                                         \
    currentSector = sector

uint32_t cluster2sector(uint32_t index) { return firstDataSector + index - 2; }

uint32_t readFAT32(uint32_t Offset, uint8_t index) {
  uint32_t sector = index * 4 / BPB.bytesPerSector;
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
  readAndCheck(BPB.fat32FSInfoPtr, FAT32_READ_FAIL);

  FSInfo *f = (FSInfo *)readBuffer;

  if (f->signature1 != FIRST_SIGNATURE || f->signature2 != SECCOND_SIGNATURE) {
    return FAT32_VERIFY_FAILED;
  }

  return 1;
}

int verifyFat32() {
  readAndCheck(0, 0);
  BiosParamaterBlock *b = (BiosParamaterBlock *)readBuffer;

  if (b->fat32Signature != FAT32_SIGNATURE1 &&
      b->fat32Signature != FAT32_SIGNATURE2) {

    return 0;
  }
  BPB = *b;

  // get number of sectors
  if (BPB.totalSectorsFat == 0)
    numberOfSectors = BPB.totalSectorsFatLarge;
  else
    numberOfSectors = BPB.totalSectorsFat;

  printf("Volume string: %s\n", printl(BPB.fat32VolumeName, 8));
  printf("Bytes per sector: %d\n", BPB.bytesPerSector);
  printf("reversed sector: %d\n", BPB.numberOfReservedSector);
  printf("Cluster size: %d\n", BPB.sectorsPerCluster * BPB.bytesPerSector);

  printf("Found FSInfo at sector: %d\n", BPB.fat32FSInfoPtr);

  fatOffset = BPB.numberOfReservedSector;

  if (BPB.bytesPerSector != 512)
    return FAT32_TOO_SIMPLE_NOT_SUPPORT; // this only handle 512 bytes sector

  clusterSize = BPB.sectorsPerCluster * BPB.bytesPerSector;

  int s = getFSInfo();
  if (s != 1)
    return s;

  printf("first cluster: %d\n", infoFS.searchOffsetCluster);
  printf("usable space: %d\n", clusterSize * numberOfSectors / 1024 / 1024);
  printf("number of FAT: %d\n", BPB.numberOfFat);
  printf("FAT entries: %d\n", BPB.fat32SectorsPerFat * BPB.bytesPerSector / 4);

  printf("%x\n", readFAT32(fatOffset, 2));

  firstDataSector = fatOffset + BPB.fat32SectorsPerFat * BPB.numberOfFat;

  printf("FFF: %x\n", firstDataSector * 512);

  // some test
  printf("Root Dir at cluster: %d\n", BPB.fat32RootClusters);

  FAT32Dir *d;
  readAndCheck(cluster2sector(BPB.fat32RootClusters), -100);

  d = (FAT32Dir *)readBuffer;
  printf("%s\nfile size: %d\n", d->name, d->fileSize);

  printf("Root Dir FAT32 ENTRY: 0x%x\n",
         readFAT32(fatOffset, BPB.fat32RootClusters));

  return 1;
}

int initSimpleFat32(char *rB, void *readFuncPtr, uint32_t sizeOfBuffer) {
  readBuffer = rB;
  readDiskSimpleFat32 = readFuncPtr;
  bufferSize = sizeOfBuffer;

  isInitialized = verifyFat32();
  return isInitialized;
}

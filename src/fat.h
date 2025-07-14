#ifndef FAT_H
#define FAT_H

#include <stdint.h>
typedef struct {
  char EB3C90[3]; // idk what name it is...
  char OEMidentifier[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t mbrSizeInSector;
  uint8_t numberOfFat;
  uint16_t numberOfRootDir;
  uint16_t totalSectorsFat;
  uint8_t mediaDescriptorType;
  uint16_t ignored1; // Number of sectors per FAT (Only on Fat 12, fat 16)
  uint16_t sectorsPerTrack;
  uint16_t heads;
  uint32_t beginSector;
  uint32_t totalSectorsFatLarge;
  // EXTENDED BPB
  uint32_t fat32SectorsPerFat;
  uint16_t fat32Flags;
  uint16_t fat32Version;
  uint32_t fat32RootClusters;
  uint16_t fat32FSInfoPtr;
  uint16_t fat32BackUpBootSector;
  char reserved[12];
  uint8_t fa32driveNumber;
  uint8_t fat32WindowsFlags;
  uint8_t fat32Signature;
  uint32_t fat32VolumeSerial;
  char fat32VolumeName[11];
  char fat32SystemIDString[8];
  char fat32BootCode[422];
} __attribute__((packed)) BiosParamaterBlock;

#define FIRST_SIGNATURE 0x41615252
#define SECCOND_SIGNATURE 0x61417272
#define TRAIL_SIGNATURE 0xAA550000
#define NO_HINT 0xFFFFFFFF

typedef struct __attribute__((packed)) {
  uint32_t signature1;
  char reserved[480];
  uint32_t signature2;
  uint32_t lastFreeCluster;
  uint32_t searchOffsetCluster;
  char reserved1[12];
  uint32_t trailSignature;
} FSInfo;

#endif
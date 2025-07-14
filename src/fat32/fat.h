#ifndef FAT_H
#define FAT_H

#include <stdint.h>
typedef struct {
  char EB3C90[3]; // idk what name it is...
  char OEMidentifier[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t numberOfReservedSector;
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
#define FAT32_SIGNATURE1 0x28
#define FAT32_SIGNATURE2 0x29

#define FAT32_NO_MORE_CLUSTER 123
#define FAT32_BAD_CLUSTER 124
#define FAT32_FREE_CLUSTER 125
#define FAT32_NEXT_CLUSTER_PTR 127

#define FAT32_VERIFY_FAILED 0
#define FAT32_TOO_SIMPLE_NOT_SUPPORT -1
#define FAT32_READ_FAIL -2
#define FAT32_ERROR_NO_HINT -4

typedef struct __attribute__((packed)) {
  uint32_t signature1;
  char reserved[480];
  uint32_t signature2;
  uint32_t lastFreeCluster;
  uint32_t searchOffsetCluster;
  char reserved1[12];
  uint32_t trailSignature;
} FSInfo;

typedef struct __attribute__((packed)) {
  char name[8];
  char ext[3];
  uint8_t flags;
  uint8_t reversed;
  uint8_t creationTime;
  uint16_t time;
  uint16_t date;
  uint16_t lastAccessDate;
  uint16_t fistClusterLow;
  uint16_t lastModTime;
  uint16_t lastModDate;
  uint16_t fistClusterHigh;
  uint32_t fileSize;
} FAT32Dir;

#define LONG_FILE_NAME_ATTRIBUTE 0x0f

typedef struct __attribute__((packed)) {
  uint8_t namePtr;
  uint16_t first5Name[5];
  uint8_t attr;
  uint8_t longEntryType;
  uint8_t checkSum;
  uint16_t next6Name[6];
  uint16_t reserved;
  uint16_t next2Name[2];
} longFileNameExt;

int initSimpleFat32(char *readBuffer, void *readFuncPtr, uint32_t sizeOfBuffer);

int verifyFat32();

#endif
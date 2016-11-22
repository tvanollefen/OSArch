/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is the pbs command - print boot sector.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include "pbs.h"
#include "utilities.h"
//#include "fatSupport.h"
#include "sharedMemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DataAttribs data;
FILE* FILE_SYSTEM_ID;

#define BYTES_TO_READ_IN_BOOT_SECTOR 61

void readBootSector(DataAttribs* data)
{
   unsigned char* buffer;
   unsigned int mostSignificantBits;
   unsigned int leastSignificantBits;

   // Set it to this only to read the boot sector
   BYTES_PER_SECTOR = BYTES_TO_READ_IN_BOOT_SECTOR;

   // Then reset it per the value in the boot sector

   buffer = (unsigned char*) malloc(BYTES_PER_SECTOR * sizeof(unsigned char));

   if (read_sector(0, buffer) == -1)
      printf("Something has gone wrong -- could not read the boot sector\n");

   // 12 (not 11) because little endian
   mostSignificantBits  = ( ( (int) buffer[12] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[11] )        & 0x000000ff;

   data->mBytesPerSector = mostSignificantBits | leastSignificantBits;
   data->mSectorsPerCluster = buffer[13];

   mostSignificantBits  = ( ( (int) buffer[15] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[14] )        & 0x000000ff;

   data->mNumReservedSectors = mostSignificantBits | leastSignificantBits;
   data->mNumFATs = buffer[16];

   mostSignificantBits  = ( ( (int) buffer[18] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[17] )        & 0x000000ff;

   data->mNumRootEntries = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[20] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[19] )        & 0x000000ff;

   data->mTotalSectorCount = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[23] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[22] )        & 0x000000ff;

   data->mSectorsPerFAT = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[25] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[24] )        & 0x000000ff;

   data->mSectorsPerTrack = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[27] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[26] )        & 0x000000ff;

   data->mNumHeads = mostSignificantBits | leastSignificantBits;

   data->mBootSignature = buffer[38];

   //I didn't understand how to do little endian with more than two bits, thanks Alex Apmann   
   unsigned int swap;
   swap =   ( ( (int) buffer[42] ) << 24 ) & 0xff000000 |
      ( ( (int) buffer[41] ) << 16 )       & 0x00ff0000 | 
      ( ( (int) buffer[40] ) << 8  )       & 0x0000ff00 |
      ( (int) buffer[39] )                 & 0x000000ff;

   data->mVolumeID = swap;


   unsigned char volumeLabel[] = {buffer[43], buffer[44], buffer[45], buffer[46], buffer[47], buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53], '\0'};

   strncpy(data->mVolumeLabel, volumeLabel, LABEL_LENGTH); //why does this show FAT12??? LABEL_LENGTH - 1 doesn't show FAT12

   unsigned char fileSystemType[] = {buffer[54], buffer[55], buffer[56], buffer[57], buffer[58], buffer[59], buffer[60], buffer[61], '\0'};

   strncpy(data->mFileSystemType, fileSystemType, FILESYSTEM_LENGTH);
}

void printBootSector(DataAttribs data)
{
   //print all the data from struct
   printf("Bytes per Sector: %d\n", data.mBytesPerSector);
   printf("Sectors per Cluster: %d\n", data.mSectorsPerCluster);
   printf("Number of FATs: %d\n", data.mNumFATs);
   printf("Number of Reserved Sectors: %d\n", data.mNumReservedSectors);
   printf("Number of Root Entries: %d\n", data.mNumRootEntries);
   printf("Total Sector Count: %d\n", data.mTotalSectorCount);
   printf("Sectors per Fat: %d\n", data.mSectorsPerFAT);
   printf("Sectors per Track: %d\n", data.mSectorsPerTrack);
   printf("Number of Heads: %d\n", data.mNumHeads);
   printf("Boot Signature: 0x%x\n", data.mBootSignature);
   printf("Volume ID: 0x%x\n", data.mVolumeID);
   printf("Volume Label: %s\n", data.mVolumeLabel);
   printf("File System Type: %s\n", data.mFileSystemType);
}

void usage()
{
   printf("Usage: pbs\nNo arguments necessary when calling pbs.\npbs prints out information from the boot sector.\n");
}

int main(int argc, char* argv[])
{
   if (argc > 1)
   {
      usage();
      return 0;
   }

   //https://beej.us/guide/bgipc/output/html/multipage/shm.html
   //Alex Apmann is God
   int shmId = shmget((key_t) 8888888, sizeof(char), 0666 | IPC_CREAT);
 
   SharedMemory *sharedMemory = (SharedMemory *)shmat(shmId, (void *) 0, 0);
 
   FILE_SYSTEM_ID = fopen(sharedMemory->floppyImgName, "r+");

   if (FILE_SYSTEM_ID == NULL)
   {
      printf("Could not open the floppy drive or image.\n");
      exit(1);
   }

   readBootSector(&data);
   printBootSector(data);
   return 0;
}

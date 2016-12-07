/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is the pbs command - print boot sector.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

//#include "pbs.h"
#include "utilities.h"
//#include "fatSupport.h"
#include "sharedMemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DataAttribs data;
FILE* FILE_SYSTEM_ID;

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

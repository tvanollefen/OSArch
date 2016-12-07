/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is the pfe command - print fat entries.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include "utilities.h"
#include "sharedMemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FAT_SECTORS 9
#define FAT_TABLE_SIZE BYTES_PER_SECTOR * NUM_FAT_SECTORS

DataAttribs data;
FILE* FILE_SYSTEM_ID;

void usage()
{
   printf("\n");
}

int main (int argc, char* argv[])
{
  //https://beej.us/guide/bgipc/output/html/multipage/shm.html
  //Thanks Alex Apmann
  int shmId = shmget((key_t) 8888888, sizeof(char), 0666 | IPC_CREAT);
 
  SharedMemory *sharedMemory = (SharedMemory *)shmat(shmId, (void *) 0, 0);
 
  FILE_SYSTEM_ID = fopen(sharedMemory->floppyImgName, "r+");

   if (FILE_SYSTEM_ID == NULL)
   {
      printf("Could not open the floppy drive or image.\n");
      exit(1);
   }
   
   unsigned char* fat = (unsigned char*)malloc(FAT_TABLE_SIZE);
   int i;

   for(i = 0; i < NUM_FAT_SECTORS; i++)
   {
      read_sector(i + 1, &fat[i * BYTES_PER_SECTOR]);
   }  
   
   readBootSector(&data);
   
   int totalSectors = data.mTotalSectorCount - 31;

   int usedSectors = 0;
   for(i = 2; i < totalSectors; i++)
   {
      int entry = get_fat_entry(i, fat);
      if(entry == 0x00)
      {
         usedSectors++;
      }
   }
   
   printf("Disk Usage:\n");
   printf("Free    - %d\n", usedSectors);
   printf("Used    - %d\n", totalSectors - usedSectors);
   printf("Total   - %d\n", totalSectors);
   printf("Percent - %.2f%%\n", ((float)(totalSectors - usedSectors) / (float)totalSectors) * 100);
}

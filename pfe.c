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
//#include "fatSupport.h"
#include "sharedMemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BYTES_PER_SECTOR 512
#define NUM_FAT_SECTORS 9
#define FAT_TABLE_SIZE BYTES_PER_SECTOR * NUM_FAT_SECTORS

FILE* FILE_SYSTEM_ID;

void print(char* fatTable, int x, int y)
{
   int i;
   for (i = x; i <= y; i++)
   {
       printf("Entry %d: %x\n", i, get_fat_entry(i, fatTable));
   }
}

char* readFAT12Table()
{
   int i;
   char* fatTable = malloc(FAT_TABLE_SIZE);

   for (i = 0; i < NUM_FAT_SECTORS; i++)
   {
      read_sector(i + 1, &fatTable[i * BYTES_PER_SECTOR]);
   }

   return fatTable;
}

int checkRange(int x, int y)
{
   //according to spec
   if (x > y || x < 2)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

void usage()
{
   printf("Usage: pfe int int\nThe first integer must at least 2 and not be greater than the second integer.\nPlease enter only two arguments.\nPrints out all FAT entries.\n");
}

int main (int argc, char* argv[])
{
   int x;
   int y;
   int goodRange;
      
   if (argc != 3)
   {
      usage();
      return 1;
   }
   
   x = atoi(argv[1]);
   y = atoi(argv[2]);
   goodRange = checkRange(x, y);

   if (goodRange == 1)
   {
      usage();
      return 1;
   }

   FILE_SYSTEM_ID = fopen("floppy1", "r+");

   if (FILE_SYSTEM_ID == NULL)
   {
      printf("Could not open the floppy drive or image.\n");
      exit(1);
   }

   char* fatTable = readFAT12Table();
   print(fatTable, x, y);

   
   return 0;
}

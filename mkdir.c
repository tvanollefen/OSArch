/*
  Authors: Paul Turchinetz and Tyler Van Ollefen
  Class: CSI-385-01
  Assignment: FAT
  Due Date: 6 December 2016, 11:59PM
  Description: This is the ls command - list contents.
  Certification of Authenticity:
  I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "sharedMemory.h"

DataAttribs data;
DirectoryOrFile newFile;
FILE* FILE_SYSTEM_ID;

void usage()
{
  printf("Please enter only up to 1 argument.\n");
}

int main(int argc, char **argv)
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

  int i;
  int FLC = 0;
  int realCluster;
  short oldCluster = sharedMemory->firstCluster;
  DirectoryOrFile *infoAtCluster;

  //unsigned char *data = malloc(BYTES_PER_SECTOR);

  if (argc > 2)
    {
      usage();
    }
	
  //i is either the location of the file in the cluster or the FLC of the directory. yeah, weird, I know.
  if (argv[1][0] == '/')
    i = search(0, argv[1], 0, &sharedMemory->firstCluster, &infoAtCluster);
  else
    i = search(sharedMemory->firstCluster, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);
   
    
  if (i < 0) //it wasnt found, we're good to touch ;)
    {
      //readfat12

      readBootSector(&data);
      unsigned char* fat = readFAT12Table();
      int j;
        
      //find free cluster
      int totalSectors = data.mTotalSectorCount - 31;

      //bigBossGuy was for debug but we decided to keep it
      for(j = 2; j < totalSectors; j++)
        {
	  int bigBossGuy = get_fat_entry(j, fat);
	  //printf("Big boss is %d  %x  %d\n", j, bigBossGuy, bigBossGuy);
	  if(bigBossGuy == 0)
            {
	      //printf ("Entry is %d\nj is %d\n", bigBossGuy, j);
	      set_fat_entry(j, 0xFFF, fat);
	      //printf ("Entry %d should now be %d\n",j, 4095);
	      break;
            }
        }


      if(j == totalSectors)
        {
	  printf("There is no available space to write!");
	  return 1;
        }
        
      //write sector
      int k;
      for (k = 0; k < 16; k++)
        {
	  //check the cluster if usable
	  if(infoAtCluster[k].filename[0] == 0x00 || infoAtCluster[k].filename[0] == 0xE5 || infoAtCluster[k].filename[0] == 0xffffffe5)
            {
	      if (infoAtCluster[k].filename[0] == 0x00)
		{
		  strcpy(infoAtCluster[k].filename, argv[1]);
		  infoAtCluster[k].extension[0] = 0x00;
		  infoAtCluster[k].attributes = 0x10;
		  infoAtCluster[k].reserved = 0;
		  infoAtCluster[k].creationTime = 0;
		  infoAtCluster[k].creationDate = 0;
		  infoAtCluster[k].lastAccessDate = 0;
		  infoAtCluster[k].ignoreInFat12 = 0;
		  infoAtCluster[k].lastWriteTime = 0;
		  infoAtCluster[k].lastWriteDate = 0;
		  infoAtCluster[k].firstLogicalCluster = j;
		  infoAtCluster[k].fileSize = 0;		
		  infoAtCluster[++k].filename[0] == 0x00;
		  break;
		}
	      else
		{
		  strcpy(infoAtCluster[k].filename, argv[1]);
		  infoAtCluster[k].extension[0] = 0x00;
		  infoAtCluster[k].attributes = 0x10;
		  infoAtCluster[k].reserved = 0;
		  infoAtCluster[k].creationTime = 0;
		  infoAtCluster[k].creationDate = 0;
		  infoAtCluster[k].lastAccessDate = 0;
		  infoAtCluster[k].ignoreInFat12 = 0;
		  infoAtCluster[k].lastWriteTime = 0;
		  infoAtCluster[k].lastWriteDate = 0;
		  infoAtCluster[k].firstLogicalCluster = j;
		  infoAtCluster[k].fileSize = 0;	
		  break;
		}
            }
        }
        
      if (k == 16)
        {
	  printf("eli eli lama sabanthani\n");
	  set_fat_entry(j, 0, fat);
        }
        
      int realCluster;
      if(sharedMemory->firstCluster == 0)
        {
	  realCluster = 19;
        }
      else
        {
	  realCluster = 31+sharedMemory->firstCluster;
        }
        
      char* bufferToWrite = (char*)malloc(BYTES_PER_SECTOR);
      bufferToWrite = (char*)infoAtCluster;
        
      write_sector(realCluster, bufferToWrite);
        
      writeFAT12Table(fat);
      printf("File created successfully\n");
    }
  else
    {
      printf("Directory already found!\n");
      return 1;
    }

  //free(data);
  return 0;
}

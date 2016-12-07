/*
  Authors: Paul Turchinetz and Tyler Van Ollefen
  Class: CSI-385-01
  Assignment: FAT
  Due Date: 6 December 2016, 11:59PM
  Description: This is the touch command - create file.
  Certification of Authenticity:
  I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "sharedMemory.h"

//#define BYTES_TO_READ_IN_BOOT_SECTOR 61
//#define NUM_FAT_SECTORS 9
//#define FAT_TABLE_SIZE BYTES_PER_SECTOR * NUM_FAT_SECTORS

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
    i = search(0, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);
  else
    i = search(sharedMemory->firstCluster, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);
   
    
  if (i < 0) //it wasnt found, we're good to touch ;)
    {
      //readfat12

      readBootSector(&data);
      unsigned char* fat = readFAT12Table();
      //get_fat_entry(2, fat);
      //set_fat_entry(9, 0xFFF, fat);
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
      //printf("we get here?\n");
      //char* fileAndExtension = argv[1];
      char* fileAndExtension = calloc(strlen(argv[1])+1, sizeof(char));
      strcpy(fileAndExtension, argv[1]);
      //printf("What about here\n");
      char* filename = (char*)malloc(8);
      char* extension = (char*)malloc(3);
                
      filename = strtok(fileAndExtension, ".");
      //printf("Filefnweknfewkfn is %s|, filename is %s|\n", fileAndExtension, filename);

      int l;
      for (l = 0; l < strlen(argv[1]); l++)
	{
	  if (argv[1][l] == '.')
	    {
	      l++;
	      break;
	    }
	}

      if (l >= strlen(argv[1]))
	{
	  printf("Please enter a usable filename\n");
	  return 1;
	}

      int placeInExtension = 0;
      for (l; l < strlen(argv[1]); l++)
	{
	  extension[placeInExtension++] = argv[1][l];
	}
                
      int k; //counter
       
      //printf("Filename is %s\nExtension is %s\n", filename, extension);
      //write sector
      for (k = 0; k < 16; k++)
        {
	  //check the cluster if usable
	  if(infoAtCluster[k].filename[0] == 0x00 || infoAtCluster[k].filename[0] == 0xE5 || infoAtCluster[k].filename[0] == 0xffffffe5)
            {
	      if (infoAtCluster[k].filename[0] == 0x00)
		{
		  strcpy(infoAtCluster[k].filename, filename);
		  strcpy(infoAtCluster[k].extension, extension);
		  infoAtCluster[k].attributes = 0;
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
		  strcpy(infoAtCluster[k].filename, filename);
		  strcpy(infoAtCluster[k].extension, extension);
		  infoAtCluster[k].attributes = 0;
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
      //if I overwrite 0x00 i gotta make teh new one 0x00
    }
  else
    {
      printf("File already found!\n");
      return 1;
    }

  //free(data);
  return 0;
}

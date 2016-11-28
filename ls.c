/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 17 November 2016, 11:59PM
Description: This is the ls command - list contents.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "sharedMemory.h"

FILE* FILE_SYSTEM_ID;

void usage()
{
  printf("Please enter only up to 1 argument.\n");
}

void print(DirectoryOrFile *infoAtCluster)
{
	int i;

	   printf("Name              Type    File Size    FLC\n");

	   for(i = 0; i < 16; i++)
	     {
	       
	       //"If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining
	       //directory entries in this directory are also free." - from the project spec
	       if(infoAtCluster[i].filename[0] == 0x00)
		 {
			printf("This is an entirely free directory\n");
		   break;
		 }

	       //"If the first byte of the Filename field is 0xE5, then the directory entry is free (i.e., currently unused),
	       //and hence there is no file or subdirectory associated with the directory entry." - from the project spec
	       if(infoAtCluster[i].filename[0] == 0xE5 || infoAtCluster[i].filename[0] == 0xffffffe5) 
		 {
			printf("This directory is unused\n");
		   continue;
		}
	
		strtok(infoAtCluster[i].filename, " ");

	       //"If the Attributes byte is 0x0F, then this directory entry is part of a long file name and can be
	       //ignored for purposes of this assignment."
	       if(infoAtCluster[i].attributes != 0x0F)
		 {
		   //if it's a subdirectory
		   if((infoAtCluster[i].attributes & 0x10) == 0x10)
		     {
		       printf("%-15s    DIR            0  %5d\n", infoAtCluster[i].filename, infoAtCluster[i].firstLogicalCluster);
		     }
		   //otherwise it must be a non-long file name and we can print everything
		   else
		     {
			char* fileAndExtension = malloc(12); //8 for filename, 1 for ".", 3 for extension
			strcpy(fileAndExtension, infoAtCluster[i].filename);
			strcat(fileAndExtension, ".");
			strcat(fileAndExtension, infoAtCluster[i].extension);

		       printf("%-15s   FILE   %10d  %5d\n",fileAndExtension, infoAtCluster[i].fileSize, infoAtCluster[i].firstLogicalCluster);
			free(fileAndExtension);
		     }
		 }
	     }
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

   unsigned char *data = malloc(BYTES_PER_SECTOR);

   if (argc > 2)
     {
       usage();
     }
   else if (argc == 2)
     {
	int lookingForFile = 0;
	int j;
	for (j = 0; j < strlen(argv[1])+1; j++)
	{
		if (argv[1][j] == '.')
		{
			lookingForFile = 1;
			break;
		}
	}
	
	//if we only have . or .. we're not looking for files
	if ((argv[1][0] == '.' && strlen(argv[1]) == 1) ||
		(argv[1][0] == '.' && argv[1][1] == '.' && strlen(argv[1]) == 2))
	{
		lookingForFile = 0;
	}
	
	//i is either the location of the file in the cluster or the FLC of the directory. yeah, weird, I know.
	if (argv[1][0] == '/')
		i = search(0, argv[1], lookingForFile, &sharedMemory->firstCluster, &infoAtCluster);
	else
		i = search(sharedMemory->firstCluster, argv[1], lookingForFile, &sharedMemory->firstCluster, &infoAtCluster);

	if (i >= 0 && lookingForFile == 1)
	{
		strtok(infoAtCluster[i].filename, " ");
		printf("Name              Type    File Size    FLC\n");
		char* fileAndExtension = malloc(12); //8 for filename, 1 for ".", 3 for extension
		strcpy(fileAndExtension, infoAtCluster[i].filename);
		strcat(fileAndExtension, ".");
		strcat(fileAndExtension, infoAtCluster[i].extension);
		printf("%-15s   FILE   %10d  %5d\n",fileAndExtension, infoAtCluster[i].fileSize, infoAtCluster[i].firstLogicalCluster);
		free(fileAndExtension);
	}
	else if (i >= 0 && lookingForFile == 0)
	{
		if (i == 0)
			realCluster = 19;
		else
			realCluster = 31 + i;
		read_sector(realCluster, data);
		infoAtCluster = (DirectoryOrFile*) data;
		print(infoAtCluster);
		sharedMemory->firstCluster = oldCluster;
	}
     }
   else
     {
       FLC = sharedMemory->firstCluster;
     }

	if(FLC == 0)
  	{
      		realCluster = 19;
   	}
   	else
   	{
      		realCluster = 31 + FLC;
   	}

   if (argc < 2)
     {
	read_sector(realCluster, data);
	infoAtCluster = (DirectoryOrFile*) data;

	print(infoAtCluster);
	 
     }
	free(data);
   return 0;
}

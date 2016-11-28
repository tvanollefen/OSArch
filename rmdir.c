#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "sharedMemory.h"

FILE* FILE_SYSTEM_ID;

void usage()
{
  printf("Please enter only 1 argument.\n");
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

   if (argc != 2)
     {
       usage();
     }
   else
     {
	//if we only have . or .. we're not looking for files
	if ((argv[1][0] == '.' && strlen(argv[1]) == 1))
	{
		printf("You can't delete the folder you're in.\n");
		return 1;
	}

	if (argv[1][0] == '.' && argv[1][1] == '.' && strlen(argv[1]) == 2)
	{
		if (sharedMemory->firstCluster == 0)
			printf("Fam, this directory is the root.\nIt doesn't have a parent.\nAnd even if it did, it wouldn't be empty.\nSilly goose.\n");
		else
			printf("You're trying to delete this directory's parent directory...\nHow silly is that?\nIt's obviously not empty.\n");
	}
	
	//i is either the location of the file in the cluster or the FLC of the directory. yeah, weird, I know.
	/*if (argv[1][0] == '/')
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
	}*/
     }

	free(data);
   return 0;
}

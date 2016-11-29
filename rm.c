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
   int deletable = 0;
   int realCluster;
   short oldCluster = sharedMemory->firstCluster;
   DirectoryOrFile *infoAtCluster;
   DirectoryOrFile *infoAtOldCluster;
   int lookingForFile = 0;

   unsigned char *data = malloc(BYTES_PER_SECTOR);
   unsigned char *deleteBuffer = malloc(BYTES_PER_SECTOR);

   if (argc != 2)
     {
       usage();
     }
   else
     {	
	if (argv[1][0] == '/')
		i = search(0, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);
	else
		i = search(sharedMemory->firstCluster, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);


	if (i >= 0)
	{
		//infoAtCluster[i].filename must become 0xE5
		//
		if (sharedMemory->firstCluster == 0)
		{
			realCluster = 19;
		}
		else
		{
			realCluster = 31 + sharedMemory->firstCluster;
		}
			read_sector(realCluster, deleteBuffer);
			deleteBuffer[i * 32] = 0xE5;

			write_sector(realCluster, deleteBuffer);
			printf("File successfully deleted!\n");
	}
	else
	{
		printf("That file cannot be deleted.\n");
		sharedMemory->firstCluster = oldCluster;
		return 1;
	}
	sharedMemory->firstCluster = oldCluster;
	
     }

	free(data);
   return 0;
}

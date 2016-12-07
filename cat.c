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
   DirectoryOrFile *infoAtCluster;

   

if (argc != 2)
     {
       usage();
     }
   else
     {
	i = search(sharedMemory->firstCluster, argv[1], 1, &sharedMemory->firstCluster, &infoAtCluster);
	if (i >= 0)
	{
		int numSecs = 0;
   		char* fatBuffer = readFAT12Table();
   		int currentFlc = infoAtCluster[i].firstLogicalCluster;
		//because currentFlc won't ever be 0

   		while(currentFlc != 0 && currentFlc != 4095)
   		{
     			 numSecs++;
      			currentFlc = get_fat_entry(currentFlc, fatBuffer);
  		}
		unsigned char *data = malloc(BYTES_PER_SECTOR * numSecs);

   		currentFlc = infoAtCluster[i].firstLogicalCluster;

		int i = 0;
  		 for(i = 0; i < numSecs; i++)
   		{
			realCluster = currentFlc + 31;
      			read_sector(realCluster, data + (BYTES_PER_SECTOR * i));
			currentFlc = get_fat_entry(currentFlc, fatBuffer);
   		}
		printf("%s\n", data);
	}
     }


	//free(data);
   return 0;
}

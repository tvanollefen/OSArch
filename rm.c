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
   short FLC = -1;
   short oldFLC = -1;
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
		short clusterToReadFileFrom = sharedMemory->firstCluster;
		if (clusterToReadFileFrom == 0)
			clusterToReadFileFrom = 19;
		else
			clusterToReadFileFrom += 31;

		read_sector(clusterToReadFileFrom, deleteBuffer);
		deleteBuffer[i * 32] = 0xE5;
		write_sector(clusterToReadFileFrom, deleteBuffer);


		//FLC will never be 0 because the file can't actually be the root directory
		FLC = infoAtCluster[i].firstLogicalCluster;
		char* fatEntryBuffer = readFAT12Table();

		while (FLC != 0 && FLC != 4095) //FFF -> decimal = 4095
		{
		        oldFLC = get_fat_entry(FLC,fatEntryBuffer);
		        set_fat_entry(FLC, 0, fatEntryBuffer);
			FLC = oldFLC;
		}
		
		writeFAT12Table(fatEntryBuffer);
		
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

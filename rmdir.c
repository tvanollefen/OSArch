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
	if (argv[1][0] == '/')
		i = search(0, argv[1], 0, &sharedMemory->firstCluster, &infoAtCluster);
	else
		i = search(sharedMemory->firstCluster, argv[1], 0, &sharedMemory->firstCluster, &infoAtCluster);


	if (i >= 0)
	{
		if (i == 0)
			realCluster = 19;
		else
			realCluster = 31 + i;

		read_sector(realCluster, data);
		infoAtCluster = (DirectoryOrFile*) data;

		int j;
		for (j = 0; j < 16; j++)
		{
			//if there's something inside the directory it can't be deleted
			if (infoAtCluster[i].filename[0] != 0x00 &&
				infoAtCluster[i].filename[0] != 0xE5 &&
				infoAtCluster[i].filename[0] != 0xffffffe5 &&
				infoAtCluster[i].filename[0] != '.')
			{
				deletable = 1;
				break;
			}
		}
		if (deletable == 0)
		{
			//set filename to 0x00?
			//update fat entry?
		}
		else
		{
			printf("The directory is not empty so we can't delete it.\n");
			return 1;
		}
		//print(infoAtCluster);
		sharedMemory->firstCluster = oldCluster;
	}
     }

	free(data);
   return 0;
}

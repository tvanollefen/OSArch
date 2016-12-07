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
   short FLC = 0;
   short oldFLC = 0;
   int deletable = 0;
   int realCluster;
   short oldCluster = sharedMemory->firstCluster;
   DirectoryOrFile *infoAtCluster;
   DirectoryOrFile *infoAtOldCluster;

   unsigned char *data = malloc(BYTES_PER_SECTOR);
   unsigned char *oldData = malloc(BYTES_PER_SECTOR);

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
			//q is for the directory above
			int q;
			
			q = search(sharedMemory->firstCluster, "..", 0, &sharedMemory->firstCluster, &infoAtCluster);
			//printf("q after searching is %d\n", q);

			int newCluster;
			if (q == 0)
				newCluster = 19;
			else
				newCluster = 31 + q;
			read_sector(newCluster, oldData);
			infoAtOldCluster = (DirectoryOrFile*) oldData;

			//printf("our argument has changed maybe i hope please and it is now %s\n", argv[1]);
			//strcat(argv[1], "\0");

			for (q = 0; q < 16; q++)
			{
				//printf("We're gonna compare %s and %s\n", infoAtOldCluster[q].filename, argv[1]);
				char* fixedName = malloc(8);
				int g;
				for (g = 0; g < 8; g++)
				{
					if(infoAtOldCluster[q].filename[g] != ' ')
						fixedName[g] = infoAtOldCluster[q].filename[g];
					else
						break;
				}
				//printf("fixedName is %s, and argv is %s,\n", fixedName, argv[1]);				

				if (strcmp(fixedName, argv[1]) == 0)
					break;
			}

			//printf ("q is %d\n", q);

			char* deletionBuffer = malloc(BYTES_PER_SECTOR);
			char* deleteFromAboveBuffer = malloc(BYTES_PER_SECTOR);
			int k;
			for (k = 0; k <= BYTES_PER_SECTOR; k++)
			{
				if (k == 0)
					deletionBuffer[k] = 0xE5;
				else
					deletionBuffer[k] = 0;
			}

			read_sector(newCluster, deleteFromAboveBuffer);
			//32 bytes per file/directory info
			deleteFromAboveBuffer[q * 32] = 0xE5;

			
			
			write_sector(realCluster, deletionBuffer);
			write_sector(newCluster, deleteFromAboveBuffer);

			//FLC will never be 0 because the file can't actually be the root directory
			FLC = infoAtOldCluster[q].firstLogicalCluster;
			char* fatEntryBuffer = readFAT12Table();

			while (FLC != 0 && FLC != 4095) //FFF -> decimal = 4095
			{
		 	        oldFLC = get_fat_entry(FLC,fatEntryBuffer);
		 	        set_fat_entry(FLC, 0, fatEntryBuffer);
				FLC = oldFLC;
			}
		
			writeFAT12Table(fatEntryBuffer);

			printf("Directory successfully deleted!\n");
		}
		else
		{
			printf("The directory is not empty so we can't delete it.\n");
			sharedMemory->firstCluster = oldCluster;
			return 1;
		}
		//print(infoAtCluster);
		sharedMemory->firstCluster = oldCluster;
	}
     }

	free(data);
   return 0;
}

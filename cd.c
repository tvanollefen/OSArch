/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is the cd command - change directory.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedMemory.h"
#include "utilities.h"

FILE* FILE_SYSTEM_ID;

void usage()
{
  printf("Please enter only one argument.\nMake sure the directory actually exists!\n");
}

int main(int argc, char **argv)
{

  //https://beej.us/guide/bgipc/output/html/multipage/shm.html
  //Thank you Alex Apmann for the sweet tips on shared memory.
  int shmId = shmget((key_t) 8888888, sizeof(char), 0666 | IPC_CREAT);
 
  SharedMemory *sharedMemory = (SharedMemory *)shmat(shmId, (void *) 0, 0);
 
  FILE_SYSTEM_ID = fopen(sharedMemory->floppyImgName, "r+");
  //FILE_SYSTEM_ID = fopen("floppy1", "r+");

  if (FILE_SYSTEM_ID == NULL)
    {
      printf("Could not open the floppy drive or image.\n");
      exit(1);
    }

  DirectoryOrFile *unused;

  if (argc == 2)
    {
	
      //absolute path first
      if (argv[1][0] == '/')
	{
		if (search(0, argv[1], 0, &sharedMemory->firstCluster, &unused) >= 0)
		{
	 		strcpy(sharedMemory->curDir, argv[1]);
		}
	}
      //if you're not in the home directory, add a slash before concatenating the strings
	//also make sure we don't search from the beginning because 
	else if (search(sharedMemory->firstCluster, argv[1], 0, &sharedMemory->firstCluster, &unused) >= 0)
	{
		if (strcmp(sharedMemory->curDir, sharedMemory->homeDir))
		{
			strcat(sharedMemory->curDir, "/");
	      		strcat(sharedMemory->curDir, argv[1]);
		}
	      //just add on the string to the home directory
		else
		{
			strcat(sharedMemory->curDir, argv[1]);
		}
	}
    }
  else if (argc > 2)
    {
      usage();
	return 1;
    }
  else
    {
      strcpy(sharedMemory->curDir, sharedMemory->homeDir);
    } 

  return 0;
}

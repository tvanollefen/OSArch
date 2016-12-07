/*
  Authors: Paul Turchinetz and Tyler Van Ollefen
  Class: CSI-385-01
  Assignment: FAT
  Due Date: 2 November 2016, 11:59PM
  Description: This is the SHELL.
  Certification of Authenticity:
  I certify that this assignment is entirely our own work unless cited otherwise.
  Referenced: https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utilities.h"
//#include "fatSupport.h"
#include "sharedMemory.h"

#define MAX_LENGTH 1024
size_t MAX_INPUT_LENGTH = 256;
char* FLOPPY_IMAGE_NAME = "floppy2";
FILE* FILE_SYSTEM_ID;


char* builtin_name[] = {
  "help",
  "exit",
  "pbs",
  "pfe",
  "cd",
  "pwd",
  "ls",
  "cat",
  "rmdir",
  "rm",
  "df",
  "touch",
  "mkdir"
};

int builtinCount()
{
  return sizeof(builtin_name)/sizeof(char *);
}

int fnc_exit() 
{
  exit(0);
}

void fnc_help()
{
  printf("\nColonel Kernel OSA Shell\n");
  printf("Paul Turchinetz and Tyler VanOllefen 2016\n");
  printf("\nThe following commands are builtin to the shell:\n");
   
  int i;
  for (i = 0; i < builtinCount(); i++)
    {
      printf("%s\n", builtin_name[i]);
    }
  printf("\n");
}

int forkAndExec(int argc, char *argv[])
{
  pid_t pid;
  int status = 0;
      
  pid = fork();
  if (pid == 0)
    {
      char path[30];
      sprintf(path, "%s%s", "./", argv[0]);
      execv(path, argv);
    }
  else if (pid < 0)
    {
      printf("Lol couldn't fork, that's a big problem\n");
      return 1;
    }
  else if (pid > 0)
    {
      waitpid(pid, &status, 0);
    }
  return status;
}

int main(void)
{
  int argc;
  char *argv[10];

  //https://beej.us/guide/bgipc/output/html/multipage/shm.html
  //ALEX APMANN
  int shmId = shmget((key_t) 8888888, sizeof(char), 0666 | IPC_CREAT);

  SharedMemory *sharedMemory = (SharedMemory *)shmat(shmId, (void *) 0, 0);

  /*FILE_SYSTEM_ID = fopen(FLOPPY_IMAGE_NAME, "r+");

    if (FILE_SYSTEM_ID == NULL)
    {
    printf("Could not open the floppy drive or image.\n");
    exit(1);
    }*/

  strcpy(sharedMemory->curDir, "/"); //set starting directory to '/'
  strcpy(sharedMemory->homeDir, "/"); //set home directory to '/'
  strcpy(sharedMemory->floppyImgName, FLOPPY_IMAGE_NAME); //set floppy image to whatever is up top (can change later)
  sharedMemory->firstCluster = 0; //first logical cluster of FAT is 0
   
  while (1)
    {
      char *line = malloc(MAX_INPUT_LENGTH * sizeof(char));
      int i;

      //clear out the argv after an iteration of the shell
      for (i = 0; i < 10; i++)
	{
	  argv[i] = NULL;
	}
      argc = 0;
      printf("$ ");
      int buflength = 0;

      //read in the line
      getline(&line, &MAX_INPUT_LENGTH, stdin);
      line = strtok(line, "\n");

      //tokenize the line and split into arguments
      char *token;
      token = strtok(line, " ");
      while (token != NULL)
	{
	  argv[argc]=token;
	  argc++;
	  token = strtok(NULL, " ");
	}
      
      //handle the builtin functions
      if(strcmp(argv[0],"help") == 0)
	{
	  fnc_help();
	}
      else if(strcmp(argv[0],"exit") == 0)
	{
	  fnc_exit();
	}

      //handle the other functions in the directory
      else if(strcmp(argv[0], "pfe")   == 0 ||
	      strcmp(argv[0], "pbs")   == 0 ||
	      strcmp(argv[0], "cd" )   == 0 ||
	      strcmp(argv[0], "pwd")   == 0 ||
	      strcmp(argv[0], "ls" )   == 0 ||
	      strcmp(argv[0], "cat")   == 0 ||
	      strcmp(argv[0], "rmdir") == 0 ||
	      strcmp(argv[0], "rm")    == 0 ||
	      strcmp(argv[0], "df")    == 0 ||
	      strcmp(argv[0], "touch") == 0 ||
	      strcmp(argv[0], "mkdir") == 0)
	{
	  forkAndExec(argc, argv);
	}
      else
	{
	  printf("Command not found: %s\nPlease type 'help' for help.\n", argv[0]);
        }

      free(line);
      
    }

  //deletes the shared memory, thanks alex apmann
  shmctl(shmId, IPC_RMID, NULL);

   
  return 0;
}

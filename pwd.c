/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is the pwd command - print working directory.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <stdio.h>
#include <stdlib.h>
#include "sharedMemory.h"

int main(int argc, char **argv)
{
   //https://beej.us/guide/bgipc/output/html/multipage/shm.html
   //ALEX APMANNNNNN
   int shmId = shmget((key_t) 8888888, sizeof(char), 0666 | IPC_CREAT);
 
   SharedMemory *sharedMemory = (SharedMemory *)shmat(shmId, (void *) 0, 0);
 
   //this one was a doozy
   printf("%s\n", sharedMemory->curDir);

   return 0;
}

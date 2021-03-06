/*
  Authors: Paul Turchinetz and Tyler Van Ollefen
  Class: CSI-385-01
  Assignment: FAT
  Due Date: 6 December 2016, 11:59PM
  Description: This is sharedMemory. Alex Apmann explained to us how to use it. My hero. (Our hero).
  Certification of Authenticity:
  I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

key_t SHM_KEY = 8888888;

// With help from Alexander Apmann
typedef struct SharedMemory
{
  char curDir[128]; //current directory
  char homeDir[128]; //home directory
  char floppyImgName[32]; //floppy image name
  short firstCluster; //first logical cluster
} SharedMemory;

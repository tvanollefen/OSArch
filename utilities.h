/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 17 November 2016, 11:59PM
Description: This is utilities.h. It has utilities.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#ifndef __UTILITITIES__
#define __UTILITITIES__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_FAT_SECTORS 9
#define FAT_TABLE_SIZE BYTES_PER_SECTOR * NUM_FAT_SECTORS
//int BYTES_PER_SECTOR = 512;

//All this is from the project spec
#define FILE_NAME_LENGTH 8
#define FILE_EXTENSION_LENGTH 3
#define FILE_ATTRIBUTES_LENGTH 1
#define FIRST_LOGICAL_CLUSTER_SIZE 2
#define FILE_SIZE_SIZE 4

#define FILE_NAME_OFFSET 0
#define FILE_EXTENSION_OFFSET 8
#define FILE_ATTRIBUTES_OFFSET 11
#define FIRST_LOGICAL_CLUSTER_OFFSET 26
#define FILE_SIZE_OFFSET 28

#define BYTES_TO_BE_READ 32

#define FIRST_SECTOR 19

#define NUM_DIRECTORIES 16

extern FILE* FILE_SYSTEM_ID;
extern int BYTES_PER_SECTOR;


//DAVID JORDAN IS OUR HERO HE HELPED US UNDERSTAND
#pragma pack(1)
typedef struct DirectoryOrFile
{
	char filename[8];
	char extension[3];
	char attributes;
	short reserved;
	short creationTime;
	short creationDate;
	short lastAccessDate;
	short ignoreInFat12;
	short lastWriteTime;
	short lastWriteDate;
	short firstLogicalCluster;
	int fileSize;
} DirectoryOrFile;
#pragma pack()

int search(short FLC, const char* target, int directoryOrFile, short *FLCTwoElectricBoogaloo);
int stringCompareTwoElectricBoogaloo(char *str1, char *str2);
//void printTestInfo(DirectoryOrFile *directory, int i);

int read_sector(int sector_number, unsigned char* buffer);
int write_sector(int sector_number, unsigned char* buffer);

int get_fat_entry(int fat_entry_number, unsigned char* fat);
void set_fat_entry(int fat_entry_number, int value, unsigned char* fat);


#endif

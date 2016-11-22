/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is pbs.h which just defines the struct data.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#ifndef UTILS_H_
#define UTILS_H_

#define LABEL_LENGTH  11
#define FILESYSTEM_LENGTH 8

//extern FILE * FILE_SYSTEM_ID

typedef struct DataAttribs
{
   int mBytesPerSector;
   int mSectorsPerCluster;
   int mNumFATs;
   int mNumReservedSectors;
   int mNumRootEntries;
   int mTotalSectorCount;
   int mSectorsPerFAT;
   int mSectorsPerTrack;
   int mNumHeads;
   int mBootSignature;
   int mVolumeID;
   char mVolumeLabel[LABEL_LENGTH];
   char mFileSystemType[FILESYSTEM_LENGTH];
} DataAttribs;

#endif

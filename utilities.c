//TODO: ACCOUNT FOR FILES WITHOUT EXTENSIONS LIKE "TEST1."

/*
Authors: Paul Turchinetz and Tyler Van Ollefen
Class: CSI-385-01
Assignment: FAT
Due Date: 2 November 2016, 11:59PM
Description: This is utilities.c. It takes everything from fatSupport and adds our own utility functions.
Certification of Authenticity:
I certify that this assignment is entirely our own work unless cited otherwise.
*/

#include "utilities.h"

int BYTES_PER_SECTOR = 512;

char* readFAT12Table()
{
   int i;
   char* buffer = (char*)malloc(FAT_TABLE_SIZE);

   for (i = 0; i < NUM_FAT_SECTORS; i++)
   {
      read_sector(i + 1, &buffer[i * BYTES_PER_SECTOR]);
   }

   return buffer;
}

int writeFAT12Table(char* buffer)
{
   int i;
   for (i = 0; i < NUM_FAT_SECTORS; i++)
   {
      write_sector(i + 1, &buffer[i * BYTES_PER_SECTOR]);
   }
}


//DAVID JORDAN HELPED US UNDERSTAND READING THE DATA INTO THE STRUCT
//returns location of single file information, -1 if unimportant but found, or -2 if not found (so if it's >= 0 it's found successfully)
//returns FLC if looking for directory
//oh boy what a mess
int search(short FLC, const char* target, int directoryOrFile, short* FLCTwoElectricBoogaloo, DirectoryOrFile **infoAtCluster)
{
	//printf ("directoryOrFile at the beginning is %d\n", directoryOrFile);
	int i = 0;
	
	char* data = malloc(BYTES_PER_SECTOR);
	
	int realCluster;

	char* temp = calloc(strlen(target)+1, sizeof(char));
	strcpy(temp, target);

	char *token;

	int j;
	int slashCount = 0;
	
	//check to see how many slashes in the token

	for (j = 0; j < strlen(target)+1; j++)
	{
		if (temp[j] == '/')
		{
			slashCount++;
		}
	}

	
	
	//if there's only one slash and it's at the beginning and then it finds a dot, that means we're looking for a file in the root directory
	//so slash count should be 0
	if (slashCount == 1 && target[0] == '/')
	{
		for (j = 0; j < strlen(target)+1; j++)
		{
			if (temp[j] == '.')
			{

				slashCount = 0;
				break;
			}
		}
	}

	/*if (target[strlen(target) - 1] == '.')
	{
		
	}*/

	//lol this just works don't mind it
	if (slashCount > 1 && target[0] == '/')
		slashCount--;


      	token = strtok(temp, "/");

	while (token != NULL)
	{
		if(FLC == 0)
   		{
      			realCluster = 19;
   		}
   		else
   		{
      			realCluster = 31 + FLC;
   		}
		
		read_sector(realCluster, data);
		infoAtCluster[0] = (DirectoryOrFile*) data;

		i = 0;
		while (i < 16)
		{
			char* fileAndExtension = malloc(12); //8 for filename, 1 for ".", 3 for extension
			//dont h8 but this works
			if (directoryOrFile == 1)
			{
				//if we know it's a file
				if (slashCount <= 0)
					strtok(infoAtCluster[0][i].filename, " ");

				//else only this
				strcpy(fileAndExtension, infoAtCluster[0][i].filename);
				strcat(fileAndExtension, ".");

							
				strcat(fileAndExtension, infoAtCluster[0][i].extension);

				//printf("We have %s\n", fileAndExtension);
				//printf("We're looking for %s\n", token);
			}
			else
			{
				strcpy(fileAndExtension, infoAtCluster[0][i].filename);
			}
			
			if (stringCompareTwoElectricBoogaloo(fileAndExtension, token) == 0)
			{
				break;
			}
			else
			{
				i++;
			}	
			free(fileAndExtension);
		}
		if (i < 16)
		{
			if (validate(directoryOrFile, infoAtCluster[0][i].extension) == 1)
			{

				//int poop = validate(directoryOrFile, infoAtCluster[0][i].extension);
				//printf ("Poop is %d\ndirectoryOrFile is %d\n", poop, directoryOrFile);
				if (directoryOrFile == 1)
				{
					FLC = infoAtCluster[0][i].firstLogicalCluster;
					
					return i; 
				}
				else
				{
					FLC = infoAtCluster[0][i].firstLogicalCluster;

				}
			}
			else
			{
				if (directoryOrFile == 0)
					printf("You can't do that with a file, friend.\n");
				else if (directoryOrFile == 1)
					{
						FLC = infoAtCluster[0][i].firstLogicalCluster;
					}
				else
					printf("Bro, how did you even manage this? LOL\n");
			}
		}
		else
		{
			if (directoryOrFile == 0)
				printf("Directory not found.\n");
			else if (directoryOrFile == 1)
				printf("File not found.\n");
			else
				printf("Bro, how did you even manage this? LMAO\n");
			return -2;
		}
		slashCount--;
		token = strtok(NULL, "/");
	}
	
	*FLCTwoElectricBoogaloo = FLC;
	free(data);
	free(temp);	
	return FLC;
}

int stringCompareTwoElectricBoogaloo(char *str1, char *str2)
{	
	int i;
	for(i = 0; i < 12; i++)
	{
		
		if (str1[i] == ' ' && str2[i] == '\0')
		{
			//it didnt fail before it ended
			return 0;
		}
		else if (str1[i] == ' ' || str2[i] == '\0')
		{
			//theyre not the same length
			return 1;
		}
		if (str1[i] != str2[i])
		{
			//its not equal
			return 1;
		}
	}

	return 0;
}

int validate(int directoryOrFile, char *extension)
{
	if (directoryOrFile == 0)
	{
		//if extension[0] is a space that means it's a directory
		//return 1 if it's a directory if we're looking for a directory
		//return 0 if it's a file if we're looking for a directory
		return (extension[0] == ' ');
	}
	else if (directoryOrFile == 1)
	{
		//if extension[0] is not a space that means it's a file
		//return 1 if it's a file if we're looking for a file
		//return 0 if it's a directory if we're looking for a file
		return (extension[0] != ' ');
	}
	else
	{
		printf("This should never be seen so I dunno how you got here.\n");
		return 1;
	}
}

void readBootSector(DataAttribs* data)
{
   unsigned char* buffer;
   unsigned int mostSignificantBits;
   unsigned int leastSignificantBits;

   // Set it to this only to read the boot sector
   BYTES_PER_SECTOR = BYTES_TO_READ_IN_BOOT_SECTOR;

   // Then reset it per the value in the boot sector

   buffer = (unsigned char*) malloc(BYTES_PER_SECTOR * sizeof(unsigned char));

   if (read_sector(0, buffer) == -1)
      printf("Something has gone wrong -- could not read the boot sector\n");

   // 12 (not 11) because little endian
   mostSignificantBits  = ( ( (int) buffer[12] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[11] )        & 0x000000ff;

   data->mBytesPerSector = mostSignificantBits | leastSignificantBits;
   data->mSectorsPerCluster = buffer[13];

   mostSignificantBits  = ( ( (int) buffer[15] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[14] )        & 0x000000ff;

   data->mNumReservedSectors = mostSignificantBits | leastSignificantBits;
   data->mNumFATs = buffer[16];

   mostSignificantBits  = ( ( (int) buffer[18] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[17] )        & 0x000000ff;

   data->mNumRootEntries = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[20] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[19] )        & 0x000000ff;

   data->mTotalSectorCount = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[23] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[22] )        & 0x000000ff;

   data->mSectorsPerFAT = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[25] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[24] )        & 0x000000ff;

   data->mSectorsPerTrack = mostSignificantBits | leastSignificantBits;

   mostSignificantBits  = ( ( (int) buffer[27] ) << 8 ) & 0x0000ff00;
   leastSignificantBits =   ( (int) buffer[26] )        & 0x000000ff;

   data->mNumHeads = mostSignificantBits | leastSignificantBits;

   data->mBootSignature = buffer[38];

   //I didn't understand how to do little endian with more than two bits, thanks Alex Apmann   
   unsigned int swap;
   swap =   ( ( (int) buffer[42] ) << 24 ) & 0xff000000 |
      ( ( (int) buffer[41] ) << 16 )       & 0x00ff0000 | 
      ( ( (int) buffer[40] ) << 8  )       & 0x0000ff00 |
      ( (int) buffer[39] )                 & 0x000000ff;

   data->mVolumeID = swap;


   unsigned char volumeLabel[] = {buffer[43], buffer[44], buffer[45], buffer[46], buffer[47], buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53], '\0'};

   strncpy(data->mVolumeLabel, volumeLabel, LABEL_LENGTH); //why does this show FAT12??? LABEL_LENGTH - 1 doesn't show FAT12

   unsigned char fileSystemType[] = {buffer[54], buffer[55], buffer[56], buffer[57], buffer[58], buffer[59], buffer[60], buffer[61], '\0'};

   strncpy(data->mFileSystemType, fileSystemType, FILESYSTEM_LENGTH);
}

/******************************************************************************
 * Supporting functions for the FAT project:
 *
 *  read_sector
 *  write_sector
 *
 *  get_fat_entry
 *  set_fat_entry
 *  
 * Authors: Andy Kinley, Archana Chidanandan, David Mutchler and others.
 *          March, 2004.
 *****************************************************************************/


/******************************************************************************
 * FILE_SYSTEM_ID -- the file id for the file system (here, the floppy disk
 *                   filesystem)
 * BYTES_PER_SECTOR -- the number of bytes in each sector of the filesystem
 *****************************************************************************/


/******************************************************************************
 * read_sector
 *
 * Read the specified sector from the file system and store that sector in the
 * given buffer
 *
 * sector_number:  The number of the sector to read (0, 1, 2, ...)
 * buffer:  The array into which to store the contents of the sector that is
 *          read
 *
 * Return: the number of bytes read, or -1 if the read fails.
 *****************************************************************************/

int read_sector(int sector_number, unsigned char* buffer)
{
   int bytes_read;

   if (fseek(FILE_SYSTEM_ID,
             (long) sector_number * (long) BYTES_PER_SECTOR, SEEK_SET) != 0)
   {
	   printf("Error accessing sector %d\n", sector_number);
      return -1;
   }

   bytes_read = fread(buffer, sizeof(char), BYTES_PER_SECTOR, FILE_SYSTEM_ID);

   if (bytes_read != BYTES_PER_SECTOR)
   {
      printf("Error reading sector %d\n", sector_number);
      return -1;
   }

   return bytes_read;
}


/*****************************************************************************
 * write_sector
 *
 * Write the contents of the given buffer to the filesystem at the specified
 * sector
 *
 * sector_number:  The number of the sector to write (0, 1, 2, ...)
 * buffer:  The array whose contents are to be written
 *
 * Return: the number of bytes written, or -1 if the read fails.
 ****************************************************************************/

int write_sector(int sector_number, unsigned char* buffer) 
{
   int bytes_written;

   if (fseek(FILE_SYSTEM_ID,
       (long) sector_number * (long) BYTES_PER_SECTOR, SEEK_SET) != 0) 
   {
      printf("Error accessing sector %d\n", sector_number);
      return -1;
   }

   bytes_written = fwrite(buffer,
                          sizeof(char), BYTES_PER_SECTOR, FILE_SYSTEM_ID);

   if (bytes_written != BYTES_PER_SECTOR) 
   {
      printf("Error reading sector %d\n", sector_number);
      return -1;
   }

   return bytes_written;
}


/*****************************************************************************
 * get_fat_entry
 *
 * Get the specified entry from the given FAT
 *
 * fat_entry_number:  The number of the FAT entry to get (0, 1, 2, ...)
 * fat:  The fat table from which to get the specified entry
 *
 * printf("Directory listing returned %d entries:\n", dnum);
 * Return: the value at the specified entry of the given FAT
 ****************************************************************************/

int get_fat_entry(int fat_entry_number, unsigned char* fat) 
{
   int offset;
   int uv, wx, yz;

   offset = 3 * fat_entry_number / 2;

   // Two FAT12 entries are stored into three bytes;
   // if these bytes are uv,wx,yz then the two FAT12 entries are xuv and yzw

   // odd fat entry number, return yzw
   if (fat_entry_number & 0x0001) 
   {
      wx = (int) fat[offset];
      yz = (int) fat[offset + 1];
      return ( (yz << 4)  |  ( (wx & 0x00f0) >> 4));
   } 
   // even fat entry number, return xuv
   else 
   {
      uv = (int) fat[offset];
      wx = (int) fat[offset + 1];
      return ( ((wx & 0x000f) << 8)  |  uv );
   }
}


/******************************************************************************
 * set_fat_entry
 *
 * Set the specified entry in the given FAT to the given value
 *
 * fat_entry_number:  The number of the FAT entry to set (0, 1, 2, ...)
 * value:  The given value to place in the FAT entry
 * fat:  The fat table in which to set the given value at the specified entry
 *****************************************************************************/

void set_fat_entry(int fat_entry_number, int value, unsigned char* fat) 
{
   int offset;
   int uv, wx, yz, a, b, c;

   offset = 3 * fat_entry_number / 2;

   // Two FAT12 entries are stored into three bytes;
   // if these bytes are uv,wx,yz then the two FAT12 entries are xuv and yzw
   // Let 0a,bc denote the fat_entry_number, written as two bytes (high and
   // low, respectively)

   a = value & 0x0f00;
   b = value & 0x00f0;
   c = value & 0x000f;

   // odd fat entry number, change yzw to abc, i.e.,
   if (fat_entry_number & 0x0001) 
   {
      // wx = cx;
      fat[offset]     = (unsigned char) ((c << 4)  |  (fat[offset] & 0x000f));
      // yz = ab;
      fat[offset + 1] = (unsigned char) ((a >> 4)  |  (b >> 4));
   }
   // even fat entry number, change xuv to abc, i.e.,
   else
   {
      // uv = bc;
      fat[offset]     = (unsigned char) (b | c);
      // wx = wa;
      fat[offset + 1] = (unsigned char) ((fat[offset + 1]  & 
                                          0x00f0)  |  (a >> 8));
   }
}

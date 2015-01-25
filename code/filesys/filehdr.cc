// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

#ifdef CHANGED

FileHeader::FileHeader()
{
     numBytes = 0;
     numSectors = 0;
}

FileHeader::~FileHeader()
{
}
#endif
//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

//able to support both initialize and increment space !

bool
FileHeader::Allocate(BitMap *freeMap, int Size)
{ 
#ifdef CHANGED
    if (Size == 0) return TRUE;
    int i, j, k;

//calculate required number of sectors
    int newSectors = divRoundUp(Size, SectorSize);

//calculate number of allocated indexs
    int index = numSectors / MaxPerSector;
 
//calculate number of needed indexs
    int newindex = (newSectors + numSectors) / MaxPerSector - index;

//NumClear() return available number of sectors in disk
    if ((freeMap->NumClear() < (newindex + newSectors)) || (numBytes + Size > (int)MaxFileSize))
	return FALSE;		// not enough space

//allocate the initial first sector when file is newly created
    if (numSectors == 0)
        ASSERT (dataSectors[0] = freeMap->Find());
    
//create a buffer space to store sector# info
    int *dataset = new int[MaxPerSector];

//read the current index# info into buffer
    synchDisk->ReadSector(dataSectors[index],(char*)dataset);

//k is the number of byte;j is the number of numSectors;i is the number of needed sectors
    for (k = 0,i = 0,j = numSectors % MaxPerSector;i < newSectors;i++)
    {
        while (k < Size)
        {
              if (!(numBytes % SectorSize)) //if number of byte does reach one sector size,we need to jump into next sector
              {

                   //allocate new sector space
                    dataset[j] = freeMap->Find();
                    j = (j + 1) % MaxPerSector;

                   //if number of numSectors reach one index size,we need to jump into next index
                    if (j == 0)
                    {
                          synchDisk->WriteSector(dataSectors[index],(char*)dataset);
                          if (index < (int)(NumDirect - 1))
                          {
                             index++;
                             dataSectors[index] = freeMap->Find();
                          }
                    }
              }

              //the number of k reach one sector size,we jump out to increment i
              if ((k > 0) && (!(k % SectorSize)))
              {
                    numBytes++;
                    k++;
                    break;
              }
   
              //else we keep increasing byte by byte
              numBytes++;
              k++;
        }
    }
    synchDisk->WriteSector(dataSectors[index],(char*)dataset);
    numSectors = j + index * MaxPerSector;
#else
    numBytes = Size;
    numSectors  = divRoundUp(Size, SectorSize);
    if (freeMap->NumClear() < numSectors)
        return FALSE;           // not enough space

    for (int i = 0; i < numSectors; i++)
        dataSectors[i] = freeMap->Find();
#endif
    return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//      "reservebytes" start from 1 byte if we want to reserve a certain number of bytes
//----------------------------------------------------------------------

#ifdef CHANGED
void 
FileHeader::Deallocate(BitMap *freeMap, int reservebytes)
{
    int *dataset = new int[MaxPerSector];

//freesector is where we start to release sector#(start from zero)
    int freesector = reservebytes / SectorSize;
    if (reservebytes % SectorSize)
        freesector++;

//index is where we start to read index to relase all(start from zero)
    int index = freesector / MaxPerSector;

    synchDisk->ReadSector(dataSectors[index],(char*)dataset);
    for (int j = freesector % MaxPerSector,i = freesector;i < numSectors;i++,j = ( j + 1 ) % MaxPerSector) 
    {
        if(j == 0 && i > freesector && index < (int)(NumDirect - 1))
        {
               index++;
               if ((i - freesector) >= (int)MaxPerSector)
               {
                   ASSERT(freeMap->Test((int) dataSectors[index]));
                   freeMap->Clear((int) dataSectors[index]);
               }
               synchDisk->ReadSector(dataSectors[index],(char*)dataset);
        }
        ASSERT(freeMap->Test((int) dataset[j]));  // ought to be marked!

//deallocate space meaning mark 0 in bitmap
        freeMap->Clear((int) dataset[j]);
    }
    numSectors = freesector;
    numBytes = reservebytes;
#else
void
FileHeader::Deallocate(BitMap *freeMap)
{

    for (int i = 0; i < numSectors; i++) {
        ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
        freeMap->Clear((int) dataSectors[i]);
    }
#endif
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
#ifdef CHANGED
    if(offset > FileLength())
        return -1;
    int sectors,indexs;
    //if (offset % SectorSize || offset == 0)
        sectors = offset / SectorSize;
    //else
     //   sectors = offset / SectorSize - 1;
    indexs = sectors / MaxPerSector;
    int *dataset = new int[MaxPerSector];
    synchDisk->ReadSector(dataSectors[indexs],(char*)dataset);

//return sector# where offset byte data block settled in
    return(dataset[sectors%MaxPerSector]);
#else
    return(dataSectors[offset / SectorSize]);
#endif
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
#ifdef CHANGED
    int i, j, k, t, index = 0;
    char *data = new char[SectorSize];
    int *dataset = new int[MaxPerSector];
    synchDisk->ReadSector(dataSectors[index],(char*)dataset);
    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    for (i = 0,t = 0;i < numSectors;i++,t = (t + 1) % MaxPerSector)
    {
        if (i > 0 && t == 0)
        {
             index++;
             synchDisk->ReadSector(dataSectors[index],(char*)dataset);
        }
	printf("%d ", dataset[t]);
    }
    index = 0;
    synchDisk->ReadSector(dataSectors[index],(char*)dataset);
    printf("\nFile contents:\n");
    for (i = t = k = 0;i < numSectors;i++,t = (t + 1) % MaxPerSector) 
    {
        if (i > 0 && t == 0)
        {
             index++;
             synchDisk->ReadSector(dataSectors[index],(char*)dataset);
        }
        synchDisk->ReadSector(dataset[t],data);
        for (j = 0; (j < SectorSize) || (k < numBytes); j++, k++) 
        {
	    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
		printf("%c", data[j]);
            else
		printf("\\%x", (unsigned char)data[j]);
	}
        printf("\n"); 
    } 
    delete [] data;
    delete [] dataset;
#else
     int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    for (i = 0; i < numSectors; i++)
        printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
        synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
            if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                printf("%c", data[j]);
            else
                printf("\\%x", (unsigned char)data[j]);
        }
        printf("\n");
    }
    delete [] data;
#endif
}

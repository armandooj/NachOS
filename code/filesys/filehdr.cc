// filehdr.cc
//  Routines for managing the disk file header (in UNIX, this
//  would be called the i-node).
//
//  The file header is used to locate where on disk the
//  file's data is stored.  We implement this as a fixed size
//  table of pointers -- each entry in the table points to the
//  disk sector containing that portion of the file data
//  (in other words, there are no indirect or doubly indirect
//  blocks). The table size is chosen so that the file header
//  will be just big enough to fit in one disk sector,
//
//      Unlike in a real system, we do not keep track of file permissions,
//  ownership, last modification date, etc., in the file header.
//
//  A file header can be initialized in two ways:
//     for a new file, by modifying the in-memory data structure
//       to point to the newly allocated data blocks
//     for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

FileHeader::FileHeader() : numBytes(0), numSectors(0)
{
    memset(dataSectors, 0, sizeof(DataBlockInfo*) * NumDirect);
}

FileHeader::~FileHeader()
{
    int num_data_info_block = divRoundUp(numSectors, NumIndirect);

    // Delete data info block
    for (int i = 0; i < num_data_info_block; i++)
    {
        // Delete indirect block
        delete dataSectors[i]->data;
        delete dataSectors[i];
    }
}

//----------------------------------------------------------------------
// FileHeader::AskForSectors
//  Initialize a fresh file header for a newly created file.
//  Allocate data blocks for the file out of the map of free disk blocks.
//  Return FALSE if there are not enough free blocks to accomodate
//  the new file.
//
//  "freeMap" is the bit map of free disk sectors
//  "fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::AskForSectors(BitMap *freeMap, int addSize)
{
    int needed_new_sector = divRoundUp(numBytes + addSize, SectorSize) - numSectors;

    if (freeMap->NumClear() < needed_new_sector)
        return FALSE;       // not enough space

    // Max file size
    if (divRoundUp(numBytes + addSize, SectorSize) > (int)MAX_FILE_SECTORS)
        return FALSE;

    numBytes = numBytes + addSize;
    numSectors = divRoundUp(numBytes, SectorSize);

    int sector;

    DEBUG('f', "Need %d -- %d sector\n", needed_new_sector, NumIndirect);
    for (int i = numSectors - needed_new_sector; i < numSectors; i++)
    {
        // New indirect block
        if (i % NumIndirect == 0)
        {
            // Initialize structure for data block
            dataSectors[i / NumIndirect] = new DataBlockInfo;
            dataSectors[i / NumIndirect]->data = new DataBlockHdr;
            memset(dataSectors[i / NumIndirect]->data, 0, sizeof(DataBlockHdr));

            // Find sector for data block
            sector = freeMap->Find();
            DEBUG('f', "Allocate new data block at sector %d\n", sector);

            dataSectors[i / NumIndirect]->sector = sector;
        }

        // Write to indirect block
        dataSectors[i / NumIndirect]->data->dataSectors[i % NumIndirect] = freeMap->Find();
        DEBUG('f', "Allocate new raw at sector %d\n", dataSectors[i / NumIndirect]->data->dataSectors[i % NumIndirect]);

    }

    return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
//  De-allocate all the space allocated for data blocks for this file.
//
//  "freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void
FileHeader::Deallocate(BitMap *freeMap)
{
    int num_data_info_block = divRoundUp(numSectors, NumIndirect);

    for (int i = 0; i < numSectors; i++)
    {
        // Delete data block
        ASSERT(freeMap->Test(dataSectors[i / NumIndirect]->data->dataSectors[i % NumIndirect]));
        freeMap->Clear(dataSectors[i / NumIndirect]->data->dataSectors[i % NumIndirect]);
    }

    // Delete data info block
    for (int i = 0; i < num_data_info_block; i++)
    {
        // Delete indirect block
        ASSERT(freeMap->Test(dataSectors[i]->sector));
        freeMap->Clear(dataSectors[i]->sector);
    }

}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
//  Fetch contents of file header from disk.
//
//  "sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    // Fill this with data inside sector
    synchDisk->ReadSector(sector, (char *)this);

    int num_data_info_block = divRoundUp(numSectors, NumIndirect);

    for (int i = 0; i < num_data_info_block; i++)
    {
        // Retrieve sector
        sector = (int)dataSectors[i];
        dataSectors[i] = new DataBlockInfo;
        dataSectors[i]->sector = sector;
        dataSectors[i]->data = new DataBlockHdr;

        // Fetch DataBlockHdr
        synchDisk->ReadSector(sector, (char *)(dataSectors[i]->data));
    }
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
//  Write the modified contents of the file header back to disk.
//
//  "sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    int num_data_info_block = divRoundUp(numSectors, NumIndirect);
    int data_sector;

    FileHeader temp;
    temp.numBytes = numBytes;
    temp.numSectors = numSectors;

    for (int i = 0; i < num_data_info_block; i++)
    {
        // Write data->data inside sector data->sector
        data_sector = dataSectors[i]->sector;
        DEBUG('f', "Write data at sector %d\n", data_sector);
        synchDisk->WriteSector(data_sector, (char *)(dataSectors[i]->data));

        // Update this->dataSectors with dataSections->sector
        temp.dataSectors[i] = (DataBlockInfo *)data_sector;
    }

    // Write this inside sector *sector*
    synchDisk->WriteSector(sector, (char *)(&temp));

    // To avoid deallocate sectors
    temp.numBytes = 0;
    temp.numSectors = 0;
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
//  Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//  offset in the file) to a physical address (the sector where the
//  data at the offset is stored).
//
//  "offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    int sector = offset / SectorSize;
    int data_info_sector = sector / NumIndirect;
    int data_padding_index = sector % NumIndirect;

    DataBlockInfo *info = dataSectors[data_info_sector];
    DataBlockHdr *hdr = info->data;

    return hdr->dataSectors[data_padding_index];
}

//----------------------------------------------------------------------
// FileHeader::FileLength
//  Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
//  Print the contents of the file header, and the contents of all
//  the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);

    for (i = 0; i < divRoundUp(numSectors, (int)NumIndirect); i++)
    {
        printf("%d ", dataSectors[i]->sector);
        printf("(");
        for (j = i * NumIndirect; j < numSectors && j < (i + 1) * (int)NumIndirect; j++)
        {
            printf(" %d", dataSectors[i]->data->dataSectors[j % (int)NumIndirect]);
        }
        printf(" )");
    }

    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++)
    {
        synchDisk->ReadSector(ByteToSector(i * SectorSize), data);

        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++)
        {
            if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                printf("%c", data[j]);
            else
                printf("\\%x", (unsigned char)data[j]);
        }
        printf("\n");
    }
    delete [] data;

}

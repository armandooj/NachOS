// openfile.cc
//	Routines to manage an open Nachos file.  As in UNIX, a
//	file must be open before we can read or write to it.
//	Once we're all done, we can close it (in Nachos, by deleting
//	the OpenFile data structure).
//
//	Also as in UNIX, for convenience, we keep the file header in
//	memory while the file is open.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "filehdr.h"
#include "filesys.h"
#include "openfile.h"
#include "system.h"

#include <strings.h> /* for bzero */

static inline void TakeReaderLock(FileHdrSync* sync)
{
    sync->mutex->P();
    sync->readcount++;
    if (sync->readcount == 1)
    {
        sync->writer->P();
    }
    sync->mutex->V();
}

static inline void GiveReaderLock(FileHdrSync* sync)
{
    sync->mutex->P();
    sync->readcount--;
    if (sync->readcount == 0)
    {
        sync->writer->V();
    }
    sync->mutex->V();
}

#ifndef FILESYS_STUB
//----------------------------------------------------------------------
// OpenFile::OpenFile
//  Open a Nachos file for reading and writing.  Bring the file header
//	into memory while the file is open.
//
//	"sector" -- the location on disk of the file header for this file
//----------------------------------------------------------------------

OpenFile::OpenFile(int sector, const char* filename)
{
    hdr = new FileHeader;
    hdr->FetchFrom(sector);
    seekPosition = 0;
    fileSector = sector;
    if (filename == NULL)
    {
        name = NULL;
    }
    else
    {
        name = new char[strlen(filename) + 1];
        strcpy(name, filename);
    }

    sync = fileSyncMgr->GetFileSyncForFile(sector);
}

//----------------------------------------------------------------------
// OpenFile::~OpenFile
//  Close a Nachos file, de-allocating any in-memory data structures.
//----------------------------------------------------------------------

OpenFile::~OpenFile()
{
    if (currentThread != NULL)
        sync->writer->P();

    if (name != NULL)
    {
        fileSyncMgr->DeleteOpenedFile(name);
        delete [] name;
    }
    delete hdr;

    if (currentThread != NULL)
        sync->writer->V();

    // Detach sync structure
    fileSyncMgr->DetachFileSyncForFile(fileSector);
}

//----------------------------------------------------------------------
// OpenFile::Seek
//  Change the current location within the open file -- the point at
//	which the next Read or Write will start from.
//
//	"position" -- the location within the file for the next Read/Write
//----------------------------------------------------------------------

void
OpenFile::Seek(int position)
{
    seekPosition = position;
}

//----------------------------------------------------------------------
// OpenFile::Read/Write
//  Read/write a portion of a file, starting from seekPosition.
//	Return the number of bytes actually written or read, and as a
//	side effect, increment the current position within the file.
//
//	Implemented using the more primitive ReadAt/WriteAt.
//
//	"into" -- the buffer to contain the data to be read from disk
//	"from" -- the buffer containing the data to be written to disk
//	"numBytes" -- the number of bytes to transfer
//----------------------------------------------------------------------
int
OpenFile::Read(char *into, int numBytes)
{
    // R/W lock
    TakeReaderLock(sync);

    int result = ReadAt(into, numBytes, seekPosition, false);
    seekPosition += result;

    GiveReaderLock(sync);

    return result;
}

int
OpenFile::Write(const char *into, int numBytes)
{
    sync->writer->P();
    int result = WriteAt(into, numBytes, seekPosition, true, false);
    seekPosition += result;
    sync->writer->V();
    return result;
}

int
OpenFile::ReadVirtual(int virtualAddr, int numBytes)
{
    TakeReaderLock(sync);
    int result = ReadAtVirtual(virtualAddr, numBytes, seekPosition, false);
    seekPosition += result;
    GiveReaderLock(sync);
    return result;
}

int
OpenFile::WriteVirtual(int virtualAddr, int numBytes)
{
    sync->writer->P();
    int result = WriteAtVirtual(virtualAddr, numBytes, seekPosition, false);
    seekPosition += result;
    sync->writer->V();
    return result;
}

//----------------------------------------------------------------------
// OpenFile::ReadAt/WriteAt
//  Read/write a portion of a file, starting at "position".
//	Return the number of bytes actually written or read, but has
//	no side effects (except that Write modifies the file, of course).
//
//	There is no guarantee the request starts or ends on an even disk sector
//	boundary; however the disk only knows how to read/write a whole disk
//	sector at a time.  Thus:
//
//	For ReadAt:
//     We read in all of the full or partial sectors that are part of the
//     request, but we only copy the part we are interested in.
//	For WriteAt:
//     We must first read in any sectors that will be partially written,
//     so that we don't overwrite the unmodified portion.  We then copy
//     in the data that will be modified, and write back all the full
//     or partial sectors that are part of the request.
//
//	"into" -- the buffer to contain the data to be read from disk
//	"from" -- the buffer containing the data to be written to disk
//	"numBytes" -- the number of bytes to transfer
//	"position" -- the offset within the file of the first byte to be
//			read/written
//----------------------------------------------------------------------

int
OpenFile::ReadAt(char *into, int numBytes, int position, bool takeLock)
{
    // R/W lock
    if (takeLock)
        TakeReaderLock(sync);

    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
    {
        if (takeLock)
            GiveReaderLock(sync);

        return 0;               // check request
    }

    if ((position + numBytes) > fileLength)
        numBytes = fileLength - position;
    DEBUG('f', "Reading %d bytes at %d, from file of length %d.\n",
          numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // read in all the full and partial sectors that we need
    buf = new char[numSectors * SectorSize];
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->ReadSector(hdr->ByteToSector(i * SectorSize),
                              &buf[(i - firstSector) * SectorSize]);

    // copy the part we want
    memcpy(into, &buf[position - (firstSector * SectorSize)], numBytes);
    delete [] buf;

    // Release r/w lock
    if (takeLock)
        GiveReaderLock(sync);

    return numBytes;
}

int
OpenFile::WriteAt(const char *from, int numBytes, int position, bool allow_dynamic_space, bool takeLock)
{
    if (takeLock)
        sync->writer->P();

    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    bool firstAligned, lastAligned;
    char *buf;

    if (numBytes <= 0)
    {
        if (takeLock)
            sync->writer->V();
        return 0;				// check request
    }

    DEBUG('f', "Writing %d bytes at %d, from file of length %d.\n",
          numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // Compute total size of files
    int total_size = position + numBytes;

    // Ask for new sector if needed
    if (allow_dynamic_space)
    {
        BitMap *freeMap;
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(fileSystem->GetFreeMapFile());

        if (!hdr->AskForSectors(freeMap, total_size - hdr->FileLength()))
        {
            freeMap->WriteBack(fileSystem->GetFreeMapFile());
            if (takeLock)
                sync->writer->V();
            delete freeMap;
            return -1;
        }

        hdr->WriteBack(fileSector);
        freeMap->WriteBack(fileSystem->GetFreeMapFile());

        delete freeMap;
    }

    buf = new char[numSectors * SectorSize];
    bzero(buf, numSectors * SectorSize);

    firstAligned = (position == (firstSector * SectorSize));
    lastAligned = ((position + numBytes) == ((lastSector + 1) * SectorSize));

// read in first and last sector, if they are to be partially modified
    if (!firstAligned)
        ReadAt(buf, SectorSize, firstSector * SectorSize, false);
    if (!lastAligned && ((firstSector != lastSector) || firstAligned))
        ReadAt(&buf[(lastSector - firstSector) * SectorSize],
               SectorSize, lastSector * SectorSize, false);

// copy in the bytes we want to change
    memcpy(&buf[position - (firstSector * SectorSize)], from, numBytes);

// write modified sectors back
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->WriteSector(hdr->ByteToSector(i * SectorSize),
                               &buf[(i - firstSector) * SectorSize]);
    delete [] buf;
    if (takeLock)
        sync->writer->V();
    return numBytes;
}

//----------------------------------------------------------------------
// OpenFile::Length
//  Return the number of bytes in the file.
//----------------------------------------------------------------------

int
OpenFile::Length()
{
    return hdr->FileLength();
}
#endif


int OpenFile::ReadAtVirtual(int virtualAddr, int numBytes, int position, bool takeLock)
{
    if (takeLock)
        TakeReaderLock(sync);

    char *buffer = new char[numBytes];
    int ret = 0;
    int i = 0;

    ret = ReadAt(buffer, numBytes, position, false);

    // Error case, handle
    if (ret < 0)
    {
        DEBUG('a', "Error %d with readAt\n", ret);
        if (takeLock)
            GiveReaderLock(sync);
        delete [] buffer;
        return ret;
    }

    // Write buffer using WriteMem (ret : num of bytes read)
    for (i = 0; i < ret; i++)
    {
        DEBUG('f', "Writing at @ %d\n", virtualAddr + i);
        if (!machine->WriteMem(virtualAddr + i, sizeof(char), buffer[i]))
        {
            DEBUG('a', "Error %d with WriteMem\n", ret);
            if (takeLock)
                GiveReaderLock(sync);
            delete [] buffer;
            return -1;
        }
    }

    DEBUG('a', "Written %d bytes\n", ret);

    delete [] buffer;

    if (takeLock)
        GiveReaderLock(sync);
    return ret;
}

int OpenFile::WriteAtVirtual(int virtualAddr, int numBytes, int position, bool takeLock)
{
    if (takeLock)
        sync->writer->P();

    char *c = new char[numBytes + 1];
    int really_write = copyStringFromMachine(virtualAddr, c, numBytes);
    c[really_write] = '\0';

    int res = WriteAt(c, numBytes, position, true, false);
    delete [] c;

    if (takeLock)
        sync->writer->P();

    return res;
}

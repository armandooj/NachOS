// openfile.h
//	Data structures for opening, closing, reading and writing to
//	individual files.  The operations supported are similar to
//	the UNIX ones -- type 'man open' to the UNIX prompt.
//
//	There are two implementations.  One is a "STUB" that directly
//	turns the file operations into the underlying UNIX operations.
//	(cf. comment in filesys.h).
//
//	The other is the "real" implementation, that turns these
//	operations into read and write disk sector requests.
//	In this baseline implementation of the file system, we don't
//	worry about concurrent accesses to the file system
//	by different threads -- this is part of the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"
#include "syncmgr.h"

#ifdef FILESYS_STUB			// Temporarily implement calls to
// Nachos file system as calls to UNIX!
// See definitions listed under #else
class OpenFile {
public:
    OpenFile(int f, const char *name) { file = f; currentOffset = 0; }	// open the file
    ~OpenFile() { Close(file); }			// close the file

    int ReadAt(char *into, int numBytes, int position, bool unused = false) {
        Lseek(file, position, 0);
        return ReadPartial(file, into, numBytes);
    }
    int WriteAt(const char *from, int numBytes, int position, bool unused = false, bool unused2 = false) {
        Lseek(file, position, 0);
        WriteFile(file, from, numBytes);
        return numBytes;
    }
    int Read(char *into, int numBytes) {
        int numRead = ReadAt(into, numBytes, currentOffset);
        currentOffset += numRead;
        return numRead;
    }
    int Write(const char *from, int numBytes) {
        int numWritten = WriteAt(from, numBytes, currentOffset);
        currentOffset += numWritten;
        return numWritten;
    }

    void Seek(int position) { Lseek(file, position, 0); }
    int Length() { Lseek(file, 0, 2); return Tell(file); }
    // Read handling virtual memory
    int ReadAtVirtual(int virtualAddr, int numBytes, int position, bool unused = false);
    int WriteAtVirtual(int virtualAddr, int numBytes, int position, bool unused = false);
    int ReadVirtual(int virtualAddr, int numBytes) { return 0; };
    int WriteVirtual(int virtualAddr, int numBytes) { return 0; };
private:
    int file;
    int currentOffset;
    FileHdrSync *sync;
};

#else // FILESYS
class FileHeader;

class OpenFile {
public:
    OpenFile(int sector, const char* name);		// Open a file whose header is located
    // at "sector" on the disk
    ~OpenFile();			// Close the file

    void Seek(int position);        // Set the position from which to
    // start reading/writing -- UNIX lseek

    int Read(char *into, int numBytes); // Read/write bytes from the file,

    // starting at the implicit position.
    // Return the # actually read/written,
    // and increment position in file.
    int Write(const char *from, int numBytes);

    int ReadAt(char *into, int numBytes, int position, bool takeLock = true);
    // Read/write bytes from the file,
    // bypassing the implicit position.
    int WriteAt(const char *from, int numBytes, int position, bool allow_dynamic_space = false, bool takeLock = true);

    int Length();           // Return the number of bytes in the
    // file (this interface is simpler
    // than the UNIX idiom -- lseek to
    // end of file, tell, lseek back

    // Read handling virtual memory
    int ReadAtVirtual(int virtualAddr, int numBytes, int position, bool takelock = true);
    int WriteAtVirtual(int virtualAddr, int numBytes, int position, bool takeLock = true);
    int ReadVirtual(int virtualAddr, int numBytes);
    int WriteVirtual(int virtualAddr, int numBytes);

private:
    FileHeader *hdr;			// Header for this file
    int seekPosition;			// Current position within the file
    int fileSector;                 // Keep sector for file resize
    FileHdrSync *sync;              // Sync structure for access
    char *name;
};

#endif // FILESYS

#endif // OPENFILE_H

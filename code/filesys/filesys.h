// filesys.h
//  Data structures to represent the Nachos file system.
//
//  A file system is a set of files stored on disk, organized
//  into directories.  Operations on the file system have to
//  do with "naming" -- creating, opening, and deleting files,
//  given a textual file name.  Operations on an individual
//  "open" file (read, write, close) are to be found in the OpenFile
//  class (openfile.h).
//
//  We define two separate implementations of the file system.
//  The "STUB" version just re-defines the Nachos file system
//  operations as operations on the native UNIX file system on the machine
//  running the Nachos simulation.  This is provided in case the
//  multiprogramming and virtual memory assignments (which make use
//  of the file system) are done before the file system assignment.
//
//  The other version is a "real" file system, built on top of
//  a disk simulator.  The disk is simulated using the native UNIX
//  file system (in a file named "DISK").
//
//  In the "real" implementation, there are two key data structures used
//  in the file system.  There is a single "root" directory, listing
//  all of the files in the file system; unlike UNIX, the baseline
//  system does not provide a hierarchical directory structure.
//  In addition, there is a bitmap for allocating
//  disk sectors.  Both the root directory and the bitmap are themselves
//  stored as files in the Nachos file system -- this causes an interesting
//  bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "syncmgr.h"
#include "openfile.h"

class Directory;


#ifdef FILESYS_STUB         // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem {
public:
    FileSystem(bool format) {}

    bool Create(const char *name, int initialSize) {
        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1) return FALSE;
        Close(fileDescriptor);
        return TRUE;
    }

    OpenFile* Open(const char *name) {
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        if (fileDescriptor == -1) return NULL;
        return new OpenFile(fileDescriptor, NULL);
    }

    bool Remove(const char *name) { return Unlink(name) == 0; }
    Directory *GetDirectoryByName(const char* dirname, int *store_sector){ return NULL; }
    char *ExpandFileName(const char* filename) { return NULL; }
};

#else // FILESYS
class FileSystem {
public:
    FileSystem(bool format);    // Initialize the file system.
    ~FileSystem();
    // Must be called *after* "synchDisk"
    // has been initialized.
    // If "format", there is nothing on
    // the disk, so initialize the directory
    // and the bitmap of free blocks.

    bool Create(const char *name, int initialSize);
    // Create a file (UNIX creat)

    OpenFile* Open(const char *name);   // Open a file (UNIX open)

    int Remove(const char *name);  // Delete a file (UNIX unlink)

    void List(const char* name);      // List all the files in the file system

    void Print();     // List all the files and their contents

    char *ExpandFileName(const char* filename); /* Get absolute path */
    char *DirectoryName(const char* filename);  /* Get directory name */
    char *FileName(const char* filename);       /* Get file name */
    Directory *GetDirectoryByName(const char* dirname, int *store_sector); /* Get directory by name */
    bool CheckNameLimitation(const char* name);         /* Check if the name is valid */
    int CreateDirectory(const char *dirname);           /* Create a directory (any path) */
    bool RemoveDirectory(const char *name);             /* Remove a directory */
    void ListRec(const char *name);             /* List recursivly a directory */
    OpenFile* GetFreeMapFile() { return freeMapFile; }
private:
    OpenFile* freeMapFile;    // Bit map of free disk blocks,
    // represented as a file
    OpenFile* directoryFile;    // "Root" directory -- list of
    // file names, represented as a file
};


// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known
// sectors, so that they can be located on boot-up.
#define FreeMapSector       0
#define DirectorySector     1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number
// of files that can be loaded onto the disk.
#define FreeMapFileSize     (NumSectors / BitsInByte)
#define NumDirEntries       10
#define DirectoryFileSize   (sizeof(DirectoryEntry) * NumDirEntries)

#endif // FILESYS

#endif // FS_H

// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "../userprog/bitmap.h"

// number of direct index for each fileheader(inode) increase from 3kb to 120kb
#define NumDirect   ((SectorSize - 2 * sizeof(int)) / sizeof(int)) 

#ifdef CHANGED

#define MaxPerSector    ((SectorSize) / sizeof(int))
#define MaxSector       (NumDirect * MaxPerSector)
// max size per file = number of direct index per fileheader(inode) * sectorsize
#define MaxFileSize     (NumDirect * SectorSize * MaxPerSector)

#else

#define MaxFileSize     (NumDirect * SectorSize)

#endif


// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:
   #ifdef CHANGED

    enum FileType {
        FILE,
        DIRECTORY,
        DOTLINK
    };

    FileHeader();

    ~FileHeader();
    #endif
    bool Allocate(BitMap *bitMap, int fileSize);// Initialize a file header, 
						//  including allocating space 
						//  on disk for the file data
 #ifdef CHANGED
    void Deallocate(BitMap *bitMap, int reservebytes);          // De-allocate this file's 
                        //  data blocks
#else
    void Deallocate(BitMap *bitMap);
#endif

						//  data blocks

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk

    int ByteToSector(int offset);	// Convert a byte offset into the file
					// to the disk sector containing
					// the byte

    int FileLength();			// Return the length of the file 
					// in bytes

    void Print();			// Print the contents of the file.

#ifdef CHANGED
    void Type_Set(FileType t);
    FileType Type_Get();
    void LinkSector_Set(int sector);
    int LinkSector_Get();
    FileType type;
 #endif
    
  private:
    int numBytes;			// Number of bytes in the file
    int numSectors;			// Number of data sectors in the file
    int dataSectors[NumDirect];		// Disk sector numbers for each data 
					// block in the file
};

#endif // FILEHDR_H

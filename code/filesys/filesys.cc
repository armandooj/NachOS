// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"
#include "system.h"
#include <list>
#include <string>
#include <libgen.h>

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)

//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{ 
    DEBUG('f', "Initializing the file system.\n");
    if (format) {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
	FileHeader *mapHdr = new FileHeader;
	FileHeader *dirHdr = new FileHeader;

        DEBUG('f', "Formatting the file system.\n");

    // First, allocate space for FileHeaders for the directory and bitmap
    // (make sure no one else grabs these!)
	freeMap->Mark(FreeMapSector);	    
	freeMap->Mark(DirectorySector);

    // Second, allocate space for the data blocks containing the contents
    // of the directory and bitmap files.  There better be enough space!

	ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
	ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

    // Flush the bitmap and directory FileHeaders back to disk
    // We need to do this before we can "Open" the file, since open
    // reads the file header off of disk (and currently the disk has garbage
    // on it!).

        DEBUG('f', "Writing headers back to disk.\n");
	mapHdr->WriteBack(FreeMapSector);    
	dirHdr->WriteBack(DirectorySector);

    // OK to open the bitmap and directory files now
    // The file system operations assume these two files are left open
    // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
     
    // Once we have the files "open", we can write the initial version
    // of each file back to disk.  The directory at this point is completely
    // empty; but the bitmap has been changed to reflect the fact that
    // sectors on the disk have been allocated for the file headers and
    // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
	freeMap->WriteBack(freeMapFile);	 // flush changes to disk
	directory->WriteBack(directoryFile);

	if (DebugIsEnabled('f')) {
	    freeMap->Print();
	    directory->Print();

    delete freeMap; 
	delete directory; 
	delete mapHdr; 
	delete dirHdr;
	}
    } else {
    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
    }
}

FileSystem::~FileSystem()
{
    delete freeMapFile;
    delete directoryFile;
}


//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

bool
FileSystem::Create(const char *name, int initialSize)
{
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

    // Extract file name
    char *extractname = ExtractFileName(name);
    char *pDir = DirectoryName(extractname);
    char *fp = FileName(extractname);

    // Get the parent directory
    int psector;

    Directory *directory = GetDirectoryByName(pDir, &psector);

    if(directory==NULL)
        success = FALSE;
    else if (directory->Find(fp) != -1)
      success = FALSE;			// file is already in directory
    else {	
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);

        sector = freeMap->Find();	// find a sector to hold the file header
    	
        if (sector == -1) 		
            success = FALSE;		// no free block for file header 
        else if (!directory->Add(name, sector))
            success = FALSE;	// no space in directory
	else {
    	    hdr = new FileHeader;
	    if (!hdr->Allocate(freeMap, initialSize))
            	success = FALSE;	// no space on disk for data
	    else {	
	    	success = TRUE;
		// everthing worked, flush all changes back to disk

                        // everthing worked, flush all changes back to disk
            OpenFile *f = new OpenFile(psector, NULL);

    	    	hdr->WriteBack(sector); 	   // Write File header on disk	
    	    	directory->WriteBack(f);  // Write directory structure on disk
    	    	freeMap->WriteBack(freeMapFile);   // Write free sectors on disk
                delete f;
	    }
            delete hdr;
	}
        delete freeMap;
    }
    delete directory;
    delete [] extractname;
    delete [] pDir;    
    delete [] fp;

    return success;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------


FileSystem::Open(const char *name)
{ 
    fileSyncMgr->NewOpenedFile(name);

    DEBUG('f', "Opening file %s\n", name);
    // Expand file name
    char *extractname = ExtractFileName(name);
    char *pDir = DirectoryName(expandname);
    char *fp = FileName(expandname);

    fileSyncMgr->DeleteOpenedFile(name);
    fileSyncMgr->NewOpenedFile(extractname);

    // Get the parent directory
    int psector;

    Directory *directory = GetDirectoryByName(pDir, &psector);
    OpenFile *openFile = NULL;
    int sector;    
    
    if (directory == NULL)
    {
        openFile = NULL;
    }
    else
    {
        sector = directory->Find(fp);  // Seek file on system

        if (sector >= 0)                                        // Filename was found in directory
            openFile = new OpenFile(sector, extractname);

        if (openFile == NULL)
            fileSyncMgr->DeleteOpenedFile(extractname);

        delete directory;
    }

    delete [] pDir;
    delete [] extractname;
    delete [] fp;

    return openFile;                // return NULL if not found
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool
FileSystem::Remove(const char *name)
{ 
    
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;
    
    // Expand file name
    char *extractname = ExtractFileName(name);
    char *pDir = DirectoryName(extractname);
    char *fp = FileName(extractname);

    // Get the parent directory
    int psector;

    // Try to open parent directory
    Directory *directory = GetDirectoryByName(pDir, &psector);
    
    // Check if file is not already opened
    if (fileSyncMgr->IsOpenedFile(expandname))
    {
        delete [] extractname;
        delete [] pDir;
        delete [] fp;
        if (directory)
            delete directory;
        return -1;
    }

    // If not found
    if (directory == NULL)
    {
        delete [] extractname;
        delete [] pDir;
        delete [] fp;
        return -2;
    }

    // Search for filename
    sector = directory->Find(fp);
    if (sector == -1) {
        delete [] extractname;
        delete [] pDir;
        delete [] fp;
        delete directory;
        return -2;
    }

    // Read file from Disk
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    // Get free sectors BitMap
    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    fileHdr->Deallocate(freeMap);  		// remove data blocks
    freeMap->Clear(sector);			// remove header block
    directory->Remove(name);

    freeMap->WriteBack(freeMapFile);		// flush to disk

    // Write directory modifications to disk
    OpenFile *f = new OpenFile(parent_sector, NULL);
    directory->WriteBack(f);
    delete f;

    delete fileHdr;
    delete directory;
    delete freeMap;
    delete [] extractname;
    delete [] pDir;
    delete [] fp;
    return TRUE;
} 

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List(const char *dirname)
{
    char *extractname = ExtractFileName(dirname);
    Directory *parent = GetDirectoryByName(extractname, NULL);
    delete [] extractname;

    if (parent == NULL)
        return;

    parent->List();
    delete parent;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()

    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();

    directory->FetchFrom(directoryFile);
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
} 


            /* Create a Directory     */
int FileSystem::CreateDirectory(const char* dirname)
{
    // Extract file name
    char *extractname = ExtractFileName(dirname);
    char *pDir = DirectoryName(expandname);

    BitMap *freeMap = new BitMap(NumSectors);

    // Get the parent directory
    int psector;

    Directory *parent = GetDirectoryByName(pDir, &psector);

    int er=0;

    if(parent==NULL)
        er=-1;
    else
    {
        freeMap->FetchFrom(freeMapFile);
        int sector = freeMap->Find();
        if(sector==-1)
        {
            er=-2;
        }
        else
        {
            freeMap->Mark(sector); // Mark sector

            // Allocate space for the installation of the directory
            Directory *child_dir = new Directory(NumDirEntries);
            FileHeader *dirHdr = new FileHeader;

            dirHdr->WriteBack(sector);

            // Write Directory
            OpenFile *f = new OpenFile(sector,NULL);
            child_dir->WriteBack(f);
 
            // Changed FreeMap
            freeMap->WriteBack(freeMapFile);

            delete f;
            delete child_dir;
            delete dirHdr;
            er=0;
        }
    }

    if(er==0)
    {
        OpenFile *f = new OpenFile(psector,NULL);
        parent->WriteBack(f);
        delete f;
    }

    if(parent!=NULL)
        delete parent;

    delete [] extractname;
    delete [] pDir;
    delete freeMap;

    return er;
}

/**
 * Convert relative filename to absolute
 *
 * Return a dynamicly allocated name (should be freed)
 **/
char *FileSystem::ExtractFileName(const char* filename)
{
    // Take into account current directory
    std::string filename_s = filename;

    // If relative path
    if (filename_s[0] != '/')
    {
        filename_s = currentThread->GetCurrentDirectory() + filename_s;
    }

    char *cpy = new char[strlen(filename_s.c_str()) + 1];
    char *saveptr = cpy;

    strcpy(cpy, filename_s.c_str());

    if(strcmp(filename_s.c_str(), "..") == 0)
        return cpy;

    char *name = strtok(cpy, "/");
    std::list<std::string> final;

    while (name != NULL)
    {
        if (strcmp(name, "..") == 0 && !final.empty())
        {
            final.pop_back();
        }
        else if (strcmp(name, ".") != 0 && strcmp(name, "") != 0)
        {
            final.push_back(std::string(name));
        }
        // else it's . or //, no change

        name = strtok(NULL, "/");
    }

    // Construct string
    std::string final_string;
    std::list<std::string>::iterator it;

    for (it = final.begin(); it != final.end(); it++)
    {
        final_string += "/";
        final_string += *it;
    }

    delete [] saveptr;

    if (final_string.empty())
        final_string = "/";

    // Convert to char*
    char *res = new char[final_string.size() + 1];
    strcpy(res, final_string.c_str());

    return res;
}

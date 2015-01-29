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
#include "openfile.h"

#ifdef CHANGED
#include <string>
#include <iostream>

#include "system.h"
#include <libgen.h>
#include <list>
#include <string>
#endif



#ifndef CHANGED


// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1
#endif
// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		1000
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
    #ifdef CHANGED
        dirHdr->Type_Set(FileHeader::DIRECTORY);         //type = DIRECTORY ;
    #endif 
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

#ifdef CHANGED
        // Create a symbolic link to the current folder
    Create(".", FileHeader::DOTLINK);
    Create("..", FileHeader::DOTLINK);
    directory->FetchFrom(directoryFile);

    
    /*---------------------------------------------*/
    int dirSector = directory->Find(".");

    FileHeader* dirfileheader = new FileHeader;

    dirfileheader->FetchFrom(dirSector);   //Read the contents of the dirsector from disk

    dirfileheader->LinkSector_Set(DirectorySector);  //return dataSectors[0];

    dirfileheader->WriteBack(dirSector);    // Write modifications to the dirSector back to disk

    delete dirfileheader;
    /*---------------------------------------------*/


    /*---------------------------------------------*/
    int pSector = directory->Find("..");

    FileHeader* pfileheader = new FileHeader;

    pfileheader->FetchFrom(pSector);             //Read the contents of the psector from disk

    pfileheader->LinkSector_Set(DirectorySector);   //dataSectors[0] = sectorParent;

    pfileheader->WriteBack(pSector);            // Write modifications to the pSector back to disk

    delete pfileheader;
    /*---------------------------------------------*/


#endif

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

#ifndef CHANGED
bool
FileSystem::Create(const char *name, int initialSize) {
#else
bool FileSystem::Create(const char *name, FileHeader::FileType type) {
#endif

    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;
    int index [1];

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);

    if (directory->Find(name) != -1)
      success = FALSE;			// file is already in directory
    else {	
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
    	if (sector == -1) 		
            success = FALSE;		// no free block for file header 
        else if (!directory->Add(name, sector,index))  // Here we return the address just that in case of a directory we change the table[index] to directory
        
            success = FALSE;	// no space in directory

      	else {
    	    hdr = new FileHeader;
//    	    printf("Index is %d \n ",index[0]);
#ifndef CHANGED

	    if (!hdr->Allocate(freeMap, initialSize)) // for create file
#else
	    	directory->IsDirectory(index[0]); // needs checking
            if (!hdr->Allocate(freeMap, 0)) // Directory

#endif
            	success = FALSE;	// no space on disk for data
	    else {	
	    	success = TRUE;
		// everthing worked, flush all changes back to disk
        #ifdef CHANGED
                hdr->Type_Set(type);   //type = type; // for Directory
        #endif

    	    	hdr->WriteBack(sector); 		
    	    	directory->WriteBack(directoryFile);
    	    	freeMap->WriteBack(freeMapFile);
	    }
            delete hdr;
	}
        delete freeMap;
    }
    delete directory;
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

OpenFile *
FileSystem::Open(const char *name)
{ 
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name);
    directory->FetchFrom(directoryFile);
    sector = directory->Find(name);

    if (sector >= 0) {
	   openFile = new OpenFile(sector);	// name was found in directory 
    }
    delete directory;
    return openFile;				// return NULL if not found
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
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;
    
    directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);
    sector = directory->Find(name);
    if (sector == -1) {
       delete directory;
       return FALSE;			 // file not found 
    }
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);
#ifdef CHANGED
    fileHdr->Deallocate(freeMap,0);  		// remove data blocks
    #else
    fileHdr->Deallocate(freeMap);
    #endif

    freeMap->Clear(sector);			// remove header block
    directory->Remove(name);

    freeMap->WriteBack(freeMapFile);		// flush to disk
    directory->WriteBack(directoryFile);        // flush to disk
    delete fileHdr;
    delete directory;
    delete freeMap;
    return TRUE;
} 

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    Directory *directory = new Directory(NumDirEntries);

    directory->FetchFrom(directoryFile);
    directory->List();
    delete directory;
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
{
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


#ifdef CHANGED


/*The following function sets the path of the current folder  */
bool FileSystem::Directory_path(const char* name) {
    char* slash = NULL;
    char* dir = (char*) name;

    OpenFile* fp = directoryFile;

    if (name[0] == '/') {
        directoryFile = new OpenFile(DirectorySector);  // Opend the file from the sector no. 1 which  is a directory sector.
        dir++;              //and hence we increase the total directories
    }

/*strchr()-- returns a pointer to the first occurrence of character in the C string str.
More info. is available here: http://www.cplusplus.com/reference/cstring/strchr/

char * strncpy ( char * destination, const char * source, size_t num ); 
Copies the first num characters of source to destination. 

*/ 
   while ((slash = strchr(dir, '/')) != NULL) 
    {
        char dirName[FileNameMaxLen + 1];       //FileNameMaxLen =9, which is defined in directory.cc
        int n = slash - dir;
        strncpy(dirName, dir, n);
        dirName[n] = '\0';
        

        if (strcmp(dirName, "..") == 0) 
        {
            Directory* d = new Directory(NumDirEntries);
            d->FetchFrom(directoryFile);// Read the contents from directoryFile
            int pSector = d->Find("..");  //Find for ".." and returns its sector  no.
            delete d;

            FileHeader* dirfileheader = new FileHeader;
            dirfileheader->FetchFrom(pSector);      // read the contents from psector
            int parentSector = dirfileheader->LinkSector_Get(); //return dataSectors[0];
            delete dirfileheader;

            if (directoryFile != fp) {                //  if not available then we are deleting directoryfile.
                delete directoryFile;
            }
            directoryFile = new OpenFile(parentSector);
        } 
        else 
        {
            Directory *d = new Directory(NumDirEntries);
            d->FetchFrom(directoryFile);            // read the contents from directoryfile
            int sector = d->Find(dirName);          // Look for the sector of file 'dirname'
            delete d;

            if (sector == -1) {
                if (directoryFile != fp) {
                    delete directoryFile;
                }
                directoryFile = fp;
                return false;
            }

            FileHeader* fileheader = new FileHeader();
            fileheader->FetchFrom(sector);
            FileHeader::FileType type = fileheader->Type_Get(); // return type;
            delete fileheader;
            if (type == FileHeader::DIRECTORY) {
                OpenFile* fp2 = directoryFile;
                directoryFile = Open(dirName);
                if (fp2 != fp) {
                    delete fp2;
                }
                if (directoryFile == NULL) {
                    if (directoryFile != fp) {
                        delete directoryFile;
                    }
                    directoryFile = fp;
                    return false;
                }
            } 
            else 
            {
                if (directoryFile != fp) {
                    delete directoryFile;
                }
                directoryFile = fp;
                return false;
            }
        }
        dir = slash + 1;
    }

    if (directoryFile != fp) {
        delete fp;
    }
    return true;
}


bool FileSystem::CreateDirectory(const char *name) {

    // Create the new folder and writes a data structure to Directory
    Directory *directory;

    Create(name, FileHeader::DIRECTORY);

    directory = new Directory(NumDirEntries);

    OpenFile* newDirectory = Open(name);  //Open 'name' file for reading and writing.  

    directory->WriteBack(newDirectory);    // Write modifications to the newDirectory back to disk

    delete directory;


    // Get the sector number of the header of the new folder
    directory = new Directory(NumDirEntries);

    directory->FetchFrom(directoryFile);   //Read the contents of the root directoryFile from disk

    int newDirectorySector = directory->Find(name);  // Look up file name in directory, and return the disk sector number where the file's
    

    // Get the sector number of the header of the current folder
    FileHeader* fileheader = new FileHeader;

    int sector = directory->Find(".");   // gets the sector number of "."

    fileheader->FetchFrom(sector);     //Read the contents of the sector from disk

    int sectorParent = fileheader->LinkSector_Get();  // return dataSectors[0];
    
    delete directory;

    delete fileheader;


    // Proceed into the new folder to create "." and ".."
    Directory_path((std::string(name) + "/").c_str());


    //Create the  directory and parent header
    Create(".", FileHeader::DOTLINK);    // Create a "." directory

    Create("..", FileHeader::DOTLINK);   // Create a ".." directory

    
    // Get the file sector numbers. "." and ".."
    directory = new Directory(NumDirEntries);

    directory->FetchFrom(directoryFile);   //Read the contents of the directoryFile from disk

    
    /*Connection b/w  "." and the current folder*/
    int dirSector = directory->Find(".");

    FileHeader* dirfileheader = new FileHeader;

    dirfileheader->FetchFrom(dirSector);   //Read the contents of the dirsector from disk

    dirfileheader->LinkSector_Set(newDirectorySector);  //return dataSectors[0];

    dirfileheader->WriteBack(dirSector);    // Write modifications to the dirSector back to disk

    delete dirfileheader;
    /*---------------------------------------------*/


    /*Connection b/w  ".." and the parent folder*/
    int pSector = directory->Find("..");

    FileHeader* pfileheader = new FileHeader;

    pfileheader->FetchFrom(pSector);             //Read the contents of the psector from disk

    pfileheader->LinkSector_Set(sectorParent);   //dataSectors[0] = sectorParent;

    pfileheader->WriteBack(pSector);            // Write modifications to the pSector back to disk

    delete pfileheader;
    /*---------------------------------------------*/


    delete directory;

    Directory_path("../");

    return true;
}
// To change a directory using path name
void FileSystem::ChangeDirectory    ( const char* filename )
{
         // Take into account current directory
    std::string filename_s = filename;

    

    char *cpy = new char[strlen(filename_s.c_str()) + 1];
    //Directory *dir = new Directory(NumDirEntries);
    //dir->FetchFrom(directoryFile); //directoryFile has the current directory.
   // char *saveptr = cpy;

    strcpy(cpy, filename_s.c_str());


    char *name = strtok(cpy, "/");
    std::list<std::string> final;

    bool test =true;
    if(test == FALSE)
        printf("asd");
   
    while (name != NULL)
    {
         test = Directory_path(name);




        name = strtok(NULL, "/");
    }
}

 // Function to delete a directory
void FileSystem:: DeleteDirectory (const char *name)
{

	Directory *directory;
    //BitMap *freeMap;
    //FileHeader *hdr;
    //int sector;
   // bool success;
    //success= TRUE;

   
    directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);
     Directory_path((std::string(name) + "/").c_str());

    if (directory->IsEmpty() == false)
    {
        Directory_path("../");
     //   printf("Folder exist found \n");
      //  success = FALSE;
        //return success;
         BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;
    sector = directory->Find(name);
    if (sector == -1) {
       delete directory;
       printf("cannot rm '%s': No such file or directory\n", name);
       return;
    }
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    #ifdef CHANGED
    fileHdr->Deallocate(freeMap,0);         // remove data blocks
    #else
    fileHdr->Deallocate(freeMap);
    #endif
    
    freeMap->Clear(sector);         // remove header block
    directory->Remove(name);

    freeMap->WriteBack(freeMapFile);        // flush to disk
    directory->WriteBack(directoryFile);        // flush to disk
    delete fileHdr;
    delete directory;
    delete freeMap;
    
    }
    else 
        printf("Can't delete as directory is not empty \n");

// /return success;

}
OpenFile *
FileSystem::FreeMap()
{
    return freeMapFile;
}

#endif

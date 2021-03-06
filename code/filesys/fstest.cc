// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"

#define TransferSize 	10 	// make it small, just to be difficult

#ifdef CHANGED
void formatfilesys()
{
   fileSystem = new FileSystem(TRUE);
}
#endif
//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

void
Copy(const char *from, const char *to)
{
    FILE *fp;
    OpenFile* openFile;
    int amountRead;
#ifndef CHANGED
    int fileLength;
#endif
    char *buffer;

// Open UNIX file
    if ((fp = fopen(from, "r")) == NULL) {	 
	printf("Copy: couldn't open input file %s\n", from);
	return;
    }

// Figure out length of UNIX file
    fseek(fp, 0, 2);		
#ifndef CHANGED
    fileLength = ftell(fp);
#endif
    fseek(fp, 0, 0);

// Create a Nachos file of the same length
#ifndef CHANGED
    DEBUG('f', "Copying file %s, size %d, to file %s\n", from, fileLength, to);
#else
    DEBUG('f', "Copying file %s to file %s\n",from, to);
#endif
#ifndef CHANGED
    if (!fileSystem->Create(to,fileLength)) {	 // Create Nachos file
#else
    if (!fileSystem->Create(to)) {
#endif
	printf("Copy: couldn't create output file %s\n", to);
	fclose(fp);
	return;
    }
    
    openFile = fileSystem->Open(to);
    ASSERT(openFile != NULL);
    
// Copy the data in TransferSize chunks
    buffer = new char[TransferSize];
    while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
	openFile->Write(buffer, amountRead);	
    delete [] buffer;

// Close the UNIX and the Nachos files
    delete openFile;
    fclose(fp);
}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void
Print(char *name)
{
    OpenFile *openFile;    
    int i, amountRead;
    char *buffer;

    if ((openFile = fileSystem->Open(name)) == NULL) {
	printf("Print: unable to open file %s\n", name);
	return;
    }
    
    buffer = new char[TransferSize];
    while ((amountRead = openFile->Read(buffer, TransferSize)) > 0)
	for (i = 0; i < amountRead; i++)
	    printf("%c", buffer[i]);
    delete [] buffer;

    delete openFile;		// close the Nachos file
    return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 11000))

static void 
FileWrite()
{
    OpenFile *openFile;    
    int i, numBytes;

    printf("Sequential write of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);
#ifndef CHANGED
    if (!fileSystem->Create(FileName,0)) {
#else
    if (!fileSystem->Create(FileName)) {
#endif
      printf("Perf test: can't create %s\n", FileName);
      return;
    }
    openFile = fileSystem->Open(FileName);
    if (openFile == NULL) {
	printf("Perf test: unable to open %s\n", FileName);
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Write(Contents, ContentSize);
	if (numBytes < 10) {
	    printf("Perf test: unable to write %s\n", FileName);
	    delete openFile;
	    return;
	}
    }
    delete openFile;	// close file
}

static void 
FileRead()
{
    OpenFile *openFile;    
    char *buffer = new char[ContentSize];
    int i, numBytes;

    printf("Sequential read of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);

    if ((openFile = fileSystem->Open(FileName)) == NULL) {
	printf("Perf test: unable to open file %s\n", FileName);
	delete [] buffer;
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Read(buffer, ContentSize);
	if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
	    printf("Perf test: unable to read %s\n", FileName);
	    delete openFile;
	    delete [] buffer;
	    return;
	}
    }
    delete [] buffer;
    delete openFile;	// close file
}

void
PerformanceTest()
{
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove(FileName)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

#ifdef CHANGED
void nachcopy(const char *file1, const char *file2)
{
    OpenFile *fopen1 = NULL;
    if((fopen1 = fileSystem->Open(file1)) == NULL) {
     printf("fail to open file1 %s !\n",file1);
     return;
    }

    int amountRead;
    char *buffer = new char[TransferSize];
    fopen1->Seek(0);

    OpenFile *fopen2 = NULL;
    if((fopen2 = fileSystem->Open(file2)) == NULL) {
     printf("creating a new file %s\n",file2);
     ASSERT(fileSystem->Create(file2));
    }
    if((fopen2 = fileSystem->Open(file2)) == NULL) {
     printf("fail to open file2 %s !\n",file2);
     return;
    }
    fopen2->Seek(0);
    char *buffer2 = new char[TransferSize];
    while((amountRead = fopen1->Read(buffer, TransferSize)) > 0)
     fopen2->Write(buffer, amountRead);
    printf("finish copy");
    printf("\n------------------------\n");
    delete [] buffer2;

    return;
}
#endif

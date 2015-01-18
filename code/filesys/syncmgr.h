// sync.h
//      Routines to manage synchronization for file
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FILESYNCMGR_H
#define FILESYNCMGR_H

#include "copyright.h"
#include "synch.h"

#include <map>
#include <set>
#include <string>

struct FileHdrSync
{
    Semaphore *writer;
    Semaphore *mutex;
    int readcount;

    int reference;
};

class FileSyncMgr
{
public:
    // Keep track of opened file (for removal)
    bool IsOpenedFile(const char* name);
    void NewOpenedFile(const char* name);
    void DeleteOpenedFile(const char* name);

    // Keep track of opened file (for sync)
    void DetachFileSyncForFile(int sector);
    FileHdrSync* GetFileSyncForFile(int sector);

private:
    // Keep track of opened file (for removal)
    std::map<std::string, int> fileNames;

    // Keep track of opened file (for sync)
    // First arg is sector
    std::map<int, FileHdrSync*> fileSync;
};
#endif

// system.h 
//      All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"

#ifdef CHANGED
#define MAX_STRING_SIZE 256
#define MAX_INT_SIZE 9 // lenght(2^32)
#define MAX_OPENFILES 10
#endif

// Initialization and cleanup routines
extern void Initialize (int argc, char **argv);	// Initialization,
						// called before anything else
extern void Cleanup ();		// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;	// the thread holding the CPU
extern Thread *threadToBeDestroyed;	// the thread that just finished
extern Scheduler *scheduler;	// the ready list
extern Interrupt *interrupt;	// interrupt status
extern Statistics *stats;	// performance metrics
extern Timer *timer;		// the hardware alarm clock

#ifdef CHANGED
#include "synchconsole.h"
extern SynchConsole *synchconsole;
#endif

#ifdef USER_PROGRAM
#include "machine.h"
extern Machine *machine;	// user program memory and registers
#endif

#ifdef FILESYS_NEEDED		// FILESYS or FILESYS_STUB
#include "filesys.h"
extern FileSystem *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice *postOffice;
#endif

#ifdef CHANGED
#include "openfile.h"

//this class is used to store the filedescriptor value for all opened files
typedef struct {
  int fd;
  int count;
} tableentry;

class OpenTable {
  public:
    OpenTable();
    ~OpenTable();
    int PushOpenFile(int file);
    int PullOpenFile(int num);
    int GetOpenNum(int file);
    int GetOpenFile(int num);
  private:
    tableentry table[MAX_OPENFILES];
    Lock *binaryLock;
};
extern OpenTable *opentable;
#endif

#endif // SYSTEM_H

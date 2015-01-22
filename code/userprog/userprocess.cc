#ifdef CHANGED

#include "system.h"
#include "userprocess.h"
#include "thread.h"

// TODO DELETE DUMMY
static void StartProcess(int dummy) {

  printf("StartProcess\n");

  DEBUG('t', "StartUserThread with Stack Position: %d\n", currentThread->GetTid());

  if (currentThread->GetTid() < 0) {
    DEBUG('t', "Error, new Thread doesn't have a valid stack space");
    return;
  }

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();

  machine->Run();
}

int do_UserProcessCreate(char *filename) {

  OpenFile *executable = fileSystem->Open(filename);
  AddrSpace *space;

  if (executable == NULL)
  {
    printf ("Unable to open file %s\n", filename);
    return -1;
  }

  space = new AddrSpace(executable);
  delete executable;


  Thread *newThread = new Thread("New Process Thread");
  newThread->space = space;
  
  // newThread->SetTid(0);

  // We'll use it to let Fork know it's a thread, and consecuently not setting the address space again
  ThreadParam *threadParam = new ThreadParam();
  threadParam->isProcess = true;

  newThread->space->increaseUserProcesses();
  newThread->Fork(StartProcess, (int) threadParam);
  currentThread->Yield();
    
  return 0;
}

void do_UserProcessExit() {
  // TODO should check process count and just finish when it's diff to 0
  interrupt->Halt();
}

#endif

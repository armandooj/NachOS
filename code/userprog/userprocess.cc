#ifdef CHANGED

#include "system.h"
#include "userprocess.h"
#include "userthread.h"

// TODO DELETE DUMMY
static void StartProcess(int dummy) {

  DEBUG('t', "StartUserThread with id: %d\n", currentThread->GetPID());

  currentThread->space->InitRegisters();
  //currentThread->space->RestoreState();

  machine->Run();
}

int do_UserProcessCreate(char *filename) {

  OpenFile *executable = fileSystem->Open(filename);

  if (executable == NULL) {
    printf ("Unable to open file %s\n", filename);
    return -1;
  }

  AddrSpace *space;
  space = new AddrSpace(executable);  
  delete executable;

  Thread *newThread = new Thread(filename);
  newThread->space = space;
  newThread->SetPID();  // Set new ID

  machine->IncrementProcesses();

  // We'll use it to let Fork know it's a thread, and consecuently not setting the address space again
  ThreadParam *threadParam = new ThreadParam();
  threadParam->isProcess = true;  

  newThread->Fork(StartProcess, (int) threadParam);
    
  return newThread->GetPID();
}


void do_UserProcessExit() {

  currentThread->space->decreaseUserThreads();
  machine->DecrementProcesses();

  DEBUG('l', "Thread '%d' sends EXIT Signal\n", currentThread->GetPID());
  DEBUG('l', "Number of UserThread %d, userprocess: %d\n", 
                    currentThread->space->getNumberOfUserThreads(),
                    machine->numberOfProcesses);

  while (currentThread->space->getNumberOfUserThreads() != 0) {
    currentThread->space->ExitForMain->P();
  }

  if (machine->numberOfProcesses == 0) {
    interrupt->Halt();
  } else {
    currentThread->Finish();
  }
  
}

#endif

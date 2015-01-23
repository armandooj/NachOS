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
  space->pid = machine->numberOfProcesses++;
  delete executable;

  Thread *newThread = new Thread(filename);
  newThread->space = space;
  newThread->SetTid(0);

  // We'll use it to let Fork know it's a thread, and consecuently not setting the address space again
  ThreadParam *threadParam = new ThreadParam();
  threadParam->isProcess = true;  

  newThread->Fork(StartProcess, (int) threadParam);
  // currentThread->Yield();
    
  return space->pid;
}


void do_UserProcessExit() {
  // TODO should check process count and just finish when it's diff to 0
  // printf("do_UserProcessExit = %s\n", currentThread->getName());
  currentThread->space->activeThreads->PrintContent();


  // currentThread->JoinChildren();
  

  currentThread->space->decreaseUserThreads();
  // printf("Processes: %d\n", machine->numberOfProcesses);

  while (currentThread->space->getNumberOfUserThreads() != 0) {
    currentThread->space->ExitForMain->P();  
  }

  machine->numberOfProcesses--;

  if (machine->numberOfProcesses == 0) {
    // printf("Halt.\n");
    interrupt->Halt();
  } else {
    printf("Finish : %s\n", currentThread->getName());
    // currentThread->Finish();
  }

              
  // Luan's version:

  // DEBUG('t', "Thread '%s' sends EXIT Signal\n", currentThread->getName());
  // DEBUG('t', "Number of UserThread: %d\n", currentThread->space->getNumberOfUserThreads());            
  
  // while (currentThread->space->getNumberOfUserThreads() != 0) {
  //   currentThread->space->ExitForMain->P();  
  // }

  // int value = machine->ReadRegister(4);          
  // DEBUG('a', "Exit program, return value: %d.\n", value);
  // interrupt->Halt();
}

#endif

#ifdef CHANGED

#include "system.h"
#include "userprocess.h"
#include "thread.h"

// // TODO DELETE DUMMY
// static void StartProcess(int dummy) {

//   printf("StartProcess\n");

//   DEBUG('t', "StartUserThread with Stack Position: %d\n", currentThread->GetTid());

//   if (currentThread->GetTid() < 0) {
//     DEBUG('t', "Error, new Thread doesn't have a valid stack space");
//     return;
//   }

//   currentThread->space->InitRegisters();
//   // currentThread->space->RestoreState();

//   machine->Run();
// }

static void StartProcess2(int filename) {
  OpenFile *executable = fileSystem->Open ((char *) filename);
  AddrSpace *space;

  if (executable == NULL)
  {
   printf ("Unable to open file %s\n", (char *) filename);
   return;
  }
  space = new AddrSpace (executable);
  currentThread->space = space;

  delete executable;    // close file

  space->InitRegisters ();  // set the initial register values
  space->RestoreState (); // load page table register

  machine->Run ();    // jump to the user progam
  ASSERT (FALSE);   // machine->Run never returns;
  // the address space exits
  // by doing the syscall "exit"
}

int do_UserProcessCreate(char *filename) {

  Thread *newThread = new Thread("New Process Thread");
  newThread->Fork(StartProcess2, (int) filename);
  currentThread->Yield();

  // OpenFile *executable = fileSystem->Open(filename);

  // if (executable == NULL) {
  //   printf ("Unable to open file %s\n", filename);
  //   return -1;
  // }


  // // currentThread->space->SaveState();

  // AddrSpace *space;
  // space = new AddrSpace(executable);
  // delete executable;

  // Thread *newThread = new Thread("New Process Thread");
  // // newThread->setStatus(JUST_CREATED);
  // newThread->space = space;
  
  // // newThread->SetTid(0);

  // // We'll use it to let Fork know it's a thread, and consecuently not setting the address space again
  // ThreadParam *threadParam = new ThreadParam();
  // threadParam->isProcess = true;

  // //newThread->space->increaseUserThreads();
  // machine->numberOfProcesses++;

  // newThread->Fork(StartProcess, (int) threadParam);
  // // currentThread->Yield();
    
  return 0;
}


void do_UserProcessExit() {
  // TODO should check process count and just finish when it's diff to 0
  printf("do_UserProcessExit\n");

  // currentThread->space->decreaseUserThreads();

  // printf("Processes: %d\n", machine->numberOfProcesses);
  // while (currentThread->space->getNumberOfUserThreads() != 0) {
  //   currentThread->space->ExitForMain->P();  
  // }

  // if (machine->numberOfProcesses == 0) {
  //   printf("Halt.\n");
  //   interrupt->Halt();
  // } else {
  //   printf("Current Thread: %s\n", currentThread->getName());
  //   //currentThread->Finish();
  // }

              // currentThread->space->decreaseUserThreads();
              
              // Luan's version:
            
              // DEBUG('t', "Thread '%s' sends EXIT Signal\n", currentThread->getName());
              // DEBUG('t', "Number of UserThread: %d\n", currentThread->space->getNumberOfUserThreads());            
              
              // while (currentThread->space->getNumberOfUserThreads() != 0) {
              //   currentThread->space->ExitForMain->P();  
              // }

              // int value = machine->ReadRegister(4);          
              // DEBUG('a', "Exit program, return value: %d.\n", value);
              // interrupt->Halt();

  /*
  current children, join
  if processes count = 0 -> int Halt
  else currentThread finish() ?
  */
}

#endif

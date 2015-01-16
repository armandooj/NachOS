// userthread.cc 
//      Entry point into the user level thread 
//
//      do_UserThreadCreat -- Kernel level function to allocate resource
//      and address space for a user level thread
//
//------------------------------------------------------------------------------

#ifdef CHANGED

#include "system.h"
#include "userthread.h"
#include "thread.h"
#include "scheduler.h"

/*
Initialises backups of registers of a new copy of the MIPS interpreter in the same way as the primitive interpreter 
(Machine::InitRegisters\ and \ |Machine::RestoreState\ functions), and starts the interpreter (\| Machine::Run).
*/
static void StartUserThread(int data) {

  DEBUG('t', "StartUserThread with Stack Position: %d\n", currentThread->GetTid());

  if (currentThread->GetTid() < 0) {
    DEBUG('t', "Error, new Thread doesn't have a valid stack space");
    return;    
  }

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState(); // TODO: Check if this need to reverse
  
  // Retrieve the function and arg
  ParamFunction *paramFunction = (ParamFunction *) data;
  // Write the argument in the register 4
  machine->WriteRegister(4, paramFunction->arg);
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister(PCReg, paramFunction->function);
  machine->WriteRegister(NextPCReg, paramFunction->function + 4);
  // Set the stack pointer
  currentThread->space->MultiThreadSetStackPointer((3 * PageSize) * (currentThread->GetTid() + 1));
  
  currentThread->space->increaseUserProcesses();
  
  machine->Run();
}

int do_UserThreadCreate(int f, int arg) {

  // Use a struct to pass both the function and argument to the fork function
  ParamFunction *paramFunction = new ParamFunction();
  paramFunction->function = f;
  paramFunction->arg = arg;

  Thread *newThread = new Thread("New User Thread");
  newThread->Fork(StartUserThread, (int) paramFunction);
  // The thread's id is also its location on the stack
  newThread->SetTid();

  return newThread->GetTid();
}

void do_UserThreadExit() {

    DEBUG('t', "Thread \"%s\"\n Exit", currentThread->getName() );
    DEBUG('t', "Status: number of current userthreads: %d\n", currentThread->space->getNumberOfUserProcesses());
    currentThread->space->decreaseUserProcesses();
    
    // Also frees the corresponding stack location
    currentThread->FreeTid();
    currentThread->Finish();
}

#endif

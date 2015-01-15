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

  // Retrieve the function and arg
  ParamFunction *paramFunction = (ParamFunction *) data;
  
  printf("StartUserThread with Stack Position: %d\n", paramFunction->stack_position);

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState(); // TODO: Check if this need to reverse
  
  // Write the argument in the register 4
  machine->WriteRegister(4, paramFunction->arg);
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister(PCReg, paramFunction->function);
  machine->WriteRegister(NextPCReg, paramFunction->function + 4);
  // Set the stack pointer
  currentThread->space->MultiThreadSetStackPointer((3 * PageSize) * (paramFunction->stack_position + 1));
  
  scheduler->increaseUserProcesses();
  
  machine->Run();
}

int do_UserThreadCreate(int f, int arg) {

  // Use a struct to pass both the function and argument to the fork function
  ParamFunction *paramFunction = new ParamFunction();
  paramFunction->function = f;
  paramFunction->arg = arg;

  Thread *newThread = new Thread("New Thread");
  // We need to set it's address space first so that we can access the stack!
  // TODO Does this always guarantee currentThread's space is not NULL?
  newThread->space = currentThread->space; 

  newThread->SetStackLocation(&paramFunction->stack_position);
  // If something fails just return an error for now
  if (paramFunction->stack_position < 0) {
    return -1;
  }

  // TODO ID?
  // printf("ID: %d\n", paramFunction->id);  
  newThread->Fork(StartUserThread, (int) paramFunction);
  //currentThread->Yield();
  
  return 8;
}

void do_UserThreadExit() {

    DEBUG('t', "Thread \"%s\"\n Exit", currentThread->getName() );
    DEBUG('t', "Status: number of current userthreads: %d\n", 
                                        scheduler->getNumberOfUserProcesses());
    scheduler->decreaseUserProcesses();
    // check to see if there is other threads running
//    if (!scheduler->IsRunningQueueEmpty() )
    currentThread->Finish();
}

#endif

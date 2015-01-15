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

/*
Initialises backups of registers of a new copy of the MIPS interpreter in the same way as the primitive interpreter 
(Machine::InitRegisters\ and \ |Machine::RestoreState\ functions), and starts the interpreter (\| Machine::Run).
*/
static void StartUserThread(int data) {

  printf("StartUserThread with Stack Position: %d\n", currentThread->GetStackLocation());

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
  currentThread->space->MultiThreadSetStackPointer((3 * PageSize) * (currentThread->GetStackLocation() + 1));
  
  machine->Run();  
}

int do_UserThreadCreate(int f, int arg) {

  // Use a struct to pass both the function and argument to the fork function
  ParamFunction *paramFunction = new ParamFunction();
  paramFunction->function = f;
  paramFunction->arg = arg;

  Thread *newThread = new Thread("New Thread");
  // Note, Fork() now sets the stack location
  newThread->Fork(StartUserThread, (int) paramFunction);

  if (newThread->GetStackLocation() < 0) {
    DEBUG('t', "Error creating the thread.");
    return -1;
  }
  
  currentThread->Yield();

  // TODO use a real ID
  return newThread->GetStackLocation();
}

void do_UserThreadExit() {
  currentThread->FreeStackLocation();  
  currentThread->Finish();
}

#endif

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
static void StartUserThread(int f) {
  printf("StartUserThread\n");

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState(); // TODO: Check if this need to reverse
  
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister (PCReg, f);
  machine->WriteRegister (NextPCReg, f + 4);
  // Set the stack pointer
//  machine->WriteRegister (StackReg, (numPages-3)*PageSize - 16);
  currentThread->space->MultiThreadSetStackPointer(3*PageSize);
  
  machine->Run();
  
}

int do_UserThreadCreate(int f, int arg) {

  printf("do_UserThreadCreate\n");

  Thread *newThread = new Thread("New Thread");
  newThread->Fork(StartUserThread, f);
  currentThread->Yield();
  
  return 0;
}

#endif

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
  // call f ??? 
  printf("StartUserThread\n");

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();

  //machine->WriteRegister (4, f);
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister (PCReg, f);
  // Need to also tell MIPS where next instruction is, because
  // of branch delay possibility
  machine->WriteRegister (NextPCReg, f + 4);
  // Set the stack register
  // machine->WriteRegister (StackReg, 0);
  // machine->WriteRegister (RetAddrReg, 12);
  machine->Run();
    // int lol;    
    // int lol = UserStackSize ?? / MaxThread * thread_id + 16;
    // machine->WriteRegister (StackReg, (numPages*PageSize) - lol);
}

int do_UserThreadCreate(int f, int arg) {

  printf("do_UserThreadCreate\n");

  Thread *newThread = new Thread("New Thread");
  newThread->Fork(StartUserThread, f);
  currentThread->Yield();
  
  return 0;
}

#endif

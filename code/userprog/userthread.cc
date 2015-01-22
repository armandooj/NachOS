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
#include "synch.h"

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
  ThreadParam *threadParam = (ThreadParam *) data;

  // Write the argument in the register 4
  machine->WriteRegister(4, threadParam->arg);
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister(RetAddrReg, threadParam->ret_function);
  machine->WriteRegister(PCReg, threadParam->function);
  machine->WriteRegister(NextPCReg, threadParam->function + 4);
  
  //reset stack pointer 
  currentThread->space->MultiThreadSetStackPointer((3 * PageSize) * (currentThread->GetTid()));

  machine->Run();
}

int do_UserThreadCreate(int f, int arg, int ret_function) {

  DEBUG('t', "Enter User Create Thread\n");

  // Use a struct to pass both the function and argument to the fork function
  ThreadParam *threadParam = new ThreadParam();
  threadParam->function = f;
  threadParam->arg = arg;
  threadParam->ret_function = ret_function;

  Thread *newThread = new Thread("New User Thread");
  
  // put increase counter here for synchonization problem
  currentThread->space->increaseUserThreads();    // PROBLEM??? 

  // The thread's id is also its location on the stack
  newThread->SetTid(currentThread->space);  // wrong if set after fork.

  if (newThread->GetTid() < 0) {
    currentThread->space->decreaseUserThreads();
    return -1;
  }
  
  // Add to active list
  currentThread->space->activeThreads->AppendTraverse(NULL, newThread->GetTid());
  DEBUG('l', "Add new thread: %d\n", newThread->GetTid());
  DEBUG('l', "Thread list: \n");
  currentThread->space->activeThreads->PrintContent();                          

  newThread->Fork(StartUserThread, (int) threadParam);
  
  return newThread->GetTid();
}

void do_UserThreadExit() {

    DEBUG('t', "Thread \"%s\" uses User Exit\n", currentThread->getName());
    DEBUG('t', "Status: number of current userthreads: %d\n", 
                            currentThread->space->getNumberOfUserThreads());
    
    DEBUG('l', "Thread \"%s\" uses User Exit\n", currentThread->getName() );
    
    currentThread->space->decreaseUserThreads();
    if (currentThread->space->getNumberOfUserThreads() == 0) {
        currentThread->space->ExitForMain->V();
    }

    // Also frees the corresponding stack location
    currentThread->FreeTid();
    
    // Remove from active list
    currentThread->space->activeThreads->RemoveTraverse(currentThread->GetTid());
    
    // debugging -- delete later
    DEBUG('l', "Delete thread: %d\n", currentThread->GetTid());
    DEBUG('l', "Thread list: \n");
    currentThread->space->activeThreads->PrintContent();
    
    // check queue of active lock to see if anyone needs waking up
    void* thing = currentThread->space->activeLocks->RemoveTraverse(currentThread->GetTid());    
    if (thing != NULL ) {
        // waking up the receipient
        ((JoinWaiting *) thing)->threadWaiting->V();
        
        DEBUG('l', "Delete from waiting thread: %d\n", currentThread->GetTid());
        DEBUG('l', "Waiting thread list: \n");
        currentThread->space->activeLocks->PrintContent();
    }
        
    currentThread->Finish();
}

#endif

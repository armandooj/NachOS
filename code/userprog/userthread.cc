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

  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();

  // Retrieve the function and arg
  ThreadParam *threadParam = (ThreadParam *) data;

  // Write the argument in the register 4
  machine->WriteRegister(4, threadParam->arg);
  // Initial program counter -- must be location of "Start"
  machine->WriteRegister(RetAddrReg, threadParam->ret_function);
  machine->WriteRegister(PCReg, threadParam->function);
  machine->WriteRegister(NextPCReg, threadParam->function + 4);
  
  //reset stack pointer 
  //currentThread->space->MultiThreadSetStackPointer((3 * PageSize) * (currentThread->GetTid()));
  currentThread->SetStackLocation(currentThread->space);

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
  
  newThread->SetPID();   //set ID
  
  // put increase counter here for synchonization problem
  currentThread->space->increaseUserThreads();
  // The thread's id is also its location on the stack
  int location = newThread->SetStackLocation(currentThread->space);

  if (location < 0) {
    // Thread limit reached!
    currentThread->space->decreaseUserThreads();
    return -1;
  }
  
  // Add to active list
  currentThread->space->activeThreads->AppendTraverse(NULL, newThread->GetPID());
  DEBUG('l', "Add new thread: %d\n", newThread->GetPID());
  DEBUG('l', "Thread list: \n");
  currentThread->space->activeThreads->PrintContent();                          

  newThread->Fork(StartUserThread, (int) threadParam);
  
  return newThread->GetPID();
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
    currentThread->FreeStackLocation();
    
    // Remove from active list
    currentThread->space->activeThreads->RemoveTraverse(currentThread->GetPID());
    
    // TODO debugging -- delete later
    DEBUG('l', "Delete thread: %d\n", currentThread->GetPID());
    DEBUG('l', "Thread list: \n");
    currentThread->space->activeThreads->PrintContent();
    
    // check queue of active lock to see if anyone needs waking up
    void* thing = currentThread->space->activeLocks->RemoveTraverse(currentThread->GetPID());    
    if (thing != NULL ) {
        // waking up the receipient
        ((JoinWaiting *) thing)->threadWaiting->V();
        
        DEBUG('l', "Delete from waiting thread: %d\n", currentThread->GetPID());
        DEBUG('l', "Waiting thread list: \n");
        currentThread->space->activeLocks->PrintContent();
    }
        
    currentThread->Finish();
}

// return -1 if error, 0 if not running, 1 if success
int do_UserThreadJoin(int PID) {
    
    DEBUG('l', "Begin join, thread %d waiting for %d\n", currentThread->GetPID(), PID);
    
    //Sanity check
    if (PID == currentThread->GetPID())
        return -1;
    
    // check if the thread is in the active thread,
    if (!currentThread->space->activeThreads->seek(PID))
        return 0;
    
    //build the structure to prepare to sleep, just the semaphore
    //first, send it to the queue 
    JoinWaiting* waitingCondition = new JoinWaiting();
    waitingCondition->PID = currentThread->GetPID();
    waitingCondition->threadWaiting = currentThread->joinCondition;
    
    // add to the queue
    currentThread->space->activeLocks->AppendTraverse((void*) waitingCondition, PID);

    DEBUG('l', "Insert to waiting thread: %d\n", currentThread->GetPID());
    DEBUG('l', "Waiting thread list: \n");
    currentThread->space->activeLocks->PrintContent();
    
    // the synchonization part
    if (!currentThread->space->activeThreads->seek(PID)) {
        //take off the queue and returning
        currentThread->space->activeLocks->RemoveTraverse(PID);
        return 0;
    } else {
        // go to sleep
        DEBUG('l', "Going to sleep: \n");
        currentThread->joinCondition->P();
        DEBUG('l', "Waking up \n");
    }    
    return 1;   // return after being wake up :D
}

#endif

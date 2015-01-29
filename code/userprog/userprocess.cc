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

int do_UserProcessCreate(char *filename, char *arg) {

// NO STACK INITIALIZATION
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
  
  //set extra variable here
  if( arg != 0) { // if NULL, do nothing
    newThread->space->setExtraArg(arg);   
  }
  
  machine->IncrementProcesses();
  machine->activeProcess->AppendTraverse(NULL, newThread->GetPID());

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
    currentThread->space->ExitForMain->P(); //TODO, some processes stuck here
  }
  
  // Remove from active list
  machine->activeProcess->RemoveTraverse(currentThread->GetPID());
  // check queue of active lock to see if anyone needs waking up
  void* thing = machine->activeProcessLocks->RemoveTraverse(currentThread->GetPID());    
  if (thing != NULL ) {
    // waking up the receipient
    ((JoinWaiting *) thing)->threadWaiting->V();
    
    DEBUG('l', "Delete from waiting thread: %d\n", currentThread->GetPID());
    DEBUG('l', "Waiting thread list: \n");
    machine->activeProcessLocks->PrintContent();
  }
  
  //Exit or terminate machine :)
  if (machine->numberOfProcesses == 0) {
    interrupt->Halt();
  } else {
    currentThread->Finish();
  }
  
}

// return -1 if error, 0 if not running, 1 if success
int do_UserProcessJoin(int PID) {

    DEBUG('l', "Begin join, process %d waiting for %d\n", currentThread->GetPID(), PID);
    
    //Sanity check
    if (PID == currentThread->GetPID())
        return -1;
    
    machine->activeProcess->PrintContent();
    // check if the thread is in the active thread,
    if (!machine->activeProcess->seek(PID))
        return 0;
        
    //build the structure to prepare to sleep, just the semaphore
    //first, send it to the queue 
    JoinWaiting* waitingCondition = new JoinWaiting();
    waitingCondition->PID = currentThread->GetPID();
    waitingCondition->threadWaiting = currentThread->joinCondition;
    
    // add to the queue
    machine->activeProcessLocks->AppendTraverse((void*) waitingCondition, PID);
    DEBUG('l', "Insert to waiting thread: %d\n", currentThread->GetPID());
    DEBUG('l', "Waiting thread list: \n");
    machine->activeProcessLocks->PrintContent();
    
    // the synchonization part
    if (!machine->activeProcess->seek(PID)) {
        //take off the queue and returning
        machine->activeProcessLocks->RemoveTraverse(PID);
        return 0;
    }else {
        DEBUG('l', "Going to sleep: \n");
        currentThread->joinCondition->P();
        DEBUG('l', "Waking up \n");
    }    
    return 1;   // return after being wake up :D
}

#endif


#ifdef CHANGED

#include "system.h"
#include "userprocess.h"
#include "thread.h"

int do_UserProcessCreate(char *filename) {

  //TODO: NEED A FORK HERE :DDDDD

  OpenFile *executable = fileSystem->Open (filename);
  AddrSpace *space;

  if (executable == NULL)
  {
   printf ("Unable to open file %s\n", filename);
   return -1;
  }
  
  space = new AddrSpace (executable);
  delete executable;		// close file

  Thread *newThread = new Thread("New Process Thread");
  newThread->space = space;
  newThread->setStatus(JUST_CREATED);
  
  //space->InitRegisters ();	// set the initial register values
  //space->RestoreState ();	// load page table register
  
  //save the old state before jumping
  //currentThread->space->SaveState();

  //Put to ready queue
  IntStatus oldLevel = interrupt->SetLevel (IntOff);
  scheduler->ReadyToRun (newThread);	// ReadyToRun assumes that interrupts 
  // are disabled!
  (void) interrupt->SetLevel (oldLevel);
  
  //machine->Run ();		// jump to the user progam
  // ASSERT (FALSE);		// machine->Run never returns;
  // the address space exits
  // by doing the syscall "exit"    
    
  return 0;
}

#endif

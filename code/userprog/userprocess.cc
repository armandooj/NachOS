
#ifdef CHANGED

#include "system.h"
#include "userprocess.h"

int do_UserProcessCreate(char *) {

  OpenFile *executable = fileSystem->Open (filename);
  AddrSpace *space;

  if (executable == NULL)
  {
   printf ("Unable to open file %s\n", filename);
   return -1;
  }
  
  space = new AddrSpace (executable);
  currentThread->space = space;

  delete executable;		// close file

  space->InitRegisters ();	// set the initial register values
  space->RestoreState ();	// load page table register

  machine->Run ();		// jump to the user progam
  // ASSERT (FALSE);		// machine->Run never returns;
  // the address space exits
  // by doing the syscall "exit"    
    
  return 0;
}

int do_UserProcessExit() {

}

#endif

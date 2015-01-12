// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

#ifndef CHANGED //

    if ((which == SyscallException) && (type == SC_Halt))
      {
	  DEBUG ('a', "Shutdown, initiated by user program.\n");
	  interrupt->Halt ();
      }
    else
      {
	  printf ("Unexpected user mode exception %d %d\n", which, type);
	  ASSERT (FALSE);
      }
    UpdatePC();

#else // CHANGED

    if (which == SyscallException) 
     {
          switch (type) {
             
            case SC_Halt: {
              DEBUG('a', "Shutdown, initiated by user program.\n");
              interrupt->Halt();
              break;
            }

            case SC_PutChar: {
              int rg4 = machine->ReadRegister (4);
              char ch = (char)rg4;
              DEBUG('a', "putchar, initiated by user program.\n");
              synchconsole->SynchPutChar(ch);
              break;
            }

            case SC_GetChar: {
              DEBUG('a', "getchar, initiated by user program.\n");
              int rg2 = (int)synchconsole->SynchGetChar();
              machine->WriteRegister (2, rg2);
              break;
            }

            case SC_GetString: {
              DEBUG('a', "getstring, initiated by user program.\n");
              int rg4 = machine->ReadRegister (4);
              int rg5 = machine->ReadRegister (5);
              char *buffer;
              buffer = &machine->mainMemory[rg4];
              synchconsole->SynchGetString(buffer,rg5);
              break;
             }

            case SC_PutString: {
              DEBUG('a', "putstring, initiated by user program.\n");
              int rg4 = machine->ReadRegister (4);
              unsigned int size, round = 0;
              char buffer[MAX_STRING_SIZE] = {};
              bool status = false;
              do {
               if((size = copyStringFromMachine(rg4+MAX_STRING_SIZE*round,buffer,MAX_STRING_SIZE)) == MAX_STRING_SIZE) {
                synchconsole->SynchPutString(buffer);
                round++;
               }
               else {
                status = true;
                if(size != 0) 
                 synchconsole->SynchPutString(buffer);
               }
              } while(status == false);
              break;
            }

            case SC_PutInt: {
              DEBUG('a', "putint, initiated by user program.\n");
              int n = machine->ReadRegister (4);
              synchconsole->SynchPutInt(n);
              break;
            }

            case SC_GetInt: {
              DEBUG('a', "getint, initiated by user program.\n");
              int n = machine->ReadRegister (4);
              int *buffer = (int*)(&machine->mainMemory[n]);
              synchconsole->SynchGetInt(buffer);
              break;
            }

            default: {
              DEBUG('a', "Shutdown, initiated by user program.\n");
              interrupt->Halt();
            }
              
          }
         // LB: Do not forget to increment the pc before returning!
     }
    // End of addition
     UpdatePC ();
#endif
}

#ifdef CHANGED
int copyStringFromMachine(int from, char *to, unsigned size) {
 int count = 0, i = 0, startfrom = from, buffer;
 unsigned int totalbyte = 0;
 bool status = false;
 if(from % 4) 
  startfrom = from - (from % 4);
 do {
  machine->ReadMem(startfrom+4*count,4,&buffer);
  unsigned char* point = (unsigned char*) &buffer;
  for(i=0;i<4;i++) { 
   if(startfrom + i < from && count == 0) continue;
   if(*(point+i) == '\0') {
     status = true;
     break;
   }
   if(totalbyte > size) {
     status = true;
     break;
   }
   else {
     to[totalbyte] = *(point+i);
     totalbyte++;
   }
  }
  count++;
 } while(status == false);
 if(totalbyte == size) {
  to[size-1] = '\0';
  totalbyte--;
 }
 else
  to[totalbyte] = '\0';
 return totalbyte;
}
#endif

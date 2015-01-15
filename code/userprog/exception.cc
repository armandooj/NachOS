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
#include "userthread.h"
#include "scheduler.h"

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

#ifdef CHANGED

//------------------------------------------------------------
// copyStringFromMachine
//
// Copies a string from the MIPS mode to the Linux mode.
//   from: the address of the string
//   to: the buffer
//   size: the number of character to read from the address
//------------------------------------------------------------
unsigned int copyStringFromMachine(int from, char *to, unsigned size) {
    
    bool stop = false;
    int iteration = 0;
    unsigned int bytesRead = 0;
    int buffer;
    
    //Must read from an address divisible by 4
    int fromPosition = from;
    if (from % 4 != 0 )
        fromPosition = from - (from % 4);
    
    do {
        machine -> ReadMem(fromPosition + iteration * 4, 4, &buffer);
        unsigned char* charArray = (unsigned char*) &buffer;

        //check condition to stop
        for (int i = 0; i < 4; i ++) {
        
            //Skip some of the first element due to alignment issue :D
            if (iteration == 0 && fromPosition + i < from)
                continue;
        
            if (charArray[i] == '\0') {
                stop = true;
                break;
            }
            
            if (bytesRead < size) {
                to[bytesRead] = charArray [i];
                bytesRead ++;
            }
            else {
                stop = true;
                break;
            }
            
        }
        iteration++;
                
    } while (!stop);   
    
    if (bytesRead == size) {
        //WARNING: Replace the last character with \0
        to[size - 1] = '\0';
        bytesRead --;
    }
    else 
        to[bytesRead] = '\0';
    
    return bytesRead;
}
#endif

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
    int type = machine->ReadRegister(2);
    #ifndef CHANGED // Noter le if*n*def
         if ((which == SyscallException) && (type == SC_Halt)) {
             DEBUG('a', "Shutdown, initiated by user program.\n");
             interrupt->Halt();
         } else {
             printf("Unexpected user mode exception %d %d\n", which, type);
             ASSERT(FALSE);
         }

         UpdatePC();
     
     #else // CHANGED
         if (which == SyscallException) {
           switch (type) {
            case SC_Exit: 
            {
              DEBUG('t', "Thread '%s' sends EXIT Signal\n", currentThread->getName());

              DEBUG('t', "Number of UserThread: %d\n", scheduler->getNumberOfUserProcesses() );
                  
                  
              while (scheduler->getNumberOfUserProcesses() != 0) {
                currentThread->Yield();
              }
    
              DEBUG('t', "Status: Running queue empty: %d. Blocking queue empty:%d\n",
                                         scheduler->IsRunningQueueEmpty(), 
                                         interrupt->IsBlockingQueueEmpty());

              int value = machine->ReadRegister(4);          
              DEBUG('a', "Exit program, return value: %d.\n", value);
              interrupt->Halt();
              break;
            }
            case SC_Halt: 
            {
               DEBUG('a', "Shutdown, initiated by user program.\n");
               interrupt->Halt();
               break;
            }
            case SC_PutChar: 
            {  
               int int_c = machine->ReadRegister(4);
               char c = (char) int_c;
               DEBUG('t', "Begin PutChar\n");
               synchconsole->SynchPutChar(c);
               DEBUG('t', "End PutChar\n");
               break;
            }
            case SC_PutString: 
            {                          
                int startPosition = machine->ReadRegister(4);                
                bool stop = false;
                char buffer[MAX_STRING_SIZE] = {};
                int iteration = 0;
                do {
                    unsigned int bytesRead = copyStringFromMachine(
                                                    startPosition + (MAX_STRING_SIZE-1) * iteration,
                                                    buffer, MAX_STRING_SIZE);

                    // check condition to stop. Maximum read size is Max_size_length - 1. 
                    // The last item must be \0
                    if (bytesRead < MAX_STRING_SIZE - 1) {
                        stop = true;
                    }

                    synchconsole->SynchPutString(buffer);
                    iteration ++;
                } while (!stop);
                break;            
            }            
            case SC_UserThreadCreate: 
            {
                int f = machine->ReadRegister(4);
                int arg = machine->ReadRegister(5);
                int thread_id = do_UserThreadCreate(f, arg);
                machine->WriteRegister(2, thread_id);                     
                break;
            }       
            case SC_UserThreadExit: 
            {
                do_UserThreadExit();                  
                break;
            }         
            case SC_GetChar:
            {
                char ch = synchconsole->SynchGetChar();
                machine->WriteRegister(2, int(ch));
                break;
            }
            case SC_GetString:
            {                
                int phy_addr = machine->ReadRegister(4);
                int size = machine->ReadRegister(5);

                // Get char by char so that we can find the end of file.
                int i, ch;
                for (i = 0; i < size - 1; i++) {
                    ch = synchconsole->SynchGetChar();
                    if (ch == EOF) {
                        break;
                    } else {
                        machine->WriteMem(phy_addr + i, 1, ch);
                        if (ch == '\n' || ch == '\0') {
                            break;
                        }
                    }
                }

                // End the String at the end
                machine->WriteMem(phy_addr + i, 1, '\0');
                break;
            }
            case SC_PutInt:
            {
                int val = machine->ReadRegister(4);
                synchconsole->SynchPutInt(val);
                break;
            }
            case SC_GetInt:
            {
                int val = synchconsole->SynchGetInt();
                machine->WriteMem(machine->ReadRegister(4), 4, val);
                break;
            }
            default: {
               printf("Unexpected user mode exception %d %d\n", which, type);
               ASSERT(FALSE);
             }
           }
           UpdatePC();
        }
     #endif // CHANGED
}



#ifdef CHANGED

#include "userthread.h"

extern int do_UserThreadCreate(int f, int arg)
{
        char name = (char)f;
        Thread *yourthread = new Thread(&name);
        parameter *pass = new parameter();
        pass->function = f;
        pass->argument = arg;
        yourthread->Fork(StartUserThread, (int)pass);
        currentThread->Yield();
        return 0;
}

void StartUserThread(int f)
{
        parameter *pass = (parameter*)f;
        int func = pass->function;
        int arg = pass->argument;
        currentThread->space->InitRegisters();
        currentThread->space->RestoreState();
        machine->WriteRegister(4, arg);
        machine->WriteRegister(PCReg, func);
        machine->WriteRegister(NextPCReg, func+4);
        machine->WriteRegister(PrevPCReg, func-4);
        currentThread->space->SetStackPointer(3);
        machine->Run();
}
#endif

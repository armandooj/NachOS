#ifdef CHANGED

#include "synch.h"

////-------------DO NOT CHANGE ------------------

int do_UserThreadCreate(int f, int arg, int ret_function);    //create user thread
void do_UserThreadExit();                   // called when the user thread exit
int do_UserThreadJoin(int tid);            // called when a thread wants to wait
                                          //for another thread to finish
int do_ProcessCreate(char *filename);    // to create a new process


//------------DO NOT CHANGE----------------
typedef struct {  
    int function;
    int arg;
    int ret_function;
    bool isProcess;
} ThreadParam;

//------------DO NOT CHANGE -----------------
typedef struct {
    int PID;
    Semaphore *threadWaiting;
} JoinWaiting;

#endif

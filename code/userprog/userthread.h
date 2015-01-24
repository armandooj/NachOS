#ifdef CHANGED

#include "synch.h"

int do_UserThreadCreate(int f, int arg, int ret_function);    //create user thread
void do_UserThreadExit();                   // called when the user thread exit
int do_UserThreadJoin(int tid);            // called when a thread wants to wait
                                          //for another thread to finish
int do_ProcessCreate(char *filename);    // to create a new process

typedef struct {  
  int function;
  int arg;
  int ret_function;
} ParamFunction;

typedef struct {
  int tid;
  Semaphore *threadWaiting;
} JoinWaiting;

#endif

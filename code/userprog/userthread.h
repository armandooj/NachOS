#ifdef CHANGED

#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "system.h"
#include "thread.h"

extern int do_UserThreadCreate(int f, int arg);

void StartUserThread(int f);

typedef struct {
 int function;
 int argument;
} parameter;

#endif
#endif

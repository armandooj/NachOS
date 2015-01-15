#include "syscall.h"

void print(void *num)
{
     int n = (int)num;
     //PutString("Thread ");
     PutInt(n);
     //PutString(" Created !\n");
}

int main()
{
     UserThreadCreate(print, (void*)1);
     UserThreadCreate(print, (void*)2);
     PutChar('m');
     return 0;
}

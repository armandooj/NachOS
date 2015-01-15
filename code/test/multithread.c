#include "syscall.h"

void print1(void *num)
{
     int n = (int)num;
     PutInt(n);
     PutString(" thread created !");
     PutChar('\n');
     Exit(1);
}

int main()
{
     UserThreadCreate(print1, (void*)1);
     UserThreadCreate(print1, (void*)2);
     PutString("main end.\n");
     UserThreadCreate(print1, (void*)3);
         
     return 0;
}

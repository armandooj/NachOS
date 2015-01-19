#include "syscall.h"

void print(void *c)
{
  PutChar(* ((char *) c));
  UserThreadExit();
}


int main() {

    PutString("Start!\n");
    int t1, t2;
    char ch1 = 'a';
    char ch2 = 'b';
    char *c1 = &ch1;
    char *c2 = &ch2;
    t1 = UserThreadCreate(print, (void *) c1);
    t2 = UserThreadCreate(print, (void *) c2);
    // UserThreadJoin(t1);
    // UserThreadJoin(t2);
    PutString("End\n");
    return 0;
}


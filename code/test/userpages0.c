#include "syscall.h"

void print(void *c)
{
    int i;
    for (i = 0; i < 5; i++) {
        PutChar(* ((char *) c));
    }
}


int main() {

    int t1, t2;
    char ch1 = 'a';
    char ch2 = 'b';
    char *c1 = &ch1;
    char *c2 = &ch2;
    t1 = UserThreadCreate(print, (void *) c1);
    t2 = UserThreadCreate(print, (void *) c2);
    
    ForkExec("putstring");
    
    return 0;
}


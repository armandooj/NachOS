#include "syscall.h"
#define THIS "aaa"
#define THAT "bbb"

const int N = 10; // Choose it large enough!

void my_puts(void *c) {
    char *p; 
    for (p = c; *p != '\0'; p++) 
        PutChar(* ((char *) c));
}


void f(void *s) {
    int i; 
    for (i = 0; i < N; i++) 
        my_puts((char *) s);
}

int main() {
    UserThreadCreate(f, (void *) THIS);
    f((void*) THAT);
    return 0;
}

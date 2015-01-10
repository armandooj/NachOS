
#include "syscall.h"
int main(int argc, char** argv) {

    char s[20];
    GetString(s, 20);
    PutString(s);

    Halt();
    //return 0;
}


#include "syscall.h"
int main() {
    char n;
    GetInt(&n);
    PutInt(n);
    Halt();
}


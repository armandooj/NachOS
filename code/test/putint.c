#include "syscall.h"

int main() {
    int n;
    GetInt(&n);
    PutInt(n);
    Halt();
}



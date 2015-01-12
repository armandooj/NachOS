#include "syscall.h"
int main() {
    int n;
    GetInt(&n);
    PutInt(n);
    return 0;
}
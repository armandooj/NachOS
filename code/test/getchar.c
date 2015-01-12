#include "syscall.h"

int main(int argc, char** argv) {
    char c = GetChar();
    PutChar(c);
    return 0;
}


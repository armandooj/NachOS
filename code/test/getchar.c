#include "syscall.h"

int main(int argc, char** argv) {

    char c = GetChar();
    PutChar(c);
   // printf("\n");
    Halt();
}


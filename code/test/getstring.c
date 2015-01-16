#include "syscall.h"

#define BUFFER_SIZE 10

int main(int argc, char **argv) 
{
    char s[BUFFER_SIZE];
    GetString(s, BUFFER_SIZE);
    PutString(s);
    PutChar('\n');
    return 0;
}

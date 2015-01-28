#include "syscall.h"
int main() {
    int num = 0,fd = -1;
    char buffer[10] = {};
    if((fd = Open("abc")) != -1) 
        if((num = Read(buffer,10,fd)) == 6)
            PutString(buffer);
    Close(fd);
    return 0;
}


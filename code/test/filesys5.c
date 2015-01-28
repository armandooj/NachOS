#include "syscall.h"
int main() {
    int num = 0,fd = -1;
    char *buffer = "world!";
    if((fd = Open("abc")) != -1) 
        if((num = Write(buffer,6,fd)) == 6)
            PutString("ok Write\n");
    Close(fd);
    return 0;
}


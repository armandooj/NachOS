#include "syscall.h"

int main() {
 int fd = -1;
 if(Create("abc") != -1)
    PutString("ok Create\n");
 if((fd = Open("abc")) != -1)
    PutString("ok Open\n");
 if(Close(fd) != -1)
    PutString("ok Close\n");
 return 0;
}

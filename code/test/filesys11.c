#include "syscall.h"
int main() {
 int fd2 = -1,fd3 = -1,fd4 = -1;
 if((fd3 = Open("test11")) != -1)
     PutString("open test11 success\n");
 if((fd4 = Open("test12")) != -1)
     PutString("open test12 success\n");
 if((fd2 = Open("test19")) != -1)
     PutString("open test19 success\n");
 return 0;
}

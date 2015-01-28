#include "syscall.h"
int main() {
 int fd1 = -1,fd2 = -1;
 if((fd1 = Open("test11")) != -1)
     PutString("open test success\n");
 if((fd2 = Open("test10")) != -1)
     PutString("open test10 success\n");
 return 0;
}

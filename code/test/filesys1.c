#include "syscall.h"
int main() {
 int fd3 = -1,fd4 = -1,fd1 = -1,fd2 = -1;
 if((fd1 = Open("test")) != -1)
     PutString("open test success\n");
 if((fd2 = Open("test10")) != -1)
     PutString("open test10 success\n");
 if((fd3 = Open("test17")) != -1)
     PutString("open test17 success\n");
 if((fd4 = Open("test18")) != -1)
     PutString("open test18 success\n");
 return 0;
}

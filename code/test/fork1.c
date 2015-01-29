#include "syscall.h"
int main() {
 int fd3 = -1,fd4 = -1,fd1 = -1,fd2 = -1;
 if((fd1 = Open("test13")) != -1)
     PutString("process2 open test13 success\n");
 if((fd2 = Open("test14")) != -1)
     PutString("process2 open test14 success\n");
 if((fd3 = Open("test15")) != -1)
     PutString("process2 open test15 success\n");
 if((fd4 = Open("test16")) != -1)
     PutString("process2 open test16 success\n");
 return 0;
}

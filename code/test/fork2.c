#include "syscall.h"
int main() {
 int fd2 = -1,fd3 = -1,fd4 = -1;
 if((fd3 = Open("test17")) != -1)
     PutString("process3 open test17 success\n");
 if((fd4 = Open("test18")) != -1)
     PutString("process3 open test18 success\n");
 if((fd2 = Open("test19")) != -1)
     PutString("process3 open test19 success\n");
 return 0;
}

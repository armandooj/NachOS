#include "syscall.h"
int main() {
 int fd4 = -1;
 if((fd4 = Open("test12")) != -1)
     PutString("open test12 success\n");
 return 0;
}

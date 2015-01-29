#include "syscall.h"

int main() {

  int fd8 = -1,fd7 = -1,fd5 = -1,fd6 = -1;
  ForkExec("filesys1", 0);
  ForkExec("filesys11", 0); 
  if((fd5 = Open("test13")) != -1)
      PutString("open test13 success\n");
  if(Close(fd5) == 0)
      PutString("close test13 success\n");
  if((fd6 = Open("test14")) != -1)
      PutString("open test14 success\n");
  if((fd7 = Open("test15")) != -1)
      PutString("open test15 success\n");
  if((fd8 = Open("test16")) != -1)
      PutString("open test16 success\n");
  return 0;
}

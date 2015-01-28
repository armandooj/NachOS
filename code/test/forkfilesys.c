#include "syscall.h"

int main() {

  int fd5 = -1,fd6 = -1;
  Create("test10");
  Create("test11");
  Create("test12");
  Create("test13");
  Create("test14");
  ForkExec("filesys1");
  ForkExec("filesys11"); 
  if((fd5 = Open("test13")) != -1)
      PutString("open test13 success\n");
  if((fd6 = Open("test14")) != -1)
      PutString("open test14 success\n");
  return 0;
}

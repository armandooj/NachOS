#include "syscall.h"

int main() {

  int fd8 = -1,fd7 = -1,fd5 = -1,fd6 = -1;
  if(Create("test9") != -1) 
   PutString("create test9\n");
  if(Create("test10") != -1) 
   PutString("create test10\n");
  if(Create("test11") != -1) 
   PutString("create test11\n");
  if(Create("test12") != -1) 
   PutString("create test12\n");
  if(Create("test13") != -1) 
   PutString("create test13\n");
  if(Create("test14") != -1) 
   PutString("create test14\n");
  if(Create("test15") != -1) 
   PutString("create test15\n");
  if(Create("test16") != -1) 
   PutString("create test16\n");
  if(Create("test17") != -1) 
   PutString("create test17\n");
  if(Create("test18") != -1) 
   PutString("create test18\n");
  if(Create("test19") != -1) 
   PutString("create test19\n");
  ForkExec("fork1-test", 0);
  ForkExec("fork2-test", 0); 
  if((fd5 = Open("test9")) != -1)
      PutString("process1 open test9 success\n");
  if((fd6 = Open("test10")) != -1)
      PutString("process1 open test10 success\n");
  if((fd7 = Open("test11")) != -1)
      PutString("process1 open test11 success\n");
  if((fd8 = Open("test12")) != -1)
      PutString("process1 open test12 success\n");
  return 0;
}

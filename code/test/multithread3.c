#include "syscall.h"
void thread2(void *c)
{
  int fd;
  if((fd = Open("test3")) != -1)
      PutString("open test3 success\n");
  PutChar(* ((char *) c));
  if(Close(fd) != -1)
     PutString("close test3 success\n");
}

void thread1(void *c)
{
  int fd;
  if((fd = Open("test2")) != -1) 
      PutString("open test2 success\n");
  UserThreadCreate(thread2, c);
  PutChar('2');
  PutString("Long Process");  
  if(Close(fd) != -1)
     PutString("close test2 success\n");
  // UserThreadExit(); 
}

int main() {
  
  char ch = 'K';
  char* c = &ch;
  int fd;
  if((fd = Open("test")) != -1)
      PutString("open test success\n");
  UserThreadCreate(thread1, (void *) c);

  PutChar('1');
  if(Close(fd) != -1)
     PutString("close test success\n");
    
  return 0;
}

#include "syscall.h"
void thread2(void *c)
{
  PutChar(* ((char *) c));  
  int fd1 = -1,fd3 = -1;
  if ((fd3 = Open("test11")) != -1)
       PutString("open test11\n");
  if ((fd1 = Open("test14")) != -1)
       PutString("open test14\n");
  // UserThreadExit(); 
}

void thread1(void *c)
{
  int t2 = UserThreadCreate(thread2, c);
  
  int fd1 = -1,fd2 = -1;
  if ((fd2 = Open("test10")) != -1)
       PutString("open test10\n");
  if ((fd1 = Open("test13")) != -1)
       PutString("open test13\n");
  UserThreadJoin(t2);
  PutChar('1');
  
  // UserThreadExit(); 
}

int main() {
  
  char ch = '2';
  char* c = &ch;
  int fd2 = -1,fd1 = -1;
  if ((fd1 = Open("test")) != -1)
       PutString("open test\n");
//  if (Close(fd1) == 0)
 //      PutString("close test\n");
  if ((fd2 = Open("test12")) != -1)
       PutString("open test12\n");

  int t1 = UserThreadCreate(thread1, (void *) c);
  
  UserThreadJoin(t1);
  PutChar('M');
    
  return 0;
}

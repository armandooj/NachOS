#include "syscall.h"
void thread2(void *c)
{
  PutChar(* ((char *) c));  
  int fd1 = -1,fd3 = -1;
  if ((fd3 = Open("test1")) != -1)
       PutString("thread2 open test1\n");
  if ((fd1 = Open("test4")) != -1)
       PutString("thread2 open test4\n");
  // UserThreadExit(); 
}

void thread1(void *c)
{
  int t2 = UserThreadCreate(thread2, c);
  
  int fd1 = -1,fd2 = -1;
  if ((fd2 = Open("test0")) != -1)
       PutString("thread1 open test0\n");
  if ((fd1 = Open("test3")) != -1)
       PutString("thread1 open test3\n");
  UserThreadJoin(t2);
  PutChar('1');
  
  // UserThreadExit(); 
}

int main() {
  
  char ch = '2';
  char* c = &ch;
  int fd2 = -1,fd1 = -1;
  if(Create("test0") != -1)
     PutString("create test0\n");
  if(Create("test1") != -1)
     PutString("create test1\n");
  if(Create("test2") != -1)
     PutString("create test2\n");
  if(Create("test3") != -1)
     PutString("create test3\n");
  if(Create("test4") != -1)
     PutString("create test4\n");
  if(Create("test5") != -1)
     PutString("create test5\n");
  if ((fd1 = Open("test2")) != -1)
       PutString("main thread open test2\n");
  if ((fd2 = Open("test5")) != -1)
       PutString("main thread open test5\n");

  int t1 = UserThreadCreate(thread1, (void *) c);
  
  UserThreadJoin(t1);
    
  return 0;
}

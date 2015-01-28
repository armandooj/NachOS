#include "syscall.h"
void thread2(void *c)
{
  char buffer[4] = {};
  int fd1 = -1;
  if ((fd1 = Open("test")) != -1)
       PutString("open test\n");
  if (Read(buffer,4,fd1) == 4)
      PutString(buffer);
  PutChar(* ((char *) c));  
  // UserThreadExit(); 
}

void thread1(void *c)
{
  int t2 = UserThreadCreate(thread2, c);
  char buffer[4] = {};
  int fd1 = -1;
  if ((fd1 = Open("test")) != -1)
       PutString("open test\n");
  if (Read(buffer,4,fd1) == 4)
      PutString(buffer);
  UserThreadJoin(t2);
  PutChar('1');
  
  // UserThreadExit(); 
}

int main() {
  
  char ch = '2';
  char* c = &ch;
  //char buffer[4] = {};
  char *buf = "testisgood";
  int fd1 = -1;
  Create("test");
  if ((fd1 = Open("test")) != -1)
       PutString("open test\n");
  //if (Read(buffer,4,fd1) == 4)
   //   PutString(buffer);
  int num = Write(buf,10,fd1);
  PutInt(num);
  int t1 = UserThreadCreate(thread1, (void *) c);
  
  UserThreadJoin(t1);
  PutChar('M');
    
  return 0;
}

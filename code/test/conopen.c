#include "syscall.h"
void thread2(void *c)
{
  char buffer[4] = {};
  int fd1 = -1;
  if ((fd1 = Open("test6")) != -1)
      PutString("thread2 open test6:\n");
  if (Read(buffer,4,fd1) == 4) {
      PutString("thread2 read from buffer:\n");
      PutString(buffer);
  }
  PutString("\n------------\n");
}

void thread1(void *c)
{
  int t2 = UserThreadCreate(thread2, c);
  char buffer[4] = {};
  int fd1 = -1;
  if ((fd1 = Open("test6")) != -1)
       PutString("thread1 open test6:\n");
  if (Read(buffer,4,fd1) == 4) {
      PutString("thread1 read from buffer:\n");
      PutString(buffer);
  }
  PutString("\n------------\n");
  UserThreadJoin(t2);
}

int main() {
  
  char ch = '2';
  char* c = &ch;
  char *buf = "testisgood";
  int fd1 = -1;
  Create("test6");
  PutString("content of buffer:\n");
  PutString(buf);
  PutChar('\n');
  if ((fd1 = Open("test6")) != -1)
       PutString("main thread open test6\n");
  Write(buf,10,fd1);
  PutString("\n------------\n");
  int t1 = UserThreadCreate(thread1, (void *) c);
  
  UserThreadJoin(t1);
  Close(fd1);
  return 0;
}

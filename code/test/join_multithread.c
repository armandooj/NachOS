#include "syscall.h"
void thread2(void *c)
{
  PutChar(* ((char *) c));  
  UserThreadExit(); 
}

void thread1(void *c)
{
  int t2 = UserThreadCreate(thread2, c);
  
  UserThreadJoin(t2);
  PutChar('1');
  
  UserThreadExit(); 
}

int main() {
  
  char ch = '2';
  char* c = &ch;

  int t1 = UserThreadCreate(thread1, (void *) c);
  
  UserThreadJoin(t1);
  PutChar('M');
    
  return 0;
}

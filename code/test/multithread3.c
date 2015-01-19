#include "syscall.h"
void thread2(void *c)
{
  PutChar(* ((char *) c));
}

void thread1(void *c)
{
  UserThreadCreate(thread2, c);
  PutChar('2');
  PutString("Long Process");  
  // UserThreadExit(); 
}

int main() {
  
  char ch = 'K';
  char* c = &ch;

  UserThreadCreate(thread1, (void *) c);

  PutChar('1');
    
  return 0;
}

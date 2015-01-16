#include "syscall.h"
void print(void *c)
{
  PutChar(* ((char *) c));
  // PutString("Thread ");
  // PutChar(*((char *) c) );
  // PutString(" is executing\n");
  UserThreadExit(); 
}

int main() {
  
  char ch = 'a';
  char* c = &ch;

  UserThreadCreate(print, (void *) c);
  PutChar('*');
    
  return 0;
}

#include "syscall.h"
void print(void *c)
{
  PutChar('$');
  // PutString("Thread ");
  // PutChar(*((char *) c) );
  // PutString(" is executing\n");
}

int main() {
  
  char ch = '2';
  char* c = &ch;

  // print(c);
  
  UserThreadCreate(print, (void *) c);
  PutChar('N');
  return 0;
}

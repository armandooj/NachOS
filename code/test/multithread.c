#include "syscall.h"
void print(void *c)
{
  PutChar(* ((char *) c));
  // PutString("Thread ");
  // PutChar(*((char *) c) );
  // PutString(" is executing\n");
}

int main() {
  
  char ch = 'm';
  char* c = &ch;
  
  UserThreadCreate(print, (void *) c);
  PutChar('*');
  
  return 0;
}
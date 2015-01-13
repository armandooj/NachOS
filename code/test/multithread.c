#include "syscall.h"
void print (void* c)
{
    PutString("Thread ");
    PutChar( *((char*) c) );
    PutString("is executing");
}

int main() {
  
  char ch = '2';
  char* c = &ch;
  
  UserThreadCreate(print, (void*) c );

  return 0;
}

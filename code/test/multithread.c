#include "syscall.h"
void print(void *c)
{
  PutChar(* ((char *) c));

  // TODO exit the thread!
}

int main() {
  
  char ch = 'a';
  char* c = &ch;
  
  int id = UserThreadCreate(print, (void *) c);
  if (id >= 0) {
    //PutInt(id);
    PutChar('*');
  } else {
    PutString("Error creating a Thread.");
  }

  // UserThreadCreate(print, (void *) c);
  
  return 0;
}


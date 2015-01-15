#include "syscall.h"
void print(void *c)
{
  PutChar(* ((char *) c));

  // int i;
  // for (i = 0; i < 5; i++) {
  //   PutChar('.');
  // }
  
  UserThreadExit();
}

int main() {
  
  char ch = 'a';
  char* c = &ch;
  
  int id = UserThreadCreate(print, (void *) c);
  if (id >= 0) {
    //PutInt(id);
    PutChar('*');
  } else { 
    PutString("Error creating first Thread.");
  }

  ch = 'b';

  int i;
  for (i = 0; i <= 1; i++) {
    id = UserThreadCreate(print, (void *) c);
    if (id >= 0) {      
      PutString("OK");

    } else {
      PutString("ER");
      // PutString("Error creating a Thread -> ");
      // PutInt(i);
      PutString("\n");
    }
  }
  
  return 0;
}

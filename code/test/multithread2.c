#include "syscall.h"
void print(void *c)
{
  PutChar(* ((char *) c));
}

int main() {
  
  char ch = 'a';
  char* c = &ch;
  
  int id = UserThreadCreate(print, (void *) c);
  if (id >= 0) {
    PutInt(id);
  } else { 
    PutString("Error creating first Thread.");
  }

  ch = 'b';

  int i;
  for (i = 0; i <= 4; i++) {
    id = UserThreadCreate(print, (void *) c);

    if (id > 0) {      
      PutInt(id);
    } else {
      PutString("Er");
      PutString("\n");
    }
  }
  
  return 0;
}

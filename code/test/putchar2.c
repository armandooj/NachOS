#include "syscall.h"

int main() {

  int i;
  char c = 'a';
  for (i = 0; i < 10; i++) {
    PutChar(c+i);
  }
  PutChar('\n');
  //Halt(); 
  return 0;
}

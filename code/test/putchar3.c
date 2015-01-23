#include "syscall.h"

int main() {

  int i;
  char c = 'A';
  for (i = 0; i < 10; i++) {
    PutChar(c+i);
  }
  PutChar('\n');
  //Halt(); 
  return 0;
}

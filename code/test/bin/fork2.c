#include "syscall.h"

int main() {

  PutString("Hola");
  PutChar('Y');
  ForkExec("putchar"); 
  return 0;
}

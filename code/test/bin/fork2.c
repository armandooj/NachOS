#include "syscall.h"

int main() {

  PutString("Hola");
  PutChar('Y');
  ForkExec("putchar", 0);   // NULL pointer
  return 0;
}

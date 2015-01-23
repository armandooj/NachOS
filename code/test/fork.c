#include "syscall.h"

int main() {

  ForkExec("putchar2");

  ForkExec("putchar3");
  
  PutChar('*');
  return 0;
}

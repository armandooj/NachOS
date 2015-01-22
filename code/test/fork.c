#include "syscall.h"

int main() {
  ForkExec("userpages0");
  PutChar('b');
  ForkExec("userpages1"); 
  PutChar('c');
  //ForkExec("putstring");
  return 0;
}

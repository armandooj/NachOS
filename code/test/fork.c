#include "syscall.h"

int main() {

  PutString("Hola");
  PutChar('Y');
  ForkExec("userpages0");
  PutChar('E');
  ForkExec("userpages1"); 
  PutChar('S');
  PutInt(182);
  ForkExec("putstring"); 
 
  return 0;
}

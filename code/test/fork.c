#include "syscall.h"

int main() {

  PutString("Hola");
  PutChar('Y');
  ForkExec("userpages0-test", 0);
  PutChar('E');
  ForkExec("userpages1-test", 0); 
  PutChar('S');
  PutInt(182);
  // ForkExec("PutString", 0); 
 
  return 0;
}

#include "syscall.h"
void print (char c, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    PutChar(c+i+1);
  }
  PutChar('\n');
}

int main() {
  print('a',4);
  //Halt(); 
  return 0;
}

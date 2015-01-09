#include "syscall.h"
void print (char* c)
{
    PutString(c);
}

int main() {
  print("Hello World. Here comes the robots");
  Halt(); 
}

#include "syscall.h"

int main()
{
     char select;
     int result = 1;
     do {
      select = GetChar();
      switch (select) {
        case 'a':
          PutString("a is your input\n");
          break;
        case 'b':
          PutString("b is your input\n");
          break;
        case 'c':
          PutString("c is your input\n");
          break;
        default:
          result = 0;
      }
     } while(result);
     Halt();
     return 0;
}

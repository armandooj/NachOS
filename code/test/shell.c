#include "syscall.h"

int
main ()
{  
  SpaceId newProc;
  //OpenFileId input = ConsoleInput;
  //OpenFileId output = ConsoleOutput;
  char prompt[2], buffer[60];
  int i = 0 ;

  prompt[0] = '>';
  prompt[1] = ' ';

  while (1)
  {
    PutChar(prompt[0]);
    PutChar(prompt[1]);

    i = 0;

    do 
    {
      buffer[i] = GetChar();
    }
    while (buffer[i++] != '\n');

    buffer[--i] = '\0';

    if (i > 0)
    {
      // newProc = Exec (buffer);
      // Join (newProc);
      // PutString(buffer);
      // PutChar('\n');
      newProc = ForkExec(buffer);

    }
    break;
  }
  return 0;
}
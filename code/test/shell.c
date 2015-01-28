#include "syscall.h"

int
main ()
{  
  SpaceId newProc;
  //OpenFileId input = ConsoleInput;
  //OpenFileId output = ConsoleOutput;
  char prompt[2], buffer[60], command[60];
  int i = 0, j =0;

  prompt[0] = '>';
  prompt[1] = ' ';

  PutString("\nTeam 1 Nachos OS Beta Version 0.0.0.[...].0.1 :P\n\n");

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

    //clear the command array
    for (j = 0; j < 60; j++) {
        command[j] = 0;
    }

    if (i > 0)
    {
      
      //Parse space
      for (j = 0; j < i; j++ ) {
        if (buffer[j] != ' ')
            command[j] = buffer[j];
      }
      command[j] = '\0';
      
      newProc = ForkExec(command);
      JoinExec(newProc);
    }
    PutChar('\n');
  }
  return 0;
}

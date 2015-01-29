#include "syscall.h"

int
main ()
{  
  SpaceId newProc;
  //OpenFileId input = ConsoleInput;
  //OpenFileId output = ConsoleOutput;
  char prompt[2], buffer[60], command[30], arg[30];
  int i = 0, j =0, count = 0;

  prompt[0] = '>';
  prompt[1] = ' ';

  PutString("\nTeam 1 Nachos OS Beta Version 0.0.0.[...].0.1 :P\n\n");

  while (1)
  {
    PutChar(prompt[0]);
    PutChar(prompt[1]);

    i = 0; j = 0; count = 0;

    do 
    {
      buffer[i] = GetChar();
    }
    while (buffer[i++] != '\n');

    buffer[--i] = '\0';

    //clear the command array
    for (j = 0; j < 30; j++) {
        command[j] = 0;
        arg[j] = 0;
    }

    if (i > 0)
    {
      //Parse space
      for (j = 0; j < i; j++ ) {
        if (buffer[j] == ' ') 
            break; 
        
        command[j] = buffer[j];
      }
      command[j] = '\0';
      
      //Set the next character
      if (buffer[j] == ' ') j++;
      
      for (j=j; j < i; j ++ ) {
        if (buffer[j] == '\0' )
            break;
        arg[count++] = buffer[j];
      }
      arg[count] = '\0';

      //PutString(arg);
      //PutInt(count);
      
      if (count == 0)
        newProc = ForkExec(command, 0);
      else 
        newProc = ForkExec(command, arg);

      JoinExec(newProc);
    }
    PutChar('\n');
  }
  return 0;
}

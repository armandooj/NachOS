#ifdef CHANGED

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
int pointer; 

static void ReadAvail(int arg) { 
	readAvail->V(); 
}

static void WriteAvail(int arg) { 
	writeDone->V(); 
}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
  pointer = 0;

	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 1);
	console = new Console(readFile, writeFile, ReadAvail, WriteAvail, 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
    writeDone->P();
	console->PutChar(ch);

}

char SynchConsole::SynchGetChar()
{
	readAvail->P();
	return (console->GetChar());
}

void SynchConsole::SynchPutString(const char s[])
{
    int k = 0;
    while (s[k] != '\0') {
        SynchPutChar(s[k++]);
    }
}

void SynchConsole::SynchGetString(char *s, int n)
{
  int i = 0; 
  while(s[i] != '\n' && i < n - 1 && s[i] != EOF) {	
	  s[i] = SynchGetChar();
	  i++;
  }
}

void SynchConsole::SynchPutInt(int n)
{
    char buffer[MAX_INT_SIZE + 1] ;
    snprintf(buffer, MAX_INT_SIZE + 1, "%d", n);
    this->SynchPutString(buffer);
}

int SynchConsole::SynchGetInt() {
    char buffer[MAX_INT_SIZE + 1];

    int i = 0; 
    while(i < MAX_INT_SIZE + 1) { 
      buffer[i] = SynchGetChar();
      if (buffer[i] == '\n' || buffer[i] == EOF || buffer[i] == '\0') {        
        break;
      }
      i++;
    }

    int val;
    sscanf(buffer, "%d", &val);
    return val;
}
#endif // CHANGED

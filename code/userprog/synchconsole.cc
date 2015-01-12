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
	writeDone = new Semaphore("write done", 0);
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
	console->PutChar(ch);
	writeDone->P();
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
    char BUF[MAX_INT_SIZE + 1];
    this->SynchGetString(BUF, MAX_INT_SIZE + 1);
    int val;
    sscanf(BUF, "%d", &val);
    return val;
}
#endif // CHANGED

#ifdef CHANGED

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *getBusy;
static Semaphore *putBusy;

static void ReadAvail(int arg) { 
	readAvail->V(); 
}

static void WriteAvail(int arg) { 
	writeDone->V(); 
}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
        getBusy = new Semaphore("read string", 1);
        putBusy = new Semaphore("write string", 1);
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
        putBusy->P();
	console->PutChar(ch);
	writeDone->P();
        putBusy->V();
}

char SynchConsole::SynchGetChar()
{
        getBusy->P();
	readAvail->P();
        getBusy->V();
	return console->GetChar();
}

void SynchConsole::SynchPutString(const char s[])
{
        int i = 0;
        while(s[i] != '\0') {
         SynchPutChar(s[i]);
         i++;
        }
}

void SynchConsole::SynchGetString(char *s, int n)
{
        int i = 0;
        for(i=0;i<n-1;i++) { 
         *(s+i) = SynchGetChar();
         if(*(s+i) == EOF || *(s+i) == '\n')
          break;
        }
}

void SynchConsole::SynchPutInt(int n) 
{       
        char *buffer = new char[MAX_STRING_SIZE]; 
        int i, length = snprintf(buffer,MAX_STRING_SIZE,"%d",n);
        for(i=0;i<length;i++)
         SynchPutChar(*(buffer+i));
        delete[] buffer;
}

void SynchConsole::SynchGetInt(int *n)
{
         char *buffer = new char[MAX_STRING_SIZE];
         int size = MAX_STRING_SIZE;
         SynchGetString(buffer,size);
         sscanf(buffer,"%d",n);
         delete[] buffer;
}
        
#endif // CHANGED

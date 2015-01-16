#ifdef CHANGED

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *Pro_IO;
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
        Pro_IO = new Semaphore("Threads Protecting while I/O", 1);
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
    Pro_IO->P();
	console->PutChar(ch);
    writeDone->P();
    Pro_IO->V();

}

char SynchConsole::SynchGetChar()
{
    Pro_IO->P();
    readAvail->P();
    Pro_IO->V();
	return (console->GetChar());
}

void SynchConsole::SynchPutString(const char s[])
{
    Pro_IO->P();
    int i=0;
    while(i < MAX_STRING_SIZE)
	{
        console->PutChar(*(s + i));
        writeDone->P();
	i++;
    }
    Pro_IO->V();
}

void SynchConsole::SynchGetString(char *s, int n)
{
    Pro_IO->P();
    int i = 0;
    readAvail->P();
    s[i] = console->GetChar();
    while (s[i] != '\n' && i < n - 1 && s[i] != EOF) {
        readAvail->P();
        i++;
        s[i] = SynchGetChar();
    }
    s[i+1] = '\0';
    Pro_IO->V();
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
    while (i < MAX_INT_SIZE) {
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

#ifdef CHANGED
#include
#include
#include
#include
"copyright.h"
"system.h" 
"synchconsole.h"
"synch.h"
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }
SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
readAvail = new Semaphore("read avail", 0);
writeDone = new Semaphore("write done", 0);
console = ...
}
SynchConsole::~SynchConsole()
{
delete console;
delete writeDone;
delete readAvail;
}
void SynchConsole::SynchPutChar(const char ch)
{
// ...
}
char SynchConsole::SynchGetChar()
{
// ...
}
void SynchConsole::SynchPutString(const char s[])
{
// ...
}
void SynchConsole::SynchGetString(char *s, int n)
{
// ...
}
#endif // CHANGED
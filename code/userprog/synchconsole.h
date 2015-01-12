#ifdef CHANGED
#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"

class SynchConsole {
 
        public:
                SynchConsole(char *readFile, char *writeFile);
                ~SynchConsole();
                void SynchPutChar(const char ch);
                char SynchGetChar();
                void SynchPutString(const char *s);
                void SynchGetString(char *s,int n);
                void SynchPutInt(int n);
                void SynchGetInt(int *n);
        private:
                Console *console;
};
#endif // SYNCHCONSOLE_H
#endif // CHANGED

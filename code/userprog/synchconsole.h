#ifdef CHANGED
#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "../threads/copyright.h"
#include "../threads/utility.h"
#include "../machine/console.h"

class SynchConsole {

	public:
		SynchConsole(char *readFile, char *writeFile);
		// initialize the hardware console device
		~SynchConsole();
		// clean up console emulation
		void SynchPutChar(const char ch);
		char SynchGetChar();
		// Unix putchar(3S)
		// Unix getchar(3S)
		void SynchPutString(const char *s); // Unix puts(3S)
		void SynchGetString(char *s, int n);
		// Unix fgets(3S)
		void SynchPutInt(int n);
		int SynchGetInt();
	private:
		Console *console;
		
};
#endif // SYNCHCONSOLE_H
#endif // CHANGED 

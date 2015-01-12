/*
#include "syscall.h"
int main(int argc, char * argv [] ) {

    char s[1000];
    int count;
    count =  (int)argv[1];
    GetString(s, 10);
    PutString(s);

    Halt();
    //return 0;
}
*/


int atoi(char *str)
{
    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit
     
    // If number is negative, then update sign
    if (str[0] == '-')
    {
        sign = -1; 
        i++;  // Also update index of first digit
    }
     
    // Iterate through all digits and update the result
    for (; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
   
    // Return result with sign
    return sign*res;
}


#include "syscall.h"
int main(int argc, char **argv) 
{
int count;
count = atoi (argv[3]);
char s[count];
GetString(s, count);
PutString(s);
Halt();
//return 0;
}


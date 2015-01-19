/**
  * All usefull utils here
  **/

#include "syscall.h"
#define NULL 0
#define Int_Of_Char(x) (x-'0')

typedef unsigned int size_t;

/**
 * Simulate memcpy C
 **/
void *memcpy(void *dest, const void *src, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        ((char*)dest)[i] = ((char*)src)[i];

    return dest;
}

/**
 * Simulate memset C
 **/
void *memset(void *s, int c, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        ((char*)s)[i] = (char)c;

    return s;
}

/**
 * Simplified strcmp, return 1 if same, 0 otherwise
 */
int strcmp(char* str1, char* str2)
{
  int i = 0;
  while( (str1[i] != '\0') || (str2[i] != '\0') )
  {
    if( (str1[i] != str2[i]) || ( (str1[i] == '\0') || (str2[i] == '\0') ) )
    {
      return 0;
    }
    i++;
  }
  
  return 1;
}

int strncmp(char* str1, char* str2, int size)
{
  int i = 0;
  while( ( (str1[i] != '\0') || (str2[i] != '\0') ) && (i<size) )
  {
    if( (str1[i] != str2[i]) || ( (str1[i] == '\0') || (str2[i] == '\0') ) )
    {
      return 0;
    }
    i++;
  }
  
  return 1;
}
    
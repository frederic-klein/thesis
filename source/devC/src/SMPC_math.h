#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif /* STDINT_H_INCLUDED */

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif /* STDIO_H_INCLUDED */

typedef uint32_t (*Fct)(uint32_t, uint32_t);

unsigned int mod (int, int);
unsigned int getPrime (int, int);
unsigned int getRandom (uint32_t, uint32_t);

// pass function to lib for random number generation
void setRNG(uint32_t(*fct)(uint32_t, uint32_t));

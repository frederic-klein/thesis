/*! \file SMPC_math.h
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#include <stdint.h>
#include <stdio.h>

typedef uint32_t (*Fct)(uint32_t, uint32_t); //!< Brief description after the member // TODO


unsigned int mod (long long , int ); //!< Cryptographic modulo operation.
unsigned int getPrime (int, int); //!< Brief description after the member // TODO
unsigned int getRandom (uint32_t, uint32_t); //!< Brief description after the member // TODO
unsigned int mod_fraction(int x, int p);

void setRNG(uint32_t(*fct)(uint32_t, uint32_t)); //!< pass function to lib for random number generation

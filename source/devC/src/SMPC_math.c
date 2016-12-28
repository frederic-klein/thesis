/*! \file SMPC_math.c
    \brief My Documented file 1.
    
    Details.
*/

#include "SMPC_math.h"

Fct rng;

int modInverse(long long a, int m);


/*!
  Computes the modulo x mod p based on the cryptographic modulo definition.
  \param x is an integer and the dividend.
  \param p is an integer and the divisor.
  \return The remainder of x mod p as an unsigned integer.
*/
unsigned int mod (long long x, int p){
	int value;

	if(x<0){
		value = p-((-x)%p);
	}else{
		value = x%p;
	}
	
	return value;
}

unsigned int mod_fraction(long long x, int p){

    int value;
    // If a and m are relatively prime, then modulo inverse
    // is a^(m-2) mod m
     value = modInverse(x,p);
    return value;
}

int modInverse(long long a, int m)
{
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        // q is quotient
        q = a / m;
        t = m;
        // m is remainder now, process same as
        // Euclid's algo
        m = a % m;
        a = t;

        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    // Make x1 positive
    if (x1 < 0) x1 += m0;

    return x1;
}


// define random number generator function
void setRNG(uint32_t(*fct)(uint32_t, uint32_t)){
	rng = fct;
}


uint32_t getRandom(uint32_t min, uint32_t max){
	return rng(min, max);
}


// computes a array of prime candidates between low and high (exclusive)
unsigned int getPrime(int low, int high){
	
	if(low<0 || high<0 || low>high) return 0;
	
	int numbers[high];
	//int primes[];
	
	// i in 2 to high
	int i;
	for(i = 0; i<high; i++)
	{
		numbers[i]=i;
	}
	
	for(i = 2; i<high; i++)
	{
		int j = numbers[i];
		
		if(j!=0)
		{
			unsigned int multiple=j+j;
			while(multiple<high)
			{
				numbers[multiple]=0;
				multiple=multiple+j;
			}
		}
	}
	
	for(i = 0; i<high; i++)
	{
		int j = numbers[i];
		
		if(j!=0 && j>low)
		{
			printf("prime=%i\n",j);
		}
	}
	
	return 1;
}

// function to genereate shares


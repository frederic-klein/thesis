/*! \file SMPC_math.c
    \brief My Documented file.
    
    Details.
*/

/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *  
 * etc...
 */

/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
 
#include "SMPC_math.h"

Fct rng;

/**
 * cryptographic modulo 
 */
// computes the cryptographic modulo definition
unsigned int mod (int x, int p){
	int value;
	// = x%p;
	//if(value<0){
	//	value=value+p;
	//}
	
	if(x<0){
		value = p-((-x)%p);
	}else{
		value = x%p;
	}
	
	return value;
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

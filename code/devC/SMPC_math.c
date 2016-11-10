#include<stdio.h>
#include<stdint.h>

// pass function to lib for random number generation
typedef uint32_t (*Fct)(uint32_t, uint32_t);

Fct rng;

// computes the cryptographic modulo definition
unsigned int mod (int x, int p){
	int value = x%p;
	if(value<0){
		value=value+p;
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

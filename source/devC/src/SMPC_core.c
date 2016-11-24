#include<stdio.h>
#include<sodium.h>
#include "SMPC_math.h"

//TEMP use make all

//struct smpcConfig {
//	byte partiesMin;
//	byte partiesMax;
//	byte threshold;
//	uint32_t minValue;
//	uint32_t maxValue;
//}

//TODO configuration for SMPC
	// min. max number of players in a computation
	// threshold for adversaries -> defines grade of polynomial, number of shares
	// sum verification enabled -> more messages, but detection of cheating
	// realistic bounds for values for sanity check and proper selection of prime
	// connection timeout?
	// how often refresh private key cryptosystem?

//struct pkcs{
//	uint32_t keyLength;
//}
//TODO configuration of public key cryptosystem
	// key length?
	
	
//TODO provide fucntion to set callback for system time
	// -> clock synchronization
	// store delta locally


//TODO provide function to set value for computation
	// for what time-window?

//TODO provide function to set callback for searching and connecting devices
	// bluetooth
	

//TODO move to test/examlpe
/* min included, max excluded */
uint32_t randomNumberGenerator(uint32_t min, uint32_t max){
	char myString[32]; // char 1 Byte
	uint32_t myInt; // 4 Byte
	// ti rtos rand only 32767
	
	if(sodium_init()==-1){
		return 1;
	}

	randombytes_buf(myString, 32);
    myInt = randombytes_uniform(max);
    
    while(myInt<min){
		myInt = randombytes_uniform(max);
	}
	return myInt;
}


void init(){
	setRNG(randomNumberGenerator);
}


int main() {
    
    // replace with github.com/ThrowTheSwitch/Unity
    
    // use libsodium for encryption, decryption, secure cryptographic random number
    init();
    
    printf("random1=%i\n",getRandom(0, 1000));
    printf("random2=%i\n",getRandom(10000,15000));
    printf("random3=%i\n",getRandom(20,25));
    
    unsigned int test1 = mod(-2255,2017); //1779
    unsigned int test2 = mod(2255,2017); //238
    unsigned int test3 = mod(-238,2017); //1779
       
    printf("test1=%i\n",test1);
    printf("test2=%i\n",test2);
    printf("test3=%i\n",test3);
    
    //getPrime(1000,2000);
    
    return 0;
}



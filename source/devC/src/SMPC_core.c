/*! \file SMPC_core.c
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#include <stdio.h>
#include <math.h>
#include <sodium.h>
#include "SMPC_math.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/md5.h"
#include "configurations.h"

typedef unsigned char  byte;
typedef unsigned int   word32;
typedef enum {false, true} bool;

uint32_t randomNumberGenerator(uint32_t min, uint32_t max);

// public functions

void smpc_init(){
    setRNG(randomNumberGenerator);
}

/*!
  TODO description
  \param shares[] array for storing the generated shares.
  \param n number of parties in computation group.
  \param k number of needed shares to reconstruct the secret.
  \param secret value, that shall be split into shares.
*/
void smpc_generate_shares(int shares[], int n, int k, int secret){

    unsigned int p = CONFIGURATIONS_BOUNDING_PRIME;

    unsigned int factors[k-1];
    factors[0]=1;
    // define randomly chosen factors
    for (int l = 1; l < k; ++l) {
        factors[l]=getRandom(1,p);
    }

    // compute share for each party
    for (int i = 0; i < n; ++i) {

        long long share = secret;
        printf("s_%i=%i",(i+1),secret);

        for (int j = 1; j < k; ++j) {

            printf(" + %u*%u^%u",factors[j], (i+1), j);

            unsigned long long power = (unsigned long long)powl((long double)(i+1),j);

            printf("\n%u^%u=%llu\n",i+1,j,power);

            share += ((unsigned long long)factors[j])*power;
        }
        shares[i]=(int)(share%p);
        printf("\n%llu mod %u -> shares[%u]=%u\n",share,p,i,shares[i]);
    }
}

/*!
  TODO description
  \param involved_parties[] 1 based index for involved parties (compare share permutation matrix).
  \param shares[] all received shares: share from party n is at index shares[n-1].
  \param k number of needed shares to reconstruct the secret.
  \return reconstructed secret.
*/
int smpc_lagrange_interpolation(int involved_parties[], int shares[], int k){

    long long secret = 0;

    int party;
    for (int i = 0; i < k; ++i) {
        party = involved_parties[i];

        long long numerator=1;
        long long denominator=1;

        for (int j = 0; j < k; ++j) {

            if(i==j) continue;

            numerator *= -involved_parties[j];
            denominator *= (party - involved_parties[j]);

        }

        // check if numerator/demoninator without rest

        // TODO all modulo... 

        long double result_candidate = ((long double)shares[party-1])*((double)numerator/denominator);

        printf("\t%i*%i/%i\n",shares[party-1],numerator,denominator);

        if(ceill(result_candidate) != result_candidate){

            printf("float-problem:%Lf\n", result_candidate);
//            printf("\t%i*%i/%i\n",shares[party-1],numerator,denominator);

            int denominator_fixed;
            if(denominator<0){
                denominator_fixed = -1*mod_fraction(-1*denominator, CONFIGURATIONS_BOUNDING_PRIME);
            }else{
                denominator_fixed = mod_fraction(denominator, CONFIGURATIONS_BOUNDING_PRIME);
            }
            printf("fixed to: %i\n", denominator_fixed);
//            result_candidate = (long double)shares[party-1]*numerator*denominator_fixed;
            result_candidate = (long double)shares[party-1]*numerator*denominator_fixed;
        }
//        secret += result_candidate;
        secret += mod(result_candidate, CONFIGURATIONS_BOUNDING_PRIME);

        printf("secret+ resultcandidate(%Lf)=%lli\n",result_candidate,secret);
    }
    secret = mod(secret, CONFIGURATIONS_BOUNDING_PRIME);

    return secret;

}

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
	

//TODO move to test/example
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





void smpc_core_debug_run_test() {
    
    // replace with github.com/ThrowTheSwitch/Unity
    
    // use libsodium for encryption, decryption, secure cryptographic random number
    //smpc_init();
    
    printf("random1=%i\n",getRandom(0, 1000));
    printf("random2=%i\n",getRandom(10000,15000));
    printf("random3=%i\n",getRandom(20,25));
    
    unsigned int test1 = mod(-2255,2017); //1779
    unsigned int test2 = mod(2255,2017); //238
    unsigned int test3 = mod(-238,2017); //1779
       
    printf("test1=%i\n",test1);
    printf("test2=%i\n",test2);
    printf("test3=%i\n",test3);
    
    
    char string[] = "Franz jagt im komplett verwahrlosten Taxi quer durch Bayernaaaaa";
    
    printf("length%i\n",sizeof(string)/sizeof(string[0]));
    
	
    
	byte md5sum[MD5_DIGEST_SIZE]; //16 byte
	byte buffer[64];	// must be multiple of 4; if message < buffer -> random bytes in buffer -> not reproducable
	/*fill buffer with data to hash*/
	for(int i=0;i<sizeof(string)/sizeof(string[0]);i++){
		printf("%c\n",string[i]);
		buffer[i]=(byte)string[i];
	}
	
	
	for(int i=0;i<sizeof(buffer)/sizeof(buffer[0]);i++){
		printf("%c",buffer[i]);
	}
	
	printf("\n");

	Md5 md5;
	wc_InitMd5(&md5);
	wc_Md5Update(&md5, buffer, sizeof(buffer)); /*can be called again and again*/
	wc_Md5Final(&md5, md5sum);
	
    for(int i=0;i<MD5_DIGEST_SIZE;i++){
		printf("%02X",md5sum[i]);
	}
    
    //getPrime(1000,2000);
    
}



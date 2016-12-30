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
void reduce(int *numerator, long long *denominator);
int mod_power(int base, int power, int mod);

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

    int p = CONFIGURATIONS_BOUNDING_PRIME;

    int l, i, j, share, power, poly;

    unsigned int factors[k-1];

    // define randomly chosen factors
    for (l = 1; l < k; ++l) {
        factors[l]=getRandom(1,p);
    }

    // compute share for each party
    for (i = 0; i < n; ++i) {

        share = secret;

//        printf("s_%i=%i",(i+1),secret);

        for (j = 1; j < k; ++j) {
            power = mod_power((i+1),j,CONFIGURATIONS_BOUNDING_PRIME);
            poly = ((unsigned long long)factors[j]*power)%CONFIGURATIONS_BOUNDING_PRIME;
            share = ((long long)share+poly)%CONFIGURATIONS_BOUNDING_PRIME;
        }
        shares[i]=(int)(share%p);
//        printf("\n%u mod %u -> shares[%u]=%u\n",share,p,i,shares[i]);
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

    int secret = 0;

    int party;
    for (int i = 0; i < k; ++i) {
        party = involved_parties[i];

        int numerator=1;
//        long long denominator=1;

        for (int j = 0; j < k; ++j) {

            if(i==j) continue;

//            numerator = mod((numerator*(-involved_parties[j])),CONFIGURATIONS_BOUNDING_PRIME);
//            numerator = numerator > 0? -1*(((long long)numerator*(involved_parties[j]))%CONFIGURATIONS_BOUNDING_PRIME) : ((long long)numerator*(involved_parties[j]))%CONFIGURATIONS_BOUNDING_PRIME;
            numerator = ( (long long)numerator*(involved_parties[j]) )%CONFIGURATIONS_BOUNDING_PRIME;

            numerator = mod( ( (long long)numerator*mod_fraction(party - involved_parties[j],CONFIGURATIONS_BOUNDING_PRIME)),CONFIGURATIONS_BOUNDING_PRIME);

//            denominator *= (party - involved_parties[j]);
//
//            if(denominator>CONFIGURATIONS_BOUNDING_PRIME){
//                reduce(&numerator,&denominator);
//            }
//            numerator = mod(numerator, CONFIGURATIONS_BOUNDING_PRIME);
        }

        if(k%2==0){
            numerator *= -1;
        }
        // check if numerator/demoninator without rest

//        printf("\tpre reduce: %i*%i/%i\n",shares[party-1],numerator,denominator);


//        numerator = mod(numerator, CONFIGURATIONS_BOUNDING_PRIME);

//        reduce(&numerator,&denominator);

        numerator = mod((long long)numerator*shares[party-1],CONFIGURATIONS_BOUNDING_PRIME);

//        long double result_candidate = ((long double)mod(shares[party-1]*numerator,CONFIGURATIONS_BOUNDING_PRIME))/denominator;
//        int result;
//        long double result_candidate = (long double)numerator/denominator;
//
//        printf("\tpost reduce: %i/%i\n",numerator,denominator);
//
//
//        if(ceill(result_candidate) != result_candidate){
//
//            printf("float-problem:%Lf\n", result_candidate);
//
//            int denominator_fixed;
//            if(denominator<0){
//                denominator_fixed = -1*mod_fraction(-1*denominator, CONFIGURATIONS_BOUNDING_PRIME);
//            }else{
//                denominator_fixed = mod_fraction(denominator, CONFIGURATIONS_BOUNDING_PRIME);
//            }
//            printf("fixed to: %i\n", denominator_fixed);
//            result = mod((long long)numerator*denominator_fixed, CONFIGURATIONS_BOUNDING_PRIME);
//        }else{
//            result = (int)result_candidate;
//        }


//        secret += result_candidate;
//        secret = mod((long long)secret+result, CONFIGURATIONS_BOUNDING_PRIME);
        secret = mod((long long)secret+numerator, CONFIGURATIONS_BOUNDING_PRIME);

//        printf("secret + result(%i)=%i\n",numerator,secret);
    }
//    secret = mod(secret, CONFIGURATIONS_BOUNDING_PRIME);

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

int mod_power(int base, int power, int mod){

    int value=base;

    for (int i = 1; i < power; ++i) {
        value = ((long long)value*base)%mod;
    }

    return value;

}

void reduce(int *numerator, long long *denominator) {

    printf("%i/%lli ->",*numerator,*denominator);

    int gcd;
    int sign = 1;
    if(*numerator<0 && *denominator>0 || *numerator>0 && *denominator<0){
        sign = -1;
    }
    if(*numerator<0){
        *numerator *=-1;
    }

    if(*denominator<0){
        *denominator *=-1;
    }

    if(*numerator>*denominator){
        gcd = *denominator;
     }else{
        gcd=*numerator;
    }

    while(gcd>1){
        if(*numerator%gcd==0 && *denominator%gcd==0){
            *numerator=*numerator/gcd;
            *denominator=*denominator/gcd;
            gcd = *numerator>*denominator ? *denominator : *numerator;
        }else{
            gcd--;
        }
    }

    if(sign<0){
        *numerator *= sign;
    }

    printf(" %i/%lli\n",*numerator,*denominator);
}


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



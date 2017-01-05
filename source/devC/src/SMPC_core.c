/*! \file SMPC_core.c
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#ifndef WOLFSSL_KEY_GEN
    #define WOLFSSL_KEY_GEN
#endif


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sodium.h>
#include "SMPC_math.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/md5.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/rsa.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/aes.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/random.h"
#include "../lib/wolfssl/wolfssl/wolfcrypt/integer.h"
#include "configurations.h"
#include <time.h>


typedef unsigned char  byte;
typedef unsigned int   word32;
typedef enum {false, true} bool;

uint32_t randomNumberGenerator(uint32_t min, uint32_t max);
//void reduce(int *numerator, long long *denominator);
int mod_power(int base, int power, int mod);
int RsaPublicKeyDecodeRaw(const byte* n, word32 nSz, const byte* e,
                             word32 eSz, RsaKey* key);

//RsaKey private;
//RsaKey public;

// public functions

void smpc_init(){
    setRNG(randomNumberGenerator);

    //todo prepare shares for max computation group

    RsaKey priv;
    RNG rng;
    int ret = 0;
    long e = 65537; // standard value to use for exponent

    wc_InitRsaKey(&priv, NULL); // not using heap hint. No custom memory
    wc_InitRng(&rng);

//    ret = wc_MakeRsaKey(&priv, 2048, e, &rng); // generate 2048 bit long private key
    ret = wc_MakeRsaKey(&priv, CONFIGURATIONS_RSA_KEY_SIZE, e, &rng); // generate 2048 bit long private key

    if( ret != 0 ) {
        // error generating private key
        printf("error generation private-key\n");
    }else{
        printf("no error generation private-key\n");

//        mp_int priv_n = priv.n;
//        unsigned long long  priv_n_int = *priv_n.dp;
//
//        printf("N=%llu\n", *priv.n.dp);
//
//        printf("sizes needed: n=%i, e=%i\n", mp_unsigned_bin_size(&priv.n), mp_unsigned_bin_size(&priv.e));
        byte n[384];
        //byte n[mp_unsigned_bin_size(&priv.n)];
//        mp_to_unsigned_bin(&priv.n, n);
//
        byte e[3];
//        byte e[mp_unsigned_bin_size(&priv.e)];
//        mp_to_unsigned_bin(&priv.e, e);

        RsaKey pub;
        wc_InitRsaKey(&pub, NULL); // not using heap hint. No custom memory

        word32 n_size;
        word32 e_size;
/*        wc_RsaFlattenPublicKey(&priv, e, &e_size, n, &n_size);

        printf("%i %i\n", e_size, n_size);

        ret = RsaPublicKeyDecodeRaw(n, n_size, e, e_size, &pub);*//*

*/
/*        byte *in = "Hallo World, How are you today? Everything fine? 1111111111111111 222222222222222222 12345 123456789"
                "3333333333333333 4444444444444444444 5555555555555555 6666666666666666 7777777777 888888 12345 12345"
                "888888888 9999999999999 11111111 2222222222 33333333333?";
        printf("%s %u\n", in, strlen(in));

        int rsa_encrypt_size;
        rsa_encrypt_size=wc_RsaEncryptSize(&pub);
        printf("RsaEncryptSize %i\n", rsa_encrypt_size);


        byte out[1024];
        int outLen;
        outLen = wc_RsaPublicEncrypt(in, strlen(in), out, sizeof(out), &pub, &rng);

        printf("outlen=%i\n", outLen);

        byte plain[1024];

        int plainSz;
        plainSz = wc_RsaPrivateDecrypt(out, outLen, plain, sizeof(plain), &priv);

        printf("RSA decrypted: %i %.*s\n",plainSz,plainSz,plain);*/

    }


    Aes enc;
    Aes dec;

    const byte key[] = { '1','2','3','4','5','6','7','8','9','1','2','3','4','5','6','7','8','9','1','2','3','4','5','6' }; // 24 Byte =
    const byte iv[] = { '1','2','3','4','5','6','7','8','9','1','2','3','4','5','6','7' }; // 16 Byte

    byte * message = "2147483647";
    int message_length=strlen(message);

    int buffer_size = (message_length/16+1)*16;

    byte plain[buffer_size];   // an increment of 16, fill with data
    byte cipher[buffer_size];
    byte plain_dec[buffer_size];


//    strncpy(plain, "Hallo Welt RtrejHallo Welt abcdefg", 32);
    strncpy(plain, message, strlen(message));

    printf("buffer size=%u key size=%u plain=%.*s\n", buffer_size, sizeof(key), strlen(message), plain);


    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    // encrypt
    wc_AesSetKey(&enc, key, sizeof(key), iv, AES_ENCRYPTION);
    wc_AesCbcEncrypt(&enc, cipher, plain, sizeof(plain));

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    __time_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("\n\nencryption took: %lu useconds \n\n", delta_us);

//    printf("plain:%.*s -> cipher: %.*s\n", strlen(message), plain, strlen(message), cipher);

    //    cipher now contains the cipher text from the plain text.

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    // decrypt
    wc_AesSetKey(&dec, key, sizeof(key), iv, AES_DECRYPTION);
    wc_AesCbcDecrypt(&dec, plain_dec, cipher, sizeof(cipher));

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("\n\ndecryption took: %lu useconds \n\n", delta_us);

    printf("plain: %.*s decoded to: %.*s\n", strlen(message), plain, strlen(message), plain_dec);

}


int RsaPublicKeyDecodeRaw(const byte* n, word32 nSz, const byte* e,
                             word32 eSz, RsaKey* key)
{

    key->type = RSA_PUBLIC;

    if (mp_init(&key->n) != MP_OKAY)
        return -1;

    if (mp_read_unsigned_bin(&key->n, n, nSz) != 0) {
        mp_clear(&key->n);
        return -2;
    }

    if (mp_init(&key->e) != MP_OKAY) {
        mp_clear(&key->n);
        return -3;
    }

    if (mp_read_unsigned_bin(&key->e, e, eSz) != 0) {
        mp_clear(&key->n);
        mp_clear(&key->e);
        return -4;
    }

    return 0;
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
        factors[l]=getRandom(1,(unsigned int)p);
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
        shares[i]=share%p;
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

/*void reduce(int *numerator, long long *denominator) {

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
}*/


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
    
    printf("length%i\n",(int)(sizeof(string)/sizeof(string[0])));
    
	
    
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



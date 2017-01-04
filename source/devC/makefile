all: clean SMPC_core

SMPC_core: SMPC_core.o SMPC_math.o md5.o rsa.o random.o
	gcc -o SMPC_core SMPC_core.o SMPC_math.o md5.o -lsodium -lm
	
SMPC_core.o:
	gcc -Ilib/wolfssl -c src/SMPC_core.c
	
SMPC_math.o: src/SMPC_math.c
	gcc -c src/SMPC_math.c

node.o:
	gcc -DEVAL_VALUE=30 -DCONFIGURATIONS_SHARES_THRESHOLD=0 -c src/node.c

node_eval:
	gcc -DEVAL_VALUE=$(n) -DCONFIGURATIONS_SHARES_THRESHOLD=$(a) -c src/node.c

tester.o:
	gcc -c src/tester.c
	
unity.o:
	gcc -c lib/Unity/src/unity.c
	
rsa.o:
	gcc -Ilib/wolfssl -DWOLFSSL_KEY_GEN -c lib/wolfssl/wolfcrypt/src/rsa.c

md5.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/md5.c

random.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/random.c

integer.o:
	gcc -Ilib/wolfssl -DWOLFSSL_KEY_GEN -c lib/wolfssl/wolfcrypt/src/integer.c

memory.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/memory.c

hash.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/hash.c

sha256.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/sha256.c

sha.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/sha.c

aes.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/aes.c

SMPC_math_test.o:
	gcc -c test/SMPC_math_test.c

test: clean SMPC_math.o unity.o SMPC_math_test.o
	gcc -o test_Math SMPC_math_test.o SMPC_math.o unity.o
	./test_Math

tester: tester.o node_eval SMPC_core.o SMPC_math.o md5.o rsa.o random.o memory.o integer.o hash.o sha256.o sha.o aes.o
	gcc -o tester node.o tester.o SMPC_core.o SMPC_math.o md5.o rsa.o random.o  memory.o integer.o hash.o sha256.o sha.o aes.o -lsodium -lm

clean:
	rm -f *.o SMPC_core test_Math tester

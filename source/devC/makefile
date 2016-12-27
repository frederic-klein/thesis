all: clean SMPC_core

SMPC_core: SMPC_core.o SMPC_math.o md5.o
	gcc -o SMPC_core SMPC_core.o SMPC_math.o md5.o -lsodium -lm
	
SMPC_core.o:
	gcc -Ilib/wolfssl -c src/SMPC_core.c
	
SMPC_math.o: src/SMPC_math.c
	gcc -c src/SMPC_math.c

node.o: src/node.c
	gcc -c src/node.c

tester.o:
	gcc -c src/tester.c
	
unity.o:
	gcc -c lib/Unity/src/unity.c
	
md5.o:
	gcc -Ilib/wolfssl -c lib/wolfssl/wolfcrypt/src/md5.c
	
SMPC_math_test.o:
	gcc -c test/SMPC_math_test.c

test: clean SMPC_math.o unity.o SMPC_math_test.o
	gcc -o test_Math SMPC_math_test.o SMPC_math.o unity.o
	./test_Math

tester: tester.o node.o SMPC_core.o SMPC_math.o md5.o
	gcc -o tester node.o tester.o SMPC_core.o SMPC_math.o md5.o -lsodium -lm

clean:
	rm -f *.o SMPC_core test_Math tester

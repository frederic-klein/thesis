all: SMPC_core

SMPC_core: SMPC_core.o SMPC_math.o
	gcc -o SMPC_core SMPC_core.o SMPC_math.o -lsodium
	
SMPC_core.o:
	gcc -c src/SMPC_core.c
	
SMPC_math.o: src/SMPC_math.c
	gcc -c src/SMPC_math.c
	
unity.o:
	gcc -c lib/Unity/src/unity.c
	
SMPC_math_test.o:
	gcc -c test/SMPC_math_test.c
	
test: SMPC_math.o unity.o SMPC_math_test.o
	gcc -o test_Math SMPC_math_test.o SMPC_math.o unity.o
	./test_Math
	
clean:
	rm SMPC_core.o SMPC_math.o SMPC_core test_Math

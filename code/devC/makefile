all: SMPC_core

SMPC_core: SMPC_core.o SMPC_math.o
	gcc -o SMPC_core SMPC_core.o SMPC_math.o -lsodium
	
SMPC_core.o: SMPC_core.c
	gcc -c SMPC_core.c
	
SMPC_math.o: SMPC_math.c
	gcc -c SMPC_math.c
	
clean:
	rm SMPC_core.o SMPC_math.o SMPC_core

all: SMPC_core

SMPC_core: SMPC_core.o SMPC_math.o
	gcc -o SMPC_core SMPC_core.o SMPC_math.o -lsodium
	
SMPC_core.o: src/SMPC_core.c
	gcc -c src/SMPC_core.c
	
SMPC_math.o: src/SMPC_math.c
	gcc -c src/SMPC_math.c
	
clean:
	rm SMPC_core.o SMPC_math.o SMPC_core

test: all
	gcc -o SMPC_core SMPC_core.o SMPC_math.o -lsodium
	

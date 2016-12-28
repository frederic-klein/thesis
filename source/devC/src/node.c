/*! \file node.c
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SMPC_core.h"
#include "configurations.h"

// constants

typedef enum { STATE_NONE, STATE_READY, STATE_BUSY } state_smpc;
typedef enum { SMPC_ADDITION, SMPC_MAX, SMPC_MIN } type_smpc;
typedef enum { STATE_NODE, STATE_COORDINATOR } state_node;

// variables

int score;
int computation_group[CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP];
int computation_group_size;
char mac_adresses[CONFIGURATIONS_DISCOVERY_LIMIT][18];
int mac_count;
void (*discovery_function_pointer)(char macs[][18], int* result_count);
void(*send_function_pointer)(char *target, char *message);

// private function definitions

void send_message(char *target, char *unencrypted_message);
void fill_share_permutation_matrix(int *matrix_start, int size, int threshold);
void compute_max_computation_group(int *matrix_start, int size, int result[]);
void compute_max_computation_group_recursion(int *cleaned_matrix_start, int size, int group[], int group_size, int index, int current_group[], int current_group_size);

// helper
void get_involved_parties(int *permutation_matrix, int involved_parties[], int party, int n, int k);
int* get_value_pointer(int *matrix_start, int size, int row, int column);
void print_matrix(int *matrix_start, int size);
void print_array(int *array_start, int size);

// public functions

void node_init(){
    smpc_init();
}

void node_pass_score(int newScore){

    score = newScore;

    printf("node received score: %i\n", score);

    // computation group fixed size? // todo move to a init section?
    // yes --> prepare shares
    if(CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP==CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP){

        int n = CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP;
        int adversaries = CONFIGURATIONS_SHARES_THRESHOLD;

        if(adversaries>n/2){
            adversaries=n/2;
        }

        int k = n-adversaries;
        int share_matrix[n][n];
        fill_share_permutation_matrix(&share_matrix[0][0], n, k);

        print_matrix(&share_matrix[0][0], n);

        int shares[n];
        smpc_generate_shares(shares, n, k, score);

        // shares now holds the shares to be send to parties as defined in share_computation_matrix

        // test

        for (int i = 0; i < n; ++i) {
            int parties_test[k];
            get_involved_parties(&share_matrix[0][0], parties_test, i+1, n, k);

            printf("party: %u ", (i+1));

            print_array(&parties_test[0], k);

            printf("print_array done\n");

            int restored_secret = smpc_lagrange_interpolation(parties_test, shares, k);


            if(restored_secret!=score){
                printf("\033[31m" "secret=%u restored=%u\n" "\033[0m", score, restored_secret);
            }else{
                printf("\033[32m" "secret=%u restored=%u\n" "\033[0m", score, restored_secret);
            }
            break;
        }

    }



    // no --> TODO


    //TODO trigger computation
}

void node_set_discovery_function(void (*func)(char macs[][18], int* result_count)){
    discovery_function_pointer = func;
}

void node_set_send_function(void (*func)(char *target, char *message)){
    send_function_pointer = func;
}

void node_pass_message(char *source, char *message){
    printf("node received from %s: %s\n",source, message);

    //TODO decrypt message

    //TODO handle message
}


// private functions

void get_involved_parties(int *permutation_matrix, int involved_parties[], int party, int n, int k){

        int index = 0;
        for (int j = 0; j < n; ++j) {
            if(*get_value_pointer(permutation_matrix, n, (party-1), j)==1){
                involved_parties[index]=(j+1);
                index++;
            }
//            printf("matrix value %u", *get_value_pointer(permutation_matrix, n, (party-1), j));
        }
//        printf("\n");




}

void send_message(char *target, char *unencrypted_message){

    //TODO encrypt

    char encrypted_message[strlen(unencrypted_message)];

    for (int i = 0; i < strlen(unencrypted_message); ++i) {
        encrypted_message[i]=unencrypted_message[i];
    }

    (*send_function_pointer)("11:22:33:44:55:66", encrypted_message);

}

/*!
  Computes share matrix based on configurations settings
  \param matrix_start pointer to square matrix
  \param size of the square matrix
  \param threshold k, number of shares needed; k-1 adversaries can be tolerated.
*/
void fill_share_permutation_matrix(int *matrix_start, int size, int threshold){
	int i,j;

    for (i = 0; i < size; i++) {
        for(j=0; j<size;j++){
            *(matrix_start + (i*size) + j)=0;
        }
    }

	for (i = 0; i < size; i++) {
		for(j=size-threshold+i; j<size;j++){
            *(matrix_start + (i*size) + j)=1;
            *(matrix_start + ((size-i-1)*size) + (size-j-1))=1;
		}
	}
	int rowsum;
	for (i = 0; i < size; i++) {
		rowsum = 0;
		for (j = 0; j < size; j++) {
			rowsum+=*(matrix_start + (i*size) + j);
		}
		for (j = 0; j < threshold-rowsum; j++) {
			*(matrix_start + ((i+j)*size) + i)=1;
			*(matrix_start + (i*size) + (i+j))=1;
		}
	}
}

void compute_max_computation_group(int *matrix_start, int size, int result[]) {

    computation_group_size = 0;

//    print_matrix(matrix_start, size);

    int max_group_count = 0;

    // clean up
    int* cell;
    for (int i = 0; i < size; ++i) {
        for (int j = i+1; j < size; ++j) {
            cell = get_value_pointer(matrix_start, size, i, j);
            if(*cell != *get_value_pointer(matrix_start, size, j, i)){
                *cell = 0;
            }
            *get_value_pointer(matrix_start, size, j, i)=0;
        }
    }

    int group_candidate[size];

    int index;
    for (int i = 0; i < size; ++i) {
        index = 0;
        for (int j = i; j < size; ++j) {
            cell = get_value_pointer(matrix_start, size, i, j);
            if(*cell == 1){
                group_candidate[index] = j;
                index++;
            }
        }
        if(index >= CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP){

//            printf("minimum group size = %u", CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP);

//            print_array(&group_candidate[0], index);
            int current_group[index];

            for (int j = 1; j < index; ++j) {
                compute_max_computation_group_recursion(matrix_start, size, group_candidate, index, j, current_group, 0);
//                compute_max_computation_group_recursion(matrix_start, size, group_candidate, index, index-1, current_group, 0);
            }

        }

    }

    printf("\nMax computation group:\n\t");
    print_array(&computation_group[0], computation_group_size);

}


void compute_max_computation_group_recursion(int *cleaned_matrix_start, int size, int group[], int group_size, int group_index, int current_group[], int current_group_size) {

//    printf("group[%u]=%u\n",group_index,group[group_index]);

    current_group[current_group_size]=group[group_index];
    current_group_size++;

    if(group_size<computation_group_size){
        return;
    }

    if(computation_group_size==CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP) return;


    if(current_group_size+group_index<=computation_group_size){
        return;
    }



    if(group_index==0){

        if(current_group_size>computation_group_size){
            for (int i = 0; i < current_group_size; ++i) {
                computation_group[i] = current_group[i];
            }
            computation_group_size = current_group_size;
        }

//        printf("meshed group:\n\t");
//        print_array(&current_group[0], current_group_size);
        return;
    }

    // from last to first
    for (int i = group_index; i > 0; --i) {

        int meshed = 1;

        for (int j = 0; j < current_group_size; ++j) {
            if(*get_value_pointer(cleaned_matrix_start, size, group[i-1], current_group[j]) == 0) {
                meshed = 0;
            }
        }

        if(meshed == 1){

//            printf("\tgroup[%u]=%u\n",i,group[i]);

            //printf("\t Matrix[%u,%u]=%u\n", group[i-1], group[group_index], *get_value_pointer(cleaned_matrix_start, size, group[i-1], group[group_index]) );

            compute_max_computation_group_recursion(cleaned_matrix_start, size, group, group_size, i-1, current_group, current_group_size);
        }





//        current_group[current_group_size]=group[i];
//
//        if(*get_value_pointer(cleaned_matrix_start, size, group[i-1], group[i])){
//
//
//
//        }

    }
    // check if column has next entry



}



int* get_value_pointer(int *matrix_start, int size, int row, int column){
    return (matrix_start + (row*size) + column);
}

void print_matrix(int *matrixStart, int size){
	int i,j;
	for(i=0; i<size; i++){
		for(j=0; j<size; j++){
            printf("%u; ", *get_value_pointer(matrixStart, size, i, j));
		}
		printf("\n");
	}
}


void print_array(int *array_start, int size){
    int i;
    printf("[");
    for(i=0; i<size; i++){
        printf(" %u", *(array_start+i));
    }
    printf(" ]\n");
}


void node_debug_run_test(){

    // share matrix generation
	/*
	const int k = 5;
	const int n = 15;
	int share_matrix[n][n];
    fill_share_permutation_matrix(&share_matrix[0][0], n, k);
    print_matrix(&share_matrix[0][0], n);
    */

    // computation group computation
    int computation_candidates_matrix[10][10] = {
            {1,0,1,0,1,0,1,0,1,0},
            {0,1,1,1,1,1,1,1,0,0},
            {0,1,1,1,0,1,0,1,0,0},
            {1,1,1,1,1,0,1,1,1,1},
            {1,1,0,1,1,0,1,1,0,1},
            {0,1,1,0,1,1,0,1,1,0},
            {1,1,1,1,1,1,1,0,1,1},
            {1,0,0,1,0,0,1,1,0,1},
            {1,1,1,0,1,1,0,1,1,1},
            {1,1,0,1,1,0,1,1,0,1},
    };
    int computation_group[10];
    compute_max_computation_group(&computation_candidates_matrix[0][0], 10, computation_group);

    // computation group for random matrix 100 node and max group of 20 ~ 9 seconds
    int a = 100;
    int random_value;
    int computation_candidates_matrix_new[a][a];
    for (int i = 0; i < a; ++i) {
        for (int j = 0; j < a; ++j) {
            if(i==j){
                computation_candidates_matrix_new[i][j] = 1;
            }else{
                random_value = rand();
                computation_candidates_matrix_new[i][j] = (random_value%2 || random_value%3);
            }
        }
    }
    int computation_group_new[a];
    compute_max_computation_group(&computation_candidates_matrix_new[0][0], a, computation_group_new);

    // test discovery function pointer
    (*discovery_function_pointer)(mac_adresses, &mac_count);
    for (int l = 0; l < mac_count; ++l) {
        printf("%s\n",mac_adresses[l]);
    }

    // test send message function pointer
    printf("lib sending.\n\t");
    send_message("11:22:33:44:55:66", "some encrypted message to the receiver");

}

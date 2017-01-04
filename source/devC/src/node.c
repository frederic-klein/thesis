/*! \file node.c
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SMPC_core.h"
#include "configurations.h"
#include <time.h>

// constants

typedef enum { STATE_IDLE, STATE_READY, STATE_SECURE, STATE_BUSY } state_smpc;
typedef enum { SMPC_NONE=0, SMPC_SUM=1, SMPC_MIN=2, SMPC_MAX=4 } type_smpc; // these are not yet computed
typedef enum { STATE_REQUEST=10, STATE_RESPONSE, BROADCAST, BROADCAST_RESPONSE, DISCOVERY_START_REQUEST,
    DISCOVERY_REQUEST, DISCOVERY_RESPONSE, GROUP_BROADCAST, ENCRYPTION_REQUEST, SHARE_REQUEST, SHARE_RESPONSE,
    COMPUTATION_RESULT_REQUEST, COMPUTATION_RESULT_RESPONSE, ACK, NACK } type_message;
typedef enum { STATE_NODE, STATE_COORDINATOR, STATE_TEMPORARY_COORDINATOR } state_node;

// TODO
//struct party
//{
//    char mac[18];
//    // rsa key
//    // AES key
//};

//const char *PREFIX_REQUEST = "REQ";

// variables

state_smpc current_smpc_state;
state_node current_node_state;
int smpc_options;

int score;
int k, n;
int share_matrix[CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP][CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP];
int computation_group[CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP];
int computation_group_size;
char *mac_own;
char mac_adresses[CONFIGURATIONS_DISCOVERY_LIMIT][18];
char response_arr[CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP*18];
int mac_count;
void (*discovery_function_pointer)(char macs[][18], int* result_count);
int (*await_function_pointer)(char * source, char response[]);
void(*send_function_pointer)(char *target, char *message);

// private function definitions

void send_message(char *target, char *unencrypted_message);
void broadcast(char macs[][18], int count, char *unencrypted_message);
void discovery();
void fill_share_permutation_matrix(int *matrix_start, int size, int threshold);
void compute_max_computation_group(int *matrix_start, int size, int result[]);
void compute_max_computation_group_recursion(int *cleaned_matrix_start, int size, int group[], int group_size, int index, int current_group[], int current_group_size);
void become_coordinator();

// helper
void get_involved_parties(int *permutation_matrix, int involved_parties[], int party, int n, int k);
int* get_value_pointer(int *matrix_start, int size, int row, int column);
void print_matrix(int *matrix_start, int size);
void print_array(int *array_start, int size);
void int_to_char_arr(char char_arr[4], unsigned int value);
unsigned int char_arr_to_int(char lsb, char lsb2, char msb2, char msb);


// public functions

void node_init(char *mac){

    printf("limit=%i\n",CONFIGURATIONS_DISCOVERY_LIMIT);

    mac_own = mac;
    smpc_init();
    smpc_options = SMPC_NONE;
    current_smpc_state = STATE_IDLE;
    current_node_state = STATE_NODE;

    if(CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP==CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP) {

        n = CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP;
        int adversaries = CONFIGURATIONS_SHARES_THRESHOLD;

        if (adversaries > n / 2) {
            adversaries = n / 2;
        }
        k = n - adversaries;
        fill_share_permutation_matrix(&share_matrix[0][0], n, k);
//        print_matrix(&share_matrix[0][0], n);
    }
}

void node_pass_score(int newScore){

    score = newScore;
//    printf("node received score: %i\n", score);

    // prepare shares
    if(CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP==CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP){

        int shares[n]; // move shares to SMPC_core
        smpc_generate_shares(shares, n, k, score, SMPC_CORE_SUM);
        // shares now holds the shares to be send to parties as defined in share_computation_matrix

        int max_limit = CONFIGURATIONS_MAX_SCORE;
        int rounds = 0;
        while (max_limit > 0) {
            rounds++;
            max_limit = max_limit >> 1;
        }

        int shares_max[rounds][n];

        //
        int secret_bits[rounds];

        int secret_copy = score;
        int secret_bit_length = 0;
        while (secret_copy > 0) {
            secret_bit_length++;
            secret_copy = secret_copy >> 1;
        }

        for (int j = 0; j < rounds; ++j) {
            secret_bits[j]=0;
        }

        for(int i=0;i<secret_bit_length;++i){
//            printf("%i\n",(score>>i)&1);
            secret_bits[rounds-secret_bit_length+i]=(score>>(secret_bit_length-1-i))&1;
        }

        printf("%u -> ", score);

        for (int l = 0; l < rounds; ++l) {
            printf("%u", secret_bits[l]);
        }

        printf("\n");

        // TODO also prepare shares for min/max (each with two cases for each round)


        // TODO remove; test if shares and lagrange interpolation are correct
//        for (int i = 0; i < n; ++i) {
//            int parties_test[k];
//            get_involved_parties(&share_matrix[0][0], parties_test, i+1, n, k);
//
//            printf("party: %u ", (i+1));
//            print_array(&parties_test[0], k);
//
//            int restored_secret = smpc_lagrange_interpolation(parties_test, shares, k);
//
//            if(restored_secret!=score){
//                printf("\033[31m" "secret=%u restored=%u\n" "\033[0m", score, restored_secret);
//            }else{
//                printf("\033[32m" "secret=%u restored=%u\n" "\033[0m", score, restored_secret);
//            }
//        }


    }else{
        printf("not yet implemented and bad performance expected");
    }

//    printf("test 1\n" "\033[0m");

    // shares are prepared for computation
    smpc_options = SMPC_SUM + SMPC_MIN + SMPC_MAX;

//    printf("test 1\n" "\033[0m");

    current_smpc_state = STATE_READY;

//    printf("test 1\n" "\033[0m");

    // try to become coordinator
    become_coordinator();

    if(current_node_state==STATE_COORDINATOR){
        // announce computation group and smpc type
            // receive ack from all nodes



    }else{

    }

    //TODO trigger computation
}


void node_set_discovery_function(void (*func)(char macs[][18], int* result_count)){
    discovery_function_pointer = func;
}


void node_set_await_function(int(*func)(char *source, char response[])){
    await_function_pointer = func;
}


void node_set_send_function(void (*func)(char *target, char *message)){
    send_function_pointer = func;
}

void node_pass_message(char *source, char *message){

    // TODO move to node_pass_message
//        if(strcmp(source, source_read)==0){
//            response = substring(buf,17);
//            break;
//        }

    printf("node received from %s: %s\n",source, message);

    int message_type;
    sscanf(message , "%2u", &message_type );


    if(message_type==BROADCAST){

        int broadcast_type;
        sscanf(&message[2] , "%2u", &broadcast_type );

        char broadcast_response [5]; // ACK>10 -> 2 char
        sprintf(broadcast_response, "%u%u", BROADCAST_RESPONSE, ACK);
        send_message(source, broadcast_response);

//        char response[3] = {BROADCAST_RESPONSE, (char) ACK, '\0'};
//        send_message(source, response);

        if (broadcast_type==DISCOVERY_START_REQUEST) {
            discovery();
        }else if(broadcast_type==GROUP_BROADCAST){
            // store computation group
            int mac_count = (strlen(message)-4)/17;

            computation_group_size = mac_count;

            printf("mac_count=%u %s\n", mac_count, message);

            char mac_in_group[18];

            for (int i = 0; i < mac_count; ++i) {
//                sprintf(mac_in_group, "%17s", &message[i*17+4]);
                strncpy(mac_in_group, &message[i*17+4], 17);
                mac_in_group[17]='\0';
                // replace mac_addresses
                strcpy(mac_adresses[i], mac_in_group);
                printf("cg[%u]: %s\n",i, mac_adresses[i]);
            }
        }

    }else {

        if (message_type==STATE_REQUEST) {

            if (current_smpc_state == STATE_READY) {

                char state_response [4];
                sprintf(state_response, "%u%u", STATE_RESPONSE, smpc_options);
                send_message(source, state_response);
            }

        }else if(message_type==DISCOVERY_REQUEST){

            // read out mac array from coordinator

            //unsigned int ready_nodes_count = char_arr_to_int(message[1], message[2], message[3], message[4]);

            //printf("%i %u\n", (int)message[0], ready_nodes_count);

//            printf("%u->count=%u\n",strlen(message), (strlen(message)-1)/17);

            int mac_count = (strlen(message)-2)/17;

            // compare with own discoverd macs...

            char discovery_response [2+1+mac_count];
            sprintf(discovery_response, "%u", DISCOVERY_RESPONSE);

            for (int i = 0; i < mac_count; ++i) {
                strcat(discovery_response, "1"); // TODO
            }

//            for (int i = 0; i < ready_nodes_count; ++i) {
//                printf("\t%s\n", &message[i*18+5]);
//            }

//            char discovery_response [2];
//            sprintf(discovery_response, "%u", DISCOVERY_RESPONSE);
            send_message(source, discovery_response);

//            char response[3] = {(char) DISCOVERY_RESPONSE, '\0'};
//            send_message(source, response);



            // compare to own

            // reply with array of 0/1



//            char mac_count_str[4];
//            int_to_char_arr(mac_count_str, mac_count);
//
//            char response[2+4+mac_count*18];
//            response[0] = (char) DISCOVERY_RESPONSE;
//
//            strncpy(response[1], mac_count_str, 4);
//
//            for (int i = 0; i < mac_count; ++i) {
//                strncpy(response[i*18+5], mac_adresses[i], 18);
//            }
//
//            send_message(source, response);
        }
    }

    // TODO remove

//    send_message(source, "response");

    //TODO decrypt message

    //TODO handle message

    // TODO use states here to react accordingly
}





void int_to_char_arr(char char_arr[4], unsigned int value){

    for(int i=0; i<4; i++){
        char_arr[i]= (char)((value >> (8*i)) & 0xff);
    }

}

unsigned int char_arr_to_int(char lsb, char lsb2, char msb2, char msb){

    return ((unsigned int)lsb << (8*0)) + ((unsigned int)lsb2 << (8*1))  + ((unsigned int)msb2 << (8*2))  + ((unsigned int)msb << (8*3));

}


// private functions

void become_coordinator(){

    printf("becoming coordinator...");

    // currently hot wired!
    if(current_node_state!=STATE_COORDINATOR){
        printf("denied\n");
        return;
    }

    current_smpc_state = STATE_BUSY;

    // run discovery ~3 seconds
//    (*discovery_function_pointer)(mac_adresses, &mac_count);
    discovery();

    // send all in reach state request

    char ready_nodes[mac_count][18];
    unsigned int ready_nodes_count = 0;

    char state_request [3];
    sprintf(state_request, "%u", STATE_REQUEST);

    clock_t start, end;
    double cpu_time_used;

    // online phase
    start = time(NULL);

    for (int i = 0; i < mac_count; ++i) {
//        char request[2] = {(char)STATE_REQUEST, '\0'};
        send_message(mac_adresses[i], state_request);

        printf("send, going into wait...\n");

        int response_byte_count = (*await_function_pointer)(mac_adresses[i], response_arr);

        printf("\t\treceived response %s\n",response_arr);

        int state;
        sscanf(&response_arr[2] , "%1u", &state );

        if(state!=0){
            // TODO distinction between computations needed eg make three arrays, choose longest
            strcpy(ready_nodes[ready_nodes_count], mac_adresses[i]);
            ready_nodes_count++;
        }
    }

    // +1 since coordinator is also ready
    if(ready_nodes_count+1<CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP){
        // abort, don't become coordinator for now
        return;
    }



    // broadcast a DISCOVERY_START_REQUEST (send request without direct response) ~3 seconds
    // broadcast only waits for ACK
    char request_start_discovery[3];
    sprintf(request_start_discovery, "%u", DISCOVERY_START_REQUEST);
    broadcast(ready_nodes, ready_nodes_count, request_start_discovery);

    // send sequentially: DISCOVERY_REQUEST with own Array of MACs, including own MAC as first entry (response: array of same length with 0/1 if node sees those MACs)

    // combine ready nodes macs to string
    char request_discovery[2+ready_nodes_count*17+1];

    sprintf(request_discovery, "%u", DISCOVERY_REQUEST);

    for (int l = 0; l < ready_nodes_count; ++l) {
        strcat(request_discovery, ready_nodes[l]);
    }

    // send request with list of ready nodes

    int computation_candidates_matrix[ready_nodes_count+1][ready_nodes_count+1];

    for (int i = 0; i < ready_nodes_count+1; ++i) {
        for (int j = 0; j < ready_nodes_count+1; ++j) {
            if(i==j || i==0 || j==0){
                computation_candidates_matrix[i][j] = 1;
            }
        }
    }

    int discovered;
    for (int i = 0; i < ready_nodes_count; ++i) {
        send_message(ready_nodes[i], request_discovery);
        // await response
        (*await_function_pointer)(ready_nodes[i], response_arr);

        printf("received response %s\n",response_arr);

        for (int j = 0; j < ready_nodes_count; ++j) {

            sscanf(&response_arr[j+2] , "%1u", &discovered );

            if(discovered==1){
                computation_candidates_matrix[i+1][j+1] = 1;
            }else{
                computation_candidates_matrix[i+1][j+1] = 0;
            }
        }
    }

    print_matrix(computation_candidates_matrix[0],ready_nodes_count+1);

    // compute group
    int computation_group[ready_nodes_count+1];
    compute_max_computation_group(&computation_candidates_matrix[0][0], ready_nodes_count+1, computation_group);

    printf("group_size=%u\n", computation_group_size);
    print_array(&computation_group[0], computation_group_size);

    // group large enough? Broadcast list of involved parties and SMPC type to run
    char request_group[2+computation_group_size*17+1];

    sprintf(request_group, "%u", GROUP_BROADCAST);

    strcat(request_group, mac_own); // add coordinator as first in list

    for (int l = computation_group_size-2; l >= 0; --l) {
        printf("%u %u \n", l, computation_group[l]);
        printf("%s\n", ready_nodes[computation_group[l]-1]);
        strcat(request_group, ready_nodes[computation_group[l]-1]);
    }

    printf("\tgroup: %s\n", request_group);

    broadcast(ready_nodes, ready_nodes_count, request_group);

    // for each: secure communication with node, get share, give share
        // calculate partial sum s_0
    // pass token to next node, wait for response = partial_sum s_1 to s_n

    // stop when all sums -> lagrange

    // kill all

    end = time(NULL);
    cpu_time_used = (double) (end - start);
    printf("\n\nonline phase: %f seconds \n\n", cpu_time_used);

    char * kill_command = "kill";
    broadcast(mac_adresses, mac_count, kill_command);

    printf("done...\n");

}

void get_involved_parties(int *permutation_matrix, int involved_parties[], int party, int n, int k){

    int index = 0;
    for (int j = 0; j < n; ++j) {
        if(*get_value_pointer(permutation_matrix, n, (party-1), j)==1){
            involved_parties[index]=(j+1);
            index++;
        }
    }
}


void send_message(char *target, char *unencrypted_message){

    //TODO encrypt

    // size + 1 to copy NUL termination for string
    char encrypted_message[strlen(unencrypted_message)+1];
//    char * encrypted_message;

    printf("send message: %u %s", strlen(unencrypted_message), unencrypted_message);

    strcpy(encrypted_message, unencrypted_message);

/*    for (int i = 0; i < strlen(unencrypted_message); ++i) {
        encrypted_message[i]=unencrypted_message[i];
    }*/



    (*send_function_pointer)(target, encrypted_message);
}


void discovery(){
    (*discovery_function_pointer)(mac_adresses, &mac_count);
}


void broadcast(char macs[][18], int count, char *unencrypted_message){

    char message [3+strlen(unencrypted_message)];
    sprintf(message, "%u", BROADCAST);

    strcat(message, unencrypted_message);

    printf("message for broadcast: %s\n",message);

    for (int j = 0; j < count; ++j) {
        if(strcmp(mac_own,macs[j])!=0){

            send_message(macs[j], message);

            int response_byte_count = (*await_function_pointer)(macs[j], response_arr);

            int response_type;
            sscanf(&response_arr[0] , "%2u", &response_type );

            if(response_type==BROADCAST_RESPONSE){
                sscanf(&response_arr[2] , "%2u", &response_type );
                if(response_type==ACK){
                    printf("\tbroadcast ack received\n");
                }
            }
        }
    }

    printf("Broadcast done.\n");
}


/*!
  Computes share matrix based on configurations settings
  \param matrix_start pointer to square matrix
  \param size of the square matrix
  \param threshold k, number of shares needed; k-1 adversaries can be tolerated.
*/
void fill_share_permutation_matrix(int *matrix_start, int size, int threshold){
	int i,j, rowsum;

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

    for (int l = 0; l < computation_group_size; ++l) {
        result[l] = computation_group[l];
    }

    print_array(&computation_group[0], computation_group_size);

}


void compute_max_computation_group_recursion(int *cleaned_matrix_start, int size, int group[], int group_size, int group_index, int current_group[], int current_group_size) {

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
        return;
    }

    int meshed;
    // from last to first
    for (int i = group_index; i > 0; --i) {

        meshed = 1;

        for (int j = 0; j < current_group_size; ++j) {
            if(*get_value_pointer(cleaned_matrix_start, size, group[i-1], current_group[j]) == 0) {
                meshed = 0;
            }
        }

        if(meshed == 1){
            compute_max_computation_group_recursion(cleaned_matrix_start, size, group, group_size, i-1, current_group, current_group_size);
        }
    }
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


//char * concat_str_int(char *str, int value){
//    int length = snprintf(NULL, 0, "%s%i", str, value);
//    char combined[length+1];
//    snprintf(combined, length+1, "%s%i", str, value);
//    return combined;
//}


void node_debug_run_test(char *options){

    printf("options: %s", options);

    if(strcmp("c", options)==0){
        current_node_state = STATE_COORDINATOR;
    }

    // share matrix generation
	/*
	const int k = 5;
	const int n = 15;
	int share_matrix[n][n];
    fill_share_permutation_matrix(&share_matrix[0][0], n, k);
    print_matrix(&share_matrix[0][0], n);
    */

    // computation group computation
//    int computation_candidates_matrix[10][10] = {
//            {1,0,1,0,1,0,1,0,1,0},
//            {0,1,1,1,1,1,1,1,0,0},
//            {0,1,1,1,0,1,0,1,0,0},
//            {1,1,1,1,1,0,1,1,1,1},
//            {1,1,0,1,1,0,1,1,0,1},
//            {0,1,1,0,1,1,0,1,1,0},
//            {1,1,1,1,1,1,1,0,1,1},
//            {1,0,0,1,0,0,1,1,0,1},
//            {1,1,1,0,1,1,0,1,1,1},
//            {1,1,0,1,1,0,1,1,0,1},
//    };
//    int computation_group[10];
//    compute_max_computation_group(&computation_candidates_matrix[0][0], 10, computation_group);


// computation group for random matrix 100 node and max group of 20 ~ 9 seconds
//    int a = 100;
//    int random_value;
//    int computation_candidates_matrix_new[a][a];
//    for (int i = 0; i < a; ++i) {
//        for (int j = 0; j < a; ++j) {
//            if(i==j){
//                computation_candidates_matrix_new[i][j] = 1;
//            }else{
//                random_value = rand();
//                computation_candidates_matrix_new[i][j] = (random_value%2 || random_value%3);
//            }
//        }
//    }
//    int computation_group_new[a];
//    compute_max_computation_group(&computation_candidates_matrix_new[0][0], a, computation_group_new);

    // test discovery function pointer
//    (*discovery_function_pointer)(mac_adresses, &mac_count);
//
//    for (int l = 0; l < mac_count; ++l) {
//        printf("%s\n",mac_adresses[l]);
//    }

    // test send message function pointer
//    printf("lib sending.\n\t");
//    send_message("11:22:33:44:55:66", "some encrypted message to the receiver");

}

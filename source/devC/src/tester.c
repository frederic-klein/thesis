//
// Created by uni on 12/26/16.
//

#include <stdio.h>
#include "node.h"

// example implementations
void host_function_discovery(char macs[][18], int* result_count){

    *result_count = 8;

    char results[8][18] = {
            "11:AA:AA:AA:AA:AA",
            "22:AA:AA:AA:AA:AA",
            "33:AA:AA:AA:AA:AA",
            "44:AA:AA:AA:AA:AA",
            "55:AA:AA:AA:AA:AA",
            "66:AA:AA:AA:AA:AA",
            "77:AA:AA:AA:AA:AA",
            "88:AA:AA:AA:AA:AA"
    };

    for (int j = 0; j < *result_count; ++j) {
        for (int i = 0; i < 18; ++i) {
            macs[j][i]=results[j][i];
        }
    }
}


void host_function_send_message(char *target, char *message ){

    printf("host sends: %s:%s\n", target, message);

}



int main(){

    // host needs to privide the function pointer to call the discovery of devices
    node_set_discovery_function(&host_function_discovery);
    node_set_send_function(&host_function_send_message);

    node_init();

    int score = 1500;
//    int score = 8;

    node_pass_score(score);

    node_pass_message("88:77:66:55:44:33","Encrypted message for node.");


    // test functionality
//    node_debug_run_test();

}
//
// Created by uni on 12/26/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void host_function_send_message(char *target, char *message );
void host_function_discovery(char macs[][18], int* result_count);
char * host_function_await_response(char *source);

// helper
void host_function_receive();
char * substring(char *string, int position);

char *mac;
int score;


// example implementations

// discovery function: list and number of MACs in reach
void host_function_discovery(char macs[][18], int* result_count){

    sleep(3);

    FILE *fp;
    char buff[1024];

    fp = fopen("/tmp/discovery", "r");

    int mac_count=0;

    while(fgets(buff, 1024, (FILE*)fp) != 0){
        mac_count++;
    }

    //contains also self -> reduce by one
    mac_count--;

    fclose(fp);

//    char results[mac_count][18];

    fp = fopen("/tmp/discovery", "r");

    int index=0;
    while(fgets(buff, 1024, (FILE*)fp) != 0){
        // string contains new line
        strtok(buff, "\n");

        if(strcmp(buff, mac)!=0) {
            strcpy(macs[index], buff);
            index++;
        }
    }

    fclose(fp);

    *result_count = mac_count;

}


void host_function_send_message(char *target, char *message ){
    printf("host sends: %s:%s\n", target, message);

    int fd;

    char myfifo[22];
    strcpy(myfifo, "/tmp/");
    strcat(myfifo, target);

    char combined[strlen(mac)+strlen(message)+1];
    strcpy(combined, mac);
    strcat(combined, message);

    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_WRONLY);
    write(fd, combined, strlen(combined)+1);
    close(fd);

//    /* remove the FIFO */
//    unlink(myfifo);

}

char * substring(char *string, int position){
    return &string[position];
}


void host_function_receive(){

    int fd;
    char buf[1024];

    char myfifo[22];
    strcpy(myfifo, "/tmp/");
    strcat(myfifo, mac);

    char *response;

    int read_value;
    while(1){

        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_RDONLY);
        read_value = read(fd, buf, 1024);

        if(read_value<1) continue;
//        printf("%i\n",read_value);

        char source_read[18];
        for (int i = 0; i < 17; ++i) {
            source_read[i]=buf[i];
        }
        source_read[17]='\0';

        close(fd);

        response = substring(buf,17);
        if(strcmp(&response[1], "kill")==0){
//            printf("kill-request received...\n");
            return;
        }
        node_pass_message(source_read, response);


    }
}

char * host_function_await_response(char *source){

    int fd;
    char buf[1024];

    char myfifo[22];
    strcpy(myfifo, "/tmp/");
    strcat(myfifo, mac);

    char *response;

    int read_value;
    while(1){

        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_RDONLY);
        read_value = read(fd, buf, 1024);

        if(read_value<1) continue;

        char source_read[18];
        for (int i = 0; i < 17; ++i) {
            source_read[i]=buf[i];
        }
        source_read[17]='\0';

        close(fd);

        if(strcmp(source, source_read)==0){
            response = substring(buf,17);
            break;
        }
    }

    return response;
}



int main(int argc, char *argv[] ){

    // parameters: own MAC, secret score, role [c or n]
    mac = argv[1];
    char *ptr;
    score = (int)strtol(argv[2], &ptr, 10);

    // host needs to privide the function pointer to call the discovery of devices
    node_set_discovery_function(&host_function_discovery);
    node_set_await_function(&host_function_await_response);
    node_set_send_function(&host_function_send_message);

    node_init(mac);

    node_debug_run_test(argv[3]); // set coordinator/node option

    node_pass_score(score);

    if(strcmp("c", argv[3])!=0){
        // normal node goes into receiving mode
        host_function_receive();
    }

}
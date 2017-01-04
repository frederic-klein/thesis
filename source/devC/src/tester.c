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

void host_function_send_message(char *target, char *message);
void host_function_discovery(char macs[][18], int* result_count);
int host_function_await_response(char *source, char response[]);

// helper
void host_function_receive();
char * substring(char *string, int position);

char *mac;
int score;


// example implementations

// discovery function: list and number of MACs in reach
void host_function_discovery(char macs[][18], int* result_count){

    printf("%s running discovery...", mac);

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

//        printf("in buff: %s\n", buff);

        if(strncmp(buff, mac, 17)!=0) {
//            printf("%s = %s?\n", mac, buff);
            strncpy(macs[index], buff, 17);
            macs[index][17]='\0';
            index++;
        }
    }

    fclose(fp);

    *result_count = mac_count;

    printf(" done.\n");

}


void host_function_send_message(char *target, char *message){

    printf("host %s sends to %s: %s\n", mac, target, message);

//    for (int i = 0; i < message_length; ++i) {
        //printf("%c",message[i]);
//    }

    int fd;

//    printf(" 00 ");

    char myfifo[23];

    strcpy(myfifo, "/tmp/");
    strcat(myfifo, target);

//    printf(" 0 %s %u", myfifo, strlen(myfifo));

    //int combined_length = strlen(mac)+strlen(message)+1;
    char combined[strlen(mac)+strlen(message)+1];

//    printf(" 1 ");

    strcpy(combined, mac);

//    printf(" 2 ");

    strcat(combined, message);

//    printf(" 3 ");

//    sleep(1);
    int sleep_duration = 20*strlen(message);
    if(sleep_duration<1000){
        usleep(1000);
    }else{
        usleep(sleep_duration);
    }

    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_WRONLY);

//    printf(" 4 %u", fd);

//    printf(" %s %u", combined, strlen(combined));

    if(combined[strlen(combined)]!='\0'){
        printf("not zero terminated \n");
    }


    //TODO check response

    write(fd, combined, strlen(combined)+1);
    close(fd);

    printf(" 5 ");

//    /* remove the FIFO */
//    unlink(myfifo);

}

char * substring(char *string, int position){
    return &string[position];
}


void host_function_receive(){

    int fd;
    char buf[1024];

    char myfifo[23];
    strcpy(myfifo, "/tmp/");
    strcat(myfifo, mac);

//    char *response;

    int read_value;
    while(1){

        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_RDONLY);
        read_value = read(fd, buf, 1024); // should be enough for 100 macs

        if(read_value<1) continue;

//        printf("%i\n",read_value);

        int mac_length = 18;

        char source_read[mac_length];
        strncpy(source_read, buf, mac_length-1);

        source_read[mac_length-1]='\0';

        close(fd);

        int message_length = read_value-(mac_length-1);

        char response[message_length];

        for(int i=0; i<message_length;i++){
            response[i]=buf[mac_length-1+i];
        }

        printf("%s received from %s %s\n", mac, source_read, response);

//        printf("received %i ",(int)response[0]);
//        for(int i=1; i<message_length;i++){
//            printf("%c",response[i]);
//        }

        if(strcmp(&response[2], "kill")==0){
            node_pass_message(source_read, response);
            return;
        }
        node_pass_message(source_read, response);
    }
}

int host_function_await_response(char *source, char response[]){

    int fd;
    char buf[1024];

    char myfifo[23];
    strcpy(myfifo, "/tmp/");
    strcat(myfifo, mac);

//    char * response;

    int read_value;
    while(1){

        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_RDONLY);
        read_value = read(fd, buf, 1024);

        if(read_value<1) continue;


        int mac_length = 18;

        char source_read[mac_length];
        strncpy(source_read, buf, mac_length-1);

        source_read[mac_length-1]='\0';

        close(fd);




/*        int mac_length = 18;

        char source_read[mac_length];

        for (int i = 0; i < mac_length; ++i) {
            source_read[i]=buf[i];
        }
        //source_read[17]='\0';

        close(fd);*/

        if(strcmp(source, source_read)==0){

            int message_length = read_value-(mac_length-1);

//            char response[message_length];

            for(int i=0; i<message_length;i++){
                response[i]=buf[mac_length-1+i];
            }

            printf("%s received from %s %s\n", mac, source_read, response);
/*

            int message_length = read_value-mac_length;

            char response[message_length];

            for(int i=0; i<message_length;i++){
                response[i]=buf[mac_length+i];
            }

            printf("received %i ",(int)response[0]);
            for(int i=1; i<message_length;i++){
                printf("%c",response[i]);
            }*/

            return message_length;

            //break;
        }
    }

//    return response;
}



int main(int argc, char *argv[] ){

    printf("arguments: %s %s %s", argv[1], argv[2], argv[3]);

    // parameters: own MAC, secret score, role [c or n]
    mac = argv[1];
    char *ptr;
    score = (int)strtol(argv[2], &ptr, 10);

    // host needs to provide the function pointer to call the discovery of devices
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
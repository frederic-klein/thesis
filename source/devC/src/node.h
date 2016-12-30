/*! \file node.h
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

#ifndef DEVC_NODE_H
#define DEVC_NODE_H

    void node_init(char *mac);

    void node_pass_score(int score /**< New score provided by the host system. */ ); //!< function to pass new score

    void node_pass_message(char *source, char *message); //!< pass message from host to lib

    void node_set_discovery_function(void(*func)(char macs[][18], int* result_count));

    void node_set_send_function(void(*func)(char *target, char *message));

    void node_set_await_function(char * (*func)(char * source));


    void node_debug_run_test(char *options);

#endif //DEVC_NODE_H

#include <stdio.h>
#include <stdbool.h>
#include <libpq-fe.h>


#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

typedef struct mqconsumer_s {
    amqp_connection_state_t conn;
    PGconn *connect;
} mqconsumer_t;

mqconsumer_t * mqconsumer_init();
bool mqconsumer_inject(mqconsumer_t * handle, char * message);
bool mqconsumer_free(mqconsumer_t * handle);

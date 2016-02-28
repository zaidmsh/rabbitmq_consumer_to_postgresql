#include <stdio.h>
#include <stdbool.h>
#include <libpq-fe.h>


#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

typedef struct mq_s {
    amqp_connection_state_t conn;
} mq_t;

mq_t * mq_init();
bool mq_free(mq_t * handle);

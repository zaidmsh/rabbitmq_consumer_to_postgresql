#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <libpq-fe.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

typedef struct mq_s {
    amqp_connection_state_t conn;
} mq_t;

mq_t * mq_init();
uint32_t mq_consume(mq_t * handle, char * buffer, uint32_t buflen);
bool mq_free(mq_t * handle);

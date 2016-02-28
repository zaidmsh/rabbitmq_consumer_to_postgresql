#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "utils.h"
#include "mq.h"

mq_t * mq_init() {
    mq_t * mq;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    uint16_t  status;

    mq = calloc(1, sizeof(mq_t));
    // initializing a connection for the MQ

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        die("creating TCP socket");
    }

    status = amqp_socket_open(socket, "localhost", 5672);
    if (status) {
        die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
                    "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

    mq->conn = conn;

    return mq;
}
bool mq_consume(mq_t * handle, char * buffer){
    amqp_connection_state_t conn;
    amqp_envelope_t envelope;

    conn = handle->conn;

    amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    return true;
}
bool mq_free(mq_t * handle) {

    die_on_amqp_error(amqp_channel_close(handle->conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(handle->conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(handle->conn), "Ending connection");

    free(handle);
    return true;
}

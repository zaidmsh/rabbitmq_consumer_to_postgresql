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
    uint16_t  port, status;
    char const *host;
    char const * queuename;

    mq = calloc(1, sizeof(mq_t));
    // initializing a connection for the MQ
    host = "localhost";
    queuename = "hello";
    port = 5672;
    mq->conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(mq->conn);
    if (!socket) {
        die("creating TCP socket");
    }

    status = amqp_socket_open(socket, host, port);
    if (status) {
        die("opening TCP socket");
    }
    die_on_amqp_error(amqp_login(mq->conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
                    "Logging in");
    amqp_channel_open(mq->conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(mq->conn), "Opening channel");

    amqp_queue_declare(mq->conn, 1, amqp_cstring_bytes(queuename), 0, 0, 0, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(mq->conn), "Declaring queue");

    amqp_basic_consume(mq->conn, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

    return mq;
}
uint32_t mq_consume(mq_t * handle, char * buffer, uint32_t buflen){
    amqp_envelope_t envelope;
    amqp_rpc_reply_t ret;

    amqp_maybe_release_buffers(handle->conn);
    ret = amqp_consume_message(handle->conn, &envelope, NULL, 0);

    if (AMQP_RESPONSE_NORMAL == ret.reply_type) {
        memcpy(buffer, envelope.message.body.bytes, envelope.message.body.len);
        amqp_destroy_envelope(&envelope);
        return envelope.message.body.len;
    }
    return 0;
}

bool mq_free(mq_t * handle) {

    die_on_amqp_error(amqp_channel_close(handle->conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(handle->conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(handle->conn), "Ending connection");

    free(handle);
    return true;
}

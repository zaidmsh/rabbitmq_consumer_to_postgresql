#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "utils.h"
#include "mqconsumer.h"

mqconsumer_t * mqconsumer_init() {
    mqconsumer_t * mq;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    uint16_t port, status;

    mq = calloc(1, sizeof(mqconsumer_t));
    // initializing a connection for the MQ
    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
      die("creating TCP socket");
    }
    port = 5432;
    status = amqp_socket_open(socket, "localhost", port);
    if (status) {
      die("opening TCP socket");
    }
    amqp_channel_open(conn, 1);
    mq->conn = conn;

    // connecting to th PostgreSQL DB

    mq->connect = PQconnectdb("postgresql://zaid@localhost/zaid");

        if (PQstatus(mq->connect) == CONNECTION_BAD) {
            fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(mq->connect));
            PQfinish(mq->connect);
            free(mq);
            return NULL;
        }

    return mq;
}

bool mqconsumer_inject(mqconsumer_t * handle, char * message){
    amqp_connection_state_t conn = handle->conn;
    amqp_bytes_t message_bytes;
    PGresult   *res;
    const char *paramValues[1];

    message_bytes.len = strlen(message);
    message_bytes.bytes = message;
    // receiving from the queue
    amqp_basic_consume(conn, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    paramValues[0] = message;
    res = PQexecParams(handle->connect,
                       "INSERT INTO logs (message) VALUES ($1)",
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       paramValues,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       1);      /* ask for binary results */

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Insert Failed : %s", PQerrorMessage(handle->connect));
        PQclear(res);
        return false;
    }
    return true;
}

bool mqconsumer_free(mqconsumer_t * handle) {

    die_on_amqp_error(amqp_channel_close(handle->conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(handle->conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(handle->conn), "Ending connection");

    free(handle);
    return true;
}

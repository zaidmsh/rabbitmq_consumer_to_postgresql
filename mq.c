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
    amqp_rpc_reply_t ret;
    char * buffer;

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
bool mq_consume(mq_t * handle, char * buffer){
    amqp_envelope_t envelope;
    amqp_rpc_reply_t ret;
    amqp_frame_t frame;


    amqp_maybe_release_buffers(handle->conn);
    printf("consuming\n");
    ret = amqp_consume_message(handle->conn, &envelope, NULL, 0);
    printf("consumed\n");
    if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
      if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
          AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
        if (AMQP_STATUS_OK != amqp_simple_wait_frame(handle->conn, &frame)) {
          return false;
        }

        if (AMQP_FRAME_METHOD == frame.frame_type) {
          switch (frame.payload.method.id) {
            case AMQP_BASIC_ACK_METHOD:
              /* if we've turned publisher confirms on, and we've published a message
               * here is a message being confirmed
               */

              break;
            case AMQP_BASIC_RETURN_METHOD:
              /* if a published message couldn't be routed and the mandatory flag was set
               * this is what would be returned. The message then needs to be read.
               */
              {
                amqp_message_t message;
                ret = amqp_read_message(handle->conn, frame.channel, &message, 0);
                if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                  return false;
                }
                buffer = (char *) &message.body.bytes;
                printf("%s\n", buffer);
                amqp_destroy_message(&message);
              }

              break;

            case AMQP_CHANNEL_CLOSE_METHOD:
              /* a channel.close method happens when a channel exception occurs, this
               * can happen by publishing to an exchange that doesn't exist for example
               *
               * In this case you would need to open another channel redeclare any queues
               * that were declared auto-delete, and restart any consumers that were attached
               * to the previous channel
               */
              return false;

            case AMQP_CONNECTION_CLOSE_METHOD:
              /* a connection.close method happens when a connection exception occurs,
               * this can happen by trying to use a channel that isn't open for example.
               *
               * In this case the whole connection must be restarted.
               */
              return false;

            default:
              fprintf(stderr ,"An unexpected method was received %d\n", frame.payload.method.id);
              return false;
          }
        }
      }

    } else {
        buffer = (char *) &envelope.message.body.bytes;
        printf("%s\n", buffer);
        amqp_destroy_envelope(&envelope);
    }

    amqp_destroy_envelope(&envelope);
    return true;
}

bool mq_free(mq_t * handle) {

    die_on_amqp_error(amqp_channel_close(handle->conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(handle->conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(handle->conn), "Ending connection");

    free(handle);
    return true;
}

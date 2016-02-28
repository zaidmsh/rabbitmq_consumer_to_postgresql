#include <stdio.h>
#include <stdbool.h>
#include <libpq-fe.h>

typedef struct pg_s {
    PGconn *conn;
} pg_t;

pg_t * pg_init();
bool pg_inject(pg_t * handle, char * message);
bool pg_free(pg_t * handle);

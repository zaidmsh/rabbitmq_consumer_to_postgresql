#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <libpq-fe.h>

#include "user.h"

typedef struct pg_s {
    PGconn *conn;
} pg_t;

pg_t * pg_init();
bool pg_inject(pg_t * handle, const user_t * user);
bool pg_free(pg_t * handle);

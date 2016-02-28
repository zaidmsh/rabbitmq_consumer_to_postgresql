#include <stdlib.h>
#include <libpq-fe.h>

#include "pg.h"

pg_t * pg_init(){
    pg_t * pg;
    PGconn *conn;

    pg = calloc(1, sizeof(pg_t));
    conn = PQconnectdb("postgresql://zaid@localhost/zaid");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "connion to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        free(pg);
        return NULL;
    }
    pg->conn = conn;
    return pg;
}

bool pg_inject(pg_t * handle, char * message){
    return true;
}

bool pg_free(pg_t * handle){
    free(handle);
    return true;
}

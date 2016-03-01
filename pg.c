#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "pg.h"

pg_t * pg_init(){
    pg_t * pg;

    pg = calloc(1, sizeof(pg_t));
    pg->conn = PQconnectdb("postgresql://vagrant:123456@localhost/vagrant");

    if (PQstatus(pg->conn) == CONNECTION_BAD) {
        fprintf(stderr, "connection to database failed: %s\n", PQerrorMessage(pg->conn));
        PQfinish(pg->conn);
        free(pg);
        return NULL;
    }

    return pg;
}

bool pg_inject(pg_t * handle, const user_t * user){

    PGresult *res;

    int temp_age = htonl(user->age);
    char const * values[] = {user->username, (char *) &temp_age};
    int lengths[2] = {strlen(user->username), sizeof(int)};
    int binary[2] = {0, 1};

    res = PQexecParams(handle->conn,
                       "INSERT INTO users (username, age) VALUES ($1, $2)",
                       2,       /* two params */
                       NULL,    /* let the backend deduce param type */
                       values,
                       lengths,    /* don't need param lengths since text */
                       binary,    /* default to all text params */
                       0);      /* ask for binary results */

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Insert Failed : %s", PQerrorMessage(handle->conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool pg_free(pg_t * handle){
    PQfinish(handle->conn);
    free(handle);
    return true;
}

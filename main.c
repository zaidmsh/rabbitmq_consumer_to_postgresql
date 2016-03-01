#include <stdio.h>
#include <string.h>

#include <jansson.h>

#include "mq.h"
#include "pg.h"

void json_parsing(char * buf, uint32_t buflen, user_t * user){
    json_t *root;
    json_error_t error;
    char * temp_username;
    double temp_age;

    user->age = 0;
    root = json_loads(buf, buflen, &error);
    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return;
    }

    if(!json_is_object(root))
    {
        fprintf(stderr, "error: root is not an Object\n");
        json_decref(root);
        return;
    }
    if (json_unpack_ex(root, &error, 0, "{s:s, s:F}", "username", &temp_username, "age", &temp_age) == -1){
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    }
    user->age = (int) temp_age;
    strcpy(user->username, temp_username);

    json_decref(root);
}

int main(){
    mq_t * mq;
    pg_t * pg;
    char buf[1024];
    uint32_t buflen;
    user_t user;

    mq = mq_init();
    pg = pg_init();

    while(true){
        buflen = 1024;
        buflen = mq_consume(mq, buf, buflen);
        json_parsing(buf, buflen, &user);
        pg_inject(pg, &user);
        printf("Inserting\n");
    }

    mq_free(mq);
    pg_free(pg);
    return 0;
}

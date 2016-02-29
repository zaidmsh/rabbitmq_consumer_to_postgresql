#include <stdio.h>

#include "mq.h"
#include "pg.h"

int main(){
    mq_t * mq;
    pg_t * pg;
    char * buf = "";

    mq = mq_init();
    pg = pg_init();

    mq_consume(mq, buf);

     return 0;
}

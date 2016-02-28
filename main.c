#include <stdio.h>

#include "mq.h"
#include "pg.h"

int main(){
    mq_t * mq;
    pg_t * pg;

    mq = mq_init();
    pg = pg_init();

    printf("Hello World!\n");
    return 0;
}

#include <stdio.h>

#include "mqconsumer.h"

int main(){
    mqconsumer_t * handle;

    handle = mqconsumer_init();
    
    printf("Hello World!\n");
    return 0;
}

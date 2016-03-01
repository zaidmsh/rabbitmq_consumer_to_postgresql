#include <stdio.h>
#include <stdint.h>

struct user_s {
    int age;
    char username[1024];
};
typedef struct user_s user_t;

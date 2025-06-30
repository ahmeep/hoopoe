#ifndef HOOPOE_SERVER_USER_MANAGER_H_
#define HOOPOE_SERVER_USER_MANAGER_H_

#include <stdint.h>

struct hoopoe_user {
    int fd;
    char *name;
    uint64_t ping;
    uint64_t last_pinged;
};

bool hoopoe_add_user(int fd, struct hoopoe_user **puser);
bool hoopoe_remove_user(int fd);
struct hoopoe_user *hoopoe_get_user(int fd);
struct hoopoe_user **hoopoe_get_users(uint32_t *psize);

#endif // HOOPOE_SERVER_USER_MANAGER_H_

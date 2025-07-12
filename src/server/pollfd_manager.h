#ifndef HOOPOE_SERVER_POLLFD_MANAGER_H_
#define HOOPOE_SERVER_POLLFD_MANAGER_H_

#include <poll.h>
#include <stdint.h>

#include "user.h"

bool hoopoe_add_pollfd(int fd, short events);
bool hoopoe_remove_pollfd(int fd);
/* return pointer to the array to prevent use after free/realloc */
struct pollfd **hoopoe_get_pollfds(uint32_t *size);

#endif // HOOPOE_SERVER_POLLFD_MANAGER_H_

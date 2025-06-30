#ifndef HOOPOE_SERVER_POLLFD_MANAGER_H_
#define HOOPOE_SERVER_POLLFD_MANAGER_H_

#include <poll.h>
#include <stdint.h>

#include "user.h"

bool hoopoe_add_pollfd(int fd, short events);
bool hoopoe_remove_pollfd(int fd);
void hoopoe_get_pollfds(struct pollfd **ppollfds, uint32_t *psize);

#endif // HOOPOE_SERVER_POLLFD_MANAGER_H_

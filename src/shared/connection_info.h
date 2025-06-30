#ifndef HOOPOE_CONNECTION_INFO_H_
#define HOOPOE_CONNECTION_INFO_H_

#include <stdbool.h>
#include <stdint.h>
struct addrinfo;

bool hoopoe_get_connection_info(char **hostname, uint16_t *port);
bool hoopoe_get_addrinfo(char *hostname, uint16_t port,
                         struct addrinfo **addrinfo);

#endif // HOOPOE_CONNECTION_INFO_H_

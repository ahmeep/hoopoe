#include "connection_info.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../constants.h"
#include "../options.h"

static bool parse_port(const char *str, uint16_t *port)
{
    char *endptr;
    long port_to_be = strtol(str, &endptr, 10);
    if (endptr == NULL || *endptr != 0 || port_to_be < 0
        || port_to_be > UINT16_MAX) {
        printf("Invalid port %s\n", str);
        return false;
    }
    *port = (uint16_t)port_to_be;
    return true;
}

bool hoopoe_get_connection_info(char **hostname, uint16_t *port)
{
    /*
     * connection string can be in 3 forms:
     * hostname:port
     * hostname
     * port
     */

    // Defaults
    *hostname = NULL;
    *port = DEFAULT_PORT;

    const char *connection_string = hoopoe_connection_string();

    if (connection_string == NULL)
        return true;

    char *p_colon = strchr(connection_string, ':');
    if (p_colon == NULL) {
        if (strspn(connection_string, "0123456789")
            == strlen(connection_string)) { // 'port' case
            return parse_port(connection_string, port);
        } else { // 'hostname' case
            *hostname = strdup(connection_string);
            return true;
        }
    } else { // 'hostname:port' case
        size_t hostname_size = (p_colon - connection_string) * sizeof(char);
        *hostname = malloc(hostname_size);
        memcpy(*hostname, connection_string, hostname_size);

        return parse_port(p_colon + 1, port);
    }
}

bool hoopoe_get_addrinfo(char *hostname, uint16_t port,
                         struct addrinfo **addrinfo)
{
    static char port_str[6] = {0};
    memset(port_str, 0, 6);
    sprintf(port_str, "%u", port);

    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int ret = getaddrinfo(hostname, port_str, &hints, addrinfo);

    if (ret != 0) {
        printf("An error occurred while getting addrinfo: %s\n",
               gai_strerror(ret));
        return false;
    }

    return true;
}

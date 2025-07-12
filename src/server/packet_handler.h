#ifndef HOOPOE_SERVER_PACKET_HANDLER_H_
#define HOOPOE_SERVER_PACKET_HANDLER_H_

#include <stdbool.h>

#include "../shared/packets.h"
#include "user.h"

bool hoopoe_server_handle_packet(int fd, hoopoe_packet_type type,
                                 struct hoopoe_packet_data data);

#endif // HOOPOE_SERVER_PACKET_HANDLER_H_

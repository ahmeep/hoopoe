#ifndef HOOPOE_SERVER_PACKET_HANDLER_H_
#define HOOPOE_SERVER_PACKET_HANDLER_H_

#include <stdbool.h>

#include "../shared/packets.h"
#include "user.h"

bool hoopoe_server_handle_packet(struct hoopoe_user *sender,
                                 struct hoopoe_packet packet);

#endif // HOOPOE_SERVER_PACKET_HANDLER_H_

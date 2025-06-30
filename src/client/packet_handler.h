#ifndef HOOPOE_CLIENT_PACKET_HANDLER_H_
#define HOOPOE_CLIENT_PACKET_HANDLER_H_

#include <stdbool.h>

#include "../shared/packets.h"

bool hoopoe_client_handle_packet(int sockfd, struct hoopoe_packet packet);

#endif // HOOPOE_CLIENT_PACKET_HANDLER_H_

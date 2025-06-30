#include <stdio.h>

#include "options.h"
#include "server/server.h"
#include "client/client.h"

int main(int argc, char **argv)
{
    hoopoe_set_options(argc, argv);

    if (hoopoe_is_server())
        hoopoe_server_start();
    else
        hoopoe_client_start();

    return 0;
}

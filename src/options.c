#include "options.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

static int is_server = 0;
static char *connection_string = NULL;
static char *name = "Hoopoe";

static struct option options[] = {
    {"help", no_argument, 0, 'h'},
    {"server", no_argument, &is_server, 1},
    {"name", required_argument, 0, 'n'},
};

void hoopoe_set_options(int argc, char **argv)
{
    int opt;
    while ((opt = getopt_long(argc, argv, "hn:", options, 0)) != -1) {
        switch (opt) {
        case 'h':
            printf("usage: %s [<options>] [host:port]\n", argv[0]);
            printf("\n");
            printf("Options:\n");
            printf("   -h --help      Show this and exit.\n");
            printf("   -n --name      Set name (default: Hoopoe).\n");
            printf("   --server       Start a server.\n");
            printf("\n");
            printf(
                "If host and/or port are not specified,\n"
                "the defaults (localhost:%u) will be used.\n",
                DEFAULT_PORT);
            exit(0);
            break;
        case 'n':
            name = strdup(optarg);
            break;
        }
    }

    if (optind < argc) {
        connection_string = strdup(argv[optind]);
    }
}

bool hoopoe_is_server()
{
    return is_server == 1;
}

const char *hoopoe_name()
{
    return name;
}

const char *hoopoe_connection_string()
{
    return connection_string;
}

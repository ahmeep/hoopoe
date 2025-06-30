#ifndef HOOPOE_OPTIONS_H_
#define HOOPOE_OPTIONS_H_

#include <stdbool.h>

void hoopoe_set_options(int argc, char **argv);

bool hoopoe_is_server();
const char *hoopoe_name();
const char *hoopoe_connection_string();

#endif // HOOPOE_OPTIONS_H_

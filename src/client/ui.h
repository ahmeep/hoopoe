#ifndef HOOPOE_CLIENT_UI_H_
#define HOOPOE_CLIENT_UI_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

struct hoopoe_ui_context {
    bool running;

    char *message_queue[10];
    uint32_t message_queue_size;

    char **shown_messages;
    uint32_t max_shown;
    uint32_t shown_size;

    pthread_mutex_t ui_mutex;
};

bool hoopoe_ui_start(struct hoopoe_ui_context **pcontext);
void hoopoe_ui_new_message(const char *message);
void hoopoe_ui_finish();

void hoopoe_ui_set_server_name(const char *server_name);
void hoopoe_ui_set_ping(uint64_t ping);

#endif // HOOPOE_CLIENT_UI_H_

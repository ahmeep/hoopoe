#include "ui.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../options.h"

static pthread_t ui_thread;
static struct hoopoe_ui_context context;

void *ui_routine(void *)
{
    initscr();

    context.max_shown = LINES - 2;
    context.shown_messages = calloc(context.max_shown, sizeof(char *));
    context.running = true;

    char message_buffer[101];

    while (context.running) {
        move(LINES - 2, 0);
        clrtoeol();
        addstr("chat > ");
        refresh();
        memset(message_buffer, 0, 101);
        getnstr(message_buffer, 100);
        if (strlen(message_buffer) == 0)
            continue;

        pthread_mutex_lock(&context.ui_mutex);
        if (context.message_queue_size < 10) {
            context.message_queue[context.message_queue_size++] =
                strdup(message_buffer);
        }
        pthread_mutex_unlock(&context.ui_mutex);
    }
    endwin();
    return NULL;
}

bool hoopoe_ui_start(struct hoopoe_ui_context **pcontext)
{
    *pcontext = &context;

    pthread_mutex_init(&context.ui_mutex, NULL);
    if (pthread_create(&ui_thread, NULL, ui_routine, NULL) != 0)
        return false;

    return true;
}

void hoopoe_ui_new_message(const char *message)
{
    if (!context.running)
        return;

    if (context.shown_size < context.max_shown) {
        context.shown_messages[context.shown_size++] = strdup(message);
    } else {
        free(context.shown_messages[0]);
        memmove(context.shown_messages, context.shown_messages + 1,
                (context.shown_size - 1) * sizeof(char *));
        context.shown_messages[context.shown_size - 1] = strdup(message);
    }

    int y, x;
    getyx(stdscr, y, x);
    for (uint32_t i = 0; i < context.shown_size; i++) {
        move(i, 0);
        clrtoeol();
        addstr(context.shown_messages[i]);
    }
    move(y, x);
    refresh();
}

void hoopoe_ui_finish()
{
    hoopoe_ui_new_message("connection closed. press enter to quit.");
    context.running = false;
    pthread_join(ui_thread, NULL);
    pthread_mutex_destroy(&context.ui_mutex);
}

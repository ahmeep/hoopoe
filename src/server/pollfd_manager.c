#include "pollfd_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct pollfd *pollfds = NULL;
static uint32_t pollfds_capacity = 0;
static uint32_t pollfds_size = 0;

bool find_index(int fd, uint32_t *pindex)
{
    for (uint32_t i = 0; i < pollfds_size; i++) {
        if (pollfds[i].fd == fd) {
            *pindex = i;
            return true;
        }
    }

    return false;
}

bool hoopoe_add_pollfd(int fd, short events)
{
    if (pollfds_capacity == pollfds_size) {
        pollfds =
            realloc(pollfds, (pollfds_capacity + 1) * sizeof(struct pollfd));
        if (pollfds == NULL) {
            perror("cannot realloc pollfds");
            return false;
        }
        pollfds_capacity++;
    }
    pollfds[pollfds_size].fd = fd;
    pollfds[pollfds_size].events = events;
    pollfds_size++;

    return true;
}

bool hoopoe_remove_pollfd(int fd)
{
    if (pollfds_size > 0 && pollfds[pollfds_size - 1].fd == fd) {
        pollfds_size--; // last element, no need for moving elements.
        return true;
    }

    uint32_t index;
    if (!find_index(fd, &index))
        return false;

    uint32_t last = pollfds_size - 1;

    // just put the last one here.
    memcpy(pollfds + index, pollfds + last, sizeof(struct pollfd));
    pollfds_size--;

    return true;
}

struct pollfd **hoopoe_get_pollfds(uint32_t *size)
{
    *size = pollfds_size;
    return &pollfds;
}

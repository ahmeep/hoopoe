#include "user.h"

#include <stdlib.h>
#include <string.h>

struct hoopoe_user_node {
    struct hoopoe_user user;
    struct hoopoe_user_node *next;
};

static struct hoopoe_user_node *head = NULL;
static uint32_t node_count = 0;

bool hoopoe_add_user(int fd, struct hoopoe_user **puser)
{
    struct hoopoe_user_node *node = malloc(sizeof(struct hoopoe_user_node));
    if (node == NULL)
        return false;
    memset(node, 0, sizeof(struct hoopoe_user_node));
    node->user.fd = fd;

    if (puser != NULL)
        *puser = &node->user;

    node_count++;

    if (head == NULL) {
        head = node;
        return true;
    }

    struct hoopoe_user_node *last = head;
    while (last->next != NULL)
        last = last->next;

    last->next = node;
    return true;
}

bool hoopoe_remove_user(int fd)
{
    // todo: free user data inside node after they get removed
    if (head->user.fd == fd) {
        struct hoopoe_user_node *next = head->next;
        free(head);
        head = next;
        node_count--;
        return true;
    }

    struct hoopoe_user_node *previous = head;
    while (previous->next != NULL && previous->next->user.fd != fd)
        previous = previous->next;

    if (previous->next == NULL) // user not found!
        return false;

    struct hoopoe_user_node *removing = previous->next;
    previous->next = removing->next;
    free(removing);
    node_count--;
    return true;
}

struct hoopoe_user *hoopoe_get_user(int fd)
{
    struct hoopoe_user_node *node = head;
    while (node != NULL && node->user.fd != fd)
        node = node->next;

    if (node == NULL)
        return NULL;

    return &node->user;
}

struct hoopoe_user **hoopoe_get_users(uint32_t *psize)
{
    *psize = node_count;
    struct hoopoe_user **users =
        calloc(node_count, sizeof(struct hoopoe_user *));

    struct hoopoe_user_node *node = head;
    uint32_t i = 0;
    while (node != NULL) {
        users[i++] = &node->user;
        node = node->next;
    }

    return users;
}

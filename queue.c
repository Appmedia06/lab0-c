#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(*head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *node, *safe;

    list_for_each_entry_safe (node, safe, head, list) {
        list_del(&node->list);
        q_release_element(node);
    }
    free(head);
}

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    size_t s_len = sizeof(s);
    new_node->value = malloc((s_len + 1) * sizeof(char));
    if (!new_node->value) {
        q_release_element(new_node);
        return false;
    }

    strncpy(new_node->value, s, s_len);
    new_node->value[s_len] = '\0';
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_node = malloc(sizeof(*new_node));
    if (!new_node)
        return false;
    size_t s_len = strlen(s);
    new_node->value = malloc((s_len + 1) * sizeof(char));
    if (!new_node->value) {
        q_release_element(new_node);
        return false;
    }

    strncpy(new_node->value, s, s_len);
    new_node->value[s_len] = '\0';
    list_add_tail(&new_node->list, head);
    return true;
}

element_t *q_remove_element(element_t *element, char *sp, size_t bufsize)
{
    list_del_init(&element->list);

    if (sp && bufsize) {
        strncpy(sp, element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_first_entry(head, element_t, list);
    q_remove_element(entry, sp, bufsize);
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_last_entry(head, element_t, list);
    q_remove_element(entry, sp, bufsize);
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int size = 0;
    struct list_head *node = NULL;

    list_for_each (node, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *fast = head, *slow = head;
    do {
        fast = fast->next->next;
        if ((fast == head) || (fast == head->prev)) {
            break;
        }
        slow = slow->next;
    } while (!(fast == head) && !(fast == head->prev));

    element_t *node = list_entry(slow->next, element_t, list);
    list_del(&node->list);
    q_release_element(node);
    return true;
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    if (list_empty(head) || list_is_singular(head))
        return true;

    element_t *node, *safe;
    bool dup_flag = false;
    list_for_each_entry_safe (node, safe, head, list) {
        char *next_str = list_entry(node->list.next, element_t, list)->value;
        if ((node->list.next != head) && (strcmp(next_str, node->value) == 0)) {
            list_del(&node->list);
            q_release_element(node);
            dup_flag = true;
        } else if (dup_flag) {
            list_del(&node->list);
            q_release_element(node);
            dup_flag = false;
        }
    }
    return true;
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node = NULL;
    list_for_each (node, head) {
        if (node->next == head)
            break;
        list_move(node, node->next);
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        struct list_head *tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    struct list_head *tmp = head->next;
    node->next = head->prev;
    node->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

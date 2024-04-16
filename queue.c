#include <stdint.h>
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
    size_t s_len = strlen(s);
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
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int cnt = 0;
    struct list_head *node, *safe, *head_from = head;
    LIST_HEAD(new_head);

    list_for_each_safe (node, safe, head) {
        cnt++;
        if (cnt == k) {
            list_cut_position(&new_head, head_from, node);
            q_reverse(&new_head);
            list_splice_init(&new_head, head_from);
            cnt = 0;
            head_from = safe->prev;
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *merge_Two_list(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head *new_head = NULL, **indirect = &new_head,
                     **iter_node = NULL;
    for (; left && right; *iter_node = (*iter_node)->next) {
        iter_node = strcmp(list_entry(left, element_t, list)->value,
                           list_entry(right, element_t, list)->value) >= 0
                        ? &right
                        : &left;
        *indirect = *iter_node;
        indirect = &(*indirect)->next;
    }
    *indirect = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);
    return new_head;
}

struct list_head *q_divide(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast = head, *slow = head, *mid;

    for (; fast && fast->next; fast = fast->next->next, slow = slow->next)
        ;
    mid = slow;
    slow->prev->next = NULL;

    struct list_head *left = q_divide(head);
    struct list_head *right = q_divide(mid);

    return merge_Two_list(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = q_divide(head->next);

    struct list_head *iter_node;
    for (iter_node = head; iter_node->next != NULL;
         iter_node = iter_node->next) {
        iter_node->next->prev = iter_node;
    }
    iter_node->next = head;
    head->prev = iter_node;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int node_num = 0;

    element_t *ele = list_entry(head->next, element_t, list);
    element_t *ele_next = list_entry(head->next->next, element_t, list);
    while (&ele_next->list != head) {
        if (strcmp(ele->value, ele_next->value) < 0) {
            ele_next = list_entry(ele_next->list.next, element_t, list);
            ele = list_entry(ele->list.next, element_t, list);
            node_num++;
        } else {
            list_del(&ele_next->list);
            q_release_element(ele_next);
            ele_next = list_entry(ele->list.next, element_t, list);
        }
    }
    return ++node_num;
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int node_num = 0;

    element_t *ele = list_entry(head->prev, element_t, list);
    element_t *ele_prev = list_entry(head->prev->prev, element_t, list);
    while (&ele_prev->list != head) {
        if (strcmp(ele->value, ele_prev->value) < 0) {
            ele_prev = list_entry(ele_prev->list.prev, element_t, list);
            ele = list_entry(ele->list.prev, element_t, list);
            node_num++;
        } else {
            list_del(&ele_prev->list);
            q_release_element(ele_prev);
            ele_prev = list_entry(ele->list.prev, element_t, list);
        }
    }
    return ++node_num;
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    int node_num = 0;
    struct list_head *first = list_entry(head->next, queue_contex_t, chain)->q;
    node_num += q_size(first);
    first->prev->next = NULL;
    struct list_head *iter_node = head->next->next;

    while (iter_node != head) {
        struct list_head *sub = list_entry(iter_node, queue_contex_t, chain)->q;
        node_num += q_size(sub);
        sub->prev->next = NULL;
        first->next = merge_Two_list(first->next, sub->next);
        INIT_LIST_HEAD(sub);
        iter_node = iter_node->next;
    }

    struct list_head *node;
    for (node = first; node->next != NULL; node = node->next) {
        node->next->prev = node;
    }
    node->next = first;
    first->prev = node;

    return node_num;
    // https://leetcode.com/problems/merge-k-sorted-lists/
}  // end

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create a new element_t */
element_t *element_new(char *s)
{
    element_t *res = (element_t *) malloc(sizeof(element_t));
    if (!res) {
        free(res);
        return NULL;
    }
    res->value = strdup(s);
    return res;
}

/* Delete a element */
element_t *remove_element(struct list_head *head,
                          char *sp,
                          size_t bufsize,
                          struct list_head *target)
{
    element_t *res = list_entry(target, element_t, list);
    if (sp && res->value) {
        strncpy(sp, res->value, bufsize);
        sp[bufsize - 1] = 0;
    }
    list_del(target);
    return res;
}

/* Free element_t */
void free_element(element_t *element)
{
    if (!element)
        return;
    free(element->value);
    free(element);
}


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *res =
        (struct list_head *) malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(res);
    return res;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *tmp, *next;

    list_for_each_safe (tmp, next, l) {
        element_t *entry = list_entry(tmp, element_t, list);
        list_del(tmp);
        free_element(entry);
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *element = element_new(s);
    if (!element) {
        free(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *element = element_new(s);
    if (!element) {
        free(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return remove_element(head, sp, bufsize, head->next);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return remove_element(head, sp, bufsize, head->prev);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next, *target = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        target = target->next;
    }
    list_del(target);
    element_t *entry = list_entry(target, element_t, list);
    free_element(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    if (q_size(head) == 1)
        return true;

    struct list_head *cur = NULL, *safe = NULL;
    bool last = false;
    list_for_each_safe (cur, safe, head) {
        element_t *entry = list_entry(cur, element_t, list);
        if (last || (cur->next != head &&
                     !strcmp(entry->value,
                             list_entry(cur->next, element_t, list)->value))) {
            last = true;
            list_del(cur);
            free_element(entry);
        } else
            last = false;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || q_size(head) < 2)
        return;

    struct list_head *cur = head;
    do {
        element_t *node_1 = list_entry(cur->next, element_t, list);
        element_t *node_2 = list_entry(cur->next->next, element_t, list);

        list_del(&node_1->list);
        list_del(&node_2->list);
        list_add(&node_1->list, cur);
        list_add(&node_2->list, cur);
        cur = cur->next->next;
    } while (cur != head && cur->next != head && cur->next && cur->next->next);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

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

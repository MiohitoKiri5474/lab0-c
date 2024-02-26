#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "list.h"

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
    if (!res->value) {
        free(res);
        return NULL;
    }
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
    if (!head || !element) {
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
    if (!head || !element) {
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
        bool match = cur->next != head &&
                     !strcmp(entry->value,
                             list_entry(cur->next, element_t, list)->value);
        if (last || match) {
            list_del(cur);
            free_element(entry);
        }
        last = match;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || q_size(head) < 2)
        return;
    // the previous version had a bug, using q_reverseK instead
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head)
        list_move(cur, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *cur = head, *tmp;
    do {
        // check the rest of list have k nodes or more
        bool flag = true;
        tmp = cur->next;
        for (int i = 0; i < k; i++) {
            if (!tmp || tmp == head) {
                flag = false;
                break;
            }
            tmp = tmp->next;
        }

        // if the rest of list don't have enough nodes
        if (!flag)
            break;

        // reverse the segment and splice it back to right position
        LIST_HEAD(slice);
        list_cut_position(&slice, cur, tmp->prev);
        q_reverse(&slice);
        list_splice_init(&slice, cur);

        // move forward k nodes
        for (int i = 0; i < k; i++)
            cur = cur->next;
    } while (cur != head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    int len = q_size(head);  // get the length of list
    if (!head || len < 2)
        return;

    LIST_HEAD(left);
    struct list_head *mid = head->next, *fast = head->next, *safe, *cur, *tmp;
    while (fast != head &&
           fast->next != head) {  // get the mid position by using the same way
                                  // in q_delete_mid
        fast = fast->next->next;
        mid = mid->next;
    }

    // split the list into two seq
    list_cut_position(&left, head, mid->prev);

    // sort each side
    q_sort(&left, descend);
    q_sort(head, descend);

    tmp = head->next;
    list_for_each_safe (cur, safe, &left) {  // merge lists
        while (tmp != head) {  // find a place for current element_t object
            char *cur_value = list_entry(cur, element_t, list)->value;
            char *tmp_value = list_entry(tmp, element_t, list)->value;
            if ((!descend && strcmp(cur_value, tmp_value) <= 0) ||
                (descend && strcmp(cur_value, tmp_value) >= 0)) {
                // the element is allowed be placed in this position
                list_del(cur);
                list_add(cur, tmp->prev);
                break;  // current element is placed, break the loop
            }
            // if not, keep moving
            tmp = tmp->next;
        }
        if (tmp == head) {  // if head is already at the tail
            list_del(cur);
            list_add_tail(cur, head);
        }
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *cur, *safe;
    char *mi = NULL;
    q_reverse(head);  // the process will begin from the end, reverse first
    list_for_each_safe (cur, safe, head) {
        element_t *cur_entry = list_entry(cur, element_t, list);
        if (cur != head->next && strcmp(cur_entry->value, mi) > 0) {
            // skip at first element
            list_del(cur);
            free_element(cur_entry);
        } else {
            free(mi);  // importent! remind free the string before next strdup
            mi = strdup(cur_entry->value);
        }
        cur_entry = NULL;
    }
    q_reverse(head);  // reverse back to original order
    free(mi);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    // same as q_ascend, but in different order
    if (!head || list_empty(head))
        return 0;
    struct list_head *cur, *safe;
    char *ma = NULL;
    q_reverse(head);
    list_for_each_safe (cur, safe, head) {
        element_t *cur_entry = list_entry(cur, element_t, list);
        if (cur != head->next && strcmp(cur_entry->value, ma) < 0) {
            list_del(cur);
            free_element(cur_entry);
        } else {
            free(ma);
            ma = strdup(cur_entry->value);
        }
        cur_entry = NULL;
    }
    q_reverse(head);
    free(ma);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    struct list_head *cur, *safe, *cur2, *safe2, *tmp;
    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    int sz_res = first->size;
    list_for_each_safe (cur, safe, head) {
        if (cur == &first->chain)
            continue;
        queue_contex_t *entry = list_entry(cur, queue_contex_t, chain);
        sz_res += entry->size;
        tmp = first->q->next;
        list_for_each_safe (
            cur2, safe2,
            entry->q) {                // merge the first list and current list
            while (tmp != first->q) {  // find a place for current element_t
                                       // object in first list
                char *cur_value = list_entry(cur2, element_t, list)->value;
                char *tmp_value = list_entry(tmp, element_t, list)->value;

                if ((!descend && strcmp(cur_value, tmp_value) <= 0) ||
                    (descend && strcmp(cur_value, tmp_value) >= 0)) {
                    // the element is allowed be placed in this position
                    list_del(cur2);
                    list_add(cur2, tmp->prev);
                    break;  // current element is placed, break the loop
                }
                // if not, keep moving
                tmp = tmp->next;
            }
            if (tmp == first->q) {
                list_del(cur2);
                list_add_tail(cur2, first->q);
            }
        }
    }
    return sz_res;
}

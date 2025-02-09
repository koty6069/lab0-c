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
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    size_t len = strlen(s);
    node->value = malloc(sizeof(char) * (len + 1));
    if (!node->value) {
        free(node);
        return false;
    } /* Need to free the space of node */

    strncpy(node->value, s, len);
    node->value[len] = '\0';
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    size_t len = strlen(s);
    node->value = malloc(sizeof(char) * (len + 1));
    if (!node->value) {
        free(node);
        return false;
    } /* Need to free the space of node */

    strncpy(node->value, s, len);
    node->value[len] = '\0';
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_first_entry(head, element_t, list);
    list_del_init(&node->list);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_last_entry(head, element_t, list);
    list_del_init(&node->list);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
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
    if (!head || list_empty(head))
        return false;

    struct list_head *ahead = head->next, *back = head->prev;
    while (ahead != back && ahead->next != back) {
        ahead = ahead->next;
        back = back->prev;
    }
    list_del_init(back);
    q_release_element(list_entry(back, element_t, list));
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    element_t *cur, *safe;
    bool del = false;
    list_for_each_entry_safe (cur, safe, head, list) {
        char *s = list_entry(cur->list.next, element_t, list)->value;
        if (cur->list.next != head && strcmp(cur->value, s) == 0) {
            list_del(&cur->list);
            q_release_element(cur);
            del = true;
        } else if (del) {
            list_del(&cur->list);
            q_release_element(cur);
            del = false;
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *first = head->next, *second = first->next;
    for (; first != head && second != head;) {
        first->next = second->next;
        second->prev = first->prev;
        first->prev->next = second;
        second->next->prev = first;
        first->prev = second;
        second->next = first;
        first = first->next;
        second = first->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe, *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }

    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *safe, *cut = head;
    int cnt = 0;
    list_for_each_safe (node, safe, head) {
        if (++cnt == k) {
            LIST_HEAD(tmp);
            list_cut_position(&tmp, cut->next, node);
            q_reverse(&tmp);
            list_splice(&tmp, cut);
            cut = safe->prev;
            cnt = 0;
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; l1 && l2; *node = (*node)->next) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) < 0) {
            node = &l1;
        } else {
            node = &l2;
        }
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);
    return head;
}

struct list_head *merge_sort_list(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    /* split list */
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    struct list_head *l1 = merge_sort_list(head);
    struct list_head *l2 = merge_sort_list(fast);

    return merge(l1, l2);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort_list(head->next);
    struct list_head *cur = head, *nt = head->next;
    while (nt) {
        nt->prev = cur;
        cur = nt;
        nt = nt->next;
    }
    cur->next = head;
    head->prev = cur;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *node = head->prev;
    char *str = "";
    while (node != head) {
        element_t *cur = list_entry(node, element_t, list);
        node = node->prev;
        if (strcmp(cur->value, str) < 0) {
            list_del_init(&cur->list);
            q_release_element(cur);
        } else {
            str = cur->value;
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *list = list_first_entry(head, queue_contex_t, chain)->q;
    size_t total = list_first_entry(head, queue_contex_t, chain)->size;

    queue_contex_t *node, *safe;
    list_for_each_entry_safe (node, safe, head, chain) {
        if (node->id == 0)
            continue;

        list_splice_init(node->q, list);
        total += node->size;
    }
    q_sort(list);
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return total;
}

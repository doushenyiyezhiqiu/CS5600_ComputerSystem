#include "queue.h"

/* Creates and returns an empty queue */
queue_t *create_queue(void) {
    queue_t *q = (queue_t *)malloc(sizeof(queue_t));
    if (!q) {
        fprintf(stderr, "Error: could not allocate queue\n");
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* (5 pts) push_queue: Add element to the end (tail) of the doubly-linked list */
void push_queue(queue_t *queue, void *element) {
    if (!queue) return;  /* Safety check */

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) {
        fprintf(stderr, "Error: could not allocate node\n");
        return;
    }
    new_node->data = element;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (queue->tail == NULL) {
        /* Queue is empty, new node is both head and tail */
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        /* Insert at the tail */
        new_node->prev = queue->tail;
        queue->tail->next = new_node;
        queue->tail       = new_node;
    }
    queue->size++;
}

/* (5 pts) pop_queue: remove and return data at the front (head) of the queue */
void *pop_queue(queue_t *queue) {
    if (!queue || !queue->head) {
        return NULL; /* Empty queue or invalid pointer */
    }

    node_t *front = queue->head;
    void   *data  = front->data;

    /* Move head pointer to the next node */
    queue->head = front->next;
    if (queue->head) {
        queue->head->prev = NULL;
    } else {
        /* The queue is now empty */
        queue->tail = NULL;
    }

    free(front);
    queue->size--;
    return data;
}

/* (5 pts) remove_process: remove and return the process with the highest priority
   This is specialized to process_t, so we assume the queue holds process_t* data. */
process_t *remove_process(queue_t *queue) {
    if (!queue || !queue->head) {
        return NULL;  /* No elements */
    }

    /* Find the node with the highest priority (larger int = higher priority) */
    node_t    *current = queue->head;
    node_t    *best_node = current;
    process_t *best_proc = (process_t *)current->data;

    while (current) {
        process_t *p = (process_t *)current->data;
        if (p->priority > best_proc->priority) {
            best_proc = p;
            best_node = current;
        }
        current = current->next;
    }

    /* Remove 'best_node' from the queue */
    if (best_node->prev) {
        best_node->prev->next = best_node->next;
    } else {
        /* If no prev, it's the head of the list */
        queue->head = best_node->next;
    }
    if (best_node->next) {
        best_node->next->prev = best_node->prev;
    } else {
        /* If no next, it's the tail of the list */
        queue->tail = best_node->prev;
    }

    process_t *ret = best_proc;
    free(best_node);
    queue->size--;

    return ret;
}

/* (5 pts) get_queue_size: returns the number of elements in the queue */
int get_queue_size(queue_t *queue) {
    if (!queue) return 0;
    return queue->size;
}

/* Optional helper to free all nodes of a queue (if you need it in your tests) */
void destroy_queue(queue_t *queue) {
    if (!queue) return;

    node_t *current = queue->head;
    while (current) {
        node_t *next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

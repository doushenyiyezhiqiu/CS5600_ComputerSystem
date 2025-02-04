#include "queue.h"

/* (15 pts) main function to test and exercise the queue functions */
int main(void) {
    /* Create a new queue */
    queue_t *q = create_queue();
    if (!q) {
        return 1; /* Could not create */
    }

    /* Create several process_t objects dynamically */
    process_t *p1 = (process_t *)malloc(sizeof(process_t));
    p1->id       = 1;
    p1->name     = "ProcA";
    p1->runtime  = 1000;
    p1->priority = 2;

    process_t *p2 = (process_t *)malloc(sizeof(process_t));
    p2->id       = 2;
    p2->name     = "ProcB";
    p2->runtime  = 500;
    p2->priority = 5;

    process_t *p3 = (process_t *)malloc(sizeof(process_t));
    p3->id       = 3;
    p3->name     = "ProcC";
    p3->runtime  = 300;
    p3->priority = 1;

    printf("Pushing p1, p2, p3 onto queue...\n");
    push_queue(q, p1);
    push_queue(q, p2);
    push_queue(q, p3);
    printf("Queue size is now %d.\n", get_queue_size(q));

    printf("\nPop one element from the front:\n");
    process_t *p_front = (process_t *)pop_queue(q);
    if (p_front) {
        printf("  Popped process: id=%d, name=%s, priority=%d\n",
                p_front->id, p_front->name, p_front->priority);
    }
    printf("Queue size after pop: %d.\n", get_queue_size(q));

    printf("\nRemove process with highest priority:\n");
    process_t *p_high = remove_process(q);
    if (p_high) {
        printf("  Removed highest priority process: id=%d, name=%s, priority=%d\n",
                p_high->id, p_high->name, p_high->priority);
    }
    printf("Queue size after remove_process: %d.\n", get_queue_size(q));

    /* Clean up the last process in queue (optional) */
    void *remaining = pop_queue(q);
    if (remaining) free(remaining);

    /* Clean up */
    destroy_queue(q);
    free(p_front);  /* Freed from pop_queue, if we want to properly release memory */
    free(p_high);   /* Freed from remove_process */

    return 0;
}

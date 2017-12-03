#include "queue.h"
#include <errno.h>

queue_t *create_queue(void) {
    queue_t *que  = calloc(1,sizeof(queue_t));
    if(que == NULL){
        return NULL;
    }
    que->front = NULL;
    que->rear = NULL;
    sem_init(&(que->items),0,0);
    // pthread_mutex_init(&(que->lock), NULL);
    if(pthread_mutex_init(&(que->lock), NULL) != 0){
        return NULL;
    }
    que-> invalid = false;

    return que;
}

bool invalidate_queue(queue_t *self, item_destructor_f destroy_function) {
    if(self == NULL){
        errno = EINVAL;
        return NULL;
    }
    pthread_mutex_lock(&self->lock);

    while(self->front != NULL){
        // sem_wait(&self->items);

        // queue_node_t *freenode = self->front;
        destroy_function(self->front->item);

        self->front = self->front->next;
        // free(freenode);

    }
    pthread_mutex_unlock(&self->lock);
    self-> invalid = true;


    return true;
}

bool enqueue(queue_t *self, void *item) {
    if(self == NULL || item == NULL ){
        return false;
    }
    queue_node_t *newnode  = calloc(1,sizeof(queue_node_t));
    newnode->item = item;
    newnode->next = NULL;
    pthread_mutex_lock(&self->lock);

    if(self->front == NULL){
        self->front = newnode;
        self->rear = newnode;
        // self->front->next = self->rear;
    }

    else{
        self->rear->next = newnode;
        self->rear = newnode;
    }

    pthread_mutex_unlock(&self->lock);
    sem_post(&self->items);


    return true;
}

void *dequeue(queue_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return NULL;
    }
    // if(self->front == NULL){
    //     errno = EINVAL;
    //     return NULL;
    // }
    sem_wait(&self->items);
    pthread_mutex_lock(&self->lock);

    queue_node_t *removenode = self->front;
    queue_node_t *frontnode = self->front->item;
    self->front = self->front->next;

    if(self->front == NULL){
        self->rear=NULL;
    }
    free(removenode);

    pthread_mutex_unlock(&self->lock);

    return frontnode;
}

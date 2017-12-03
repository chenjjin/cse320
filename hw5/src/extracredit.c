#ifndef EC
#define EC
#endif


#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

hashmap_t *create_map(uint32_t capacity, hash_func_f hash_function, destructor_f destroy_function) {
hashmap_t *hashmap =  calloc(1,sizeof(hashmap_t));
    if(hashmap == NULL){
        errno = EINVAL;
        return NULL;
    }
    if(hash_function == NULL){
        errno = EINVAL;
        return NULL;
    }
    if(destroy_function == NULL){
        errno = EINVAL;
        return NULL;
    }
    hashmap->capacity = capacity;
    hashmap->size = 0;
    hashmap->nodes = calloc(capacity,sizeof(map_node_t));
    for(int i  = 0 ; i < capacity ; i++){
        hashmap->nodes[i].key.key_base = NULL;
        hashmap->nodes[i].key.key_len = 0;
        hashmap->nodes[i].tombstone = false;
        hashmap->nodes[i].val.val_base = NULL;
        hashmap->nodes[i].val.val_len = 0;

    }
    hashmap->hash_function=hash_function;
    hashmap->destroy_function = destroy_function;
    hashmap->num_readers = 0;
    if(pthread_mutex_init(&(hashmap->write_lock), NULL) != 0){
        return NULL;
        }
    if(pthread_mutex_init(&(hashmap->fields_lock), NULL) != 0){
        return NULL;
    }

    hashmap->invalid = false;



    hashmap->first = NULL;
    hashmap->rear = NULL;




    return hashmap;}

bool put(hashmap_t *self, map_key_t key, map_val_t val, bool force) {
if(self == NULL || key.key_base == NULL || key.key_len == 0
        || val.val_base == NULL || val.val_len ==0 ){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = self-> hash_function(key) % self->capacity;
    uint32_t currentposition = 0;
        // char *k=key.key_base;
        // char *v=val.val_base;
        // printf("input: key: %d   val:  %d    \n",*k,*v );


    // if((self->nodes+index)->key.key_base == NULL){
    //     (self->nodes+index)->key = key;
    //     (self->nodes+index)->val = val;
    //     self->size ++;


///////////////////////////////////////////////////////////////
        // (self->nodes+index)->next = NULL;
        // (self->nodes+index)->prev = NULL;

    //     if (self->first == NULL){
    //         self->first = (self->nodes+index);
    //         self->rear = (self->nodes+index);
    //     }
    //     else{

    //         self->rear->next = (self->nodes+index);
    //         (self->nodes+index)->prev = self->rear;
    //         self->rear = (self->nodes+index);
    //         self->rear->next = NULL;

    //         }



    //     pthread_mutex_unlock(&self->write_lock);
    //     return true;
    // }


    while(currentposition < self->capacity){

        if((self->nodes+index)->tombstone == true || ((self->nodes+index)->key.key_len == key.key_len
               && memcmp((self->nodes+index)->key.key_base,key.key_base,key.key_len) == 0)||
                (self->nodes+index)->key.key_base == NULL){

            if(((self->nodes+index)->key.key_len == key.key_len
               && memcmp((self->nodes+index)->key.key_base,key.key_base,key.key_len) == 0)){
                self->size = self->size - 1;
            }

            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);
            (self->nodes+index)->key = key;
            (self->nodes+index)->val = val;
            (self->nodes+index)->tombstone = false;
            self->size = self->size + 1;

//////////////////////////////////////////////////////////////////////
            if (self->first == NULL){
                self->first = (self->nodes+index);
                self->rear = (self->nodes+index);
                self->rear->next = NULL;

            }
            else{
                self->rear->next = (self->nodes+index);
                (self->nodes+index)->prev = self->rear;
                self->rear = (self->nodes+index);
                self->rear->next = NULL;

                }


            pthread_mutex_unlock(&self->write_lock);

            return true;
        }

        index = (index + 1) % self->capacity;
        currentposition++;


    }
    if(self->size == self->capacity ){
        if(force){
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);

            (self->first)->key = key;
            (self->first)->val = val;
            (self->rear->next) = self->first;
            self->first->prev = self->rear;
            (self->rear) = self->first;
            (self->first) = self->first->next;
            self->rear->next = NULL;



            pthread_mutex_unlock(&self->write_lock);
            return true;
        }else{
            errno = ENOMEM;
            pthread_mutex_unlock(&self->write_lock);
            return false;
        }
    }

    return false;}

map_val_t get(hashmap_t *self, map_key_t key) {
  if(self == NULL || key.key_base == NULL || key.key_len == 0){
        errno = EINVAL;
        return MAP_VAL(NULL, 0);
    }
    uint32_t index = self-> hash_function(key) % self->capacity;
    uint32_t currentposition = 0;

    pthread_mutex_lock(&self->fields_lock);
    self->num_readers++;
    if(self->num_readers == 1){
        pthread_mutex_lock(&self->write_lock);
    }
    pthread_mutex_unlock(&self->fields_lock);

    while(currentposition < self->capacity){
        if((self->nodes+index)->key.key_len == key.key_len
               && memcmp((self->nodes+index)->key.key_base,key.key_base,key.key_len) == 0){
            pthread_mutex_lock(&self->fields_lock);
            self->num_readers--;
            if(self->num_readers == 0){
                pthread_mutex_unlock(&self->write_lock);
            }

//////////////////////////////////////////////////////////////////
            if(self->nodes+index == self->first){
                (self->rear->next) = self->first;
                self->first->prev = self->rear;
                (self->rear) = self->first;
                (self->first) = self->first->next;
                self->rear->next = NULL;

            }
            else{
                (self->rear->next) = self->nodes+index;
                (self->nodes+index)->prev = (self->nodes+index)->next;
                (self->rear)->prev = self->nodes+index;
                (self->rear) = self->nodes+index;
                self->rear->next = NULL;
            }


            pthread_mutex_unlock(&self->fields_lock);
            return (self->nodes+index)->val;
        }

        currentposition++;
        index = (index + 1) % self->capacity;

    }
        pthread_mutex_lock(&self->fields_lock);
        self->num_readers--;
        if(self->num_readers == 0){
            pthread_mutex_unlock(&self->write_lock);
        }
        pthread_mutex_unlock(&self->fields_lock);

        errno = EINVAL;
        return MAP_VAL(NULL, 0);}

map_node_t delete(hashmap_t *self, map_key_t key) {
    if(self == NULL || key.key_base == NULL || key.key_len == 0){
        errno = EINVAL;
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
    }
    pthread_mutex_lock(&self->write_lock);

    uint32_t index = self-> hash_function(key) % self->capacity;
    uint32_t currentposition = 0;
    while(currentposition < self->capacity){
        if((self->nodes+index)->key.key_len == key.key_len
               && memcmp((self->nodes+index)->key.key_base,key.key_base,key.key_len) == 0){

            (self->nodes+index)->tombstone = true;
            self->size--;




//////////////////////////////////////////////////////////////////
            if(self->nodes+index == self->first){
                (self->rear->next) = self->first;
                self->first->prev = self->rear;
                (self->rear) = self->first;
                (self->first) = self->first->next;
                self->rear->next = NULL;

            }
            else{
                (self->rear->next) = self->nodes+index;
                (self->nodes+index)->prev = (self->nodes+index)->next;
                (self->rear)->prev = self->nodes+index;
                (self->rear) = self->nodes+index;
                self->rear->next = NULL;

            }

            pthread_mutex_unlock(&self->write_lock);

            return MAP_NODE((self->nodes+index)->key, (self->nodes+index)->val, true);
        }

        currentposition++;
        index = (index + 1) % self->capacity;
    }

    pthread_mutex_unlock(&self->write_lock);

    return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);}

bool clear_map(hashmap_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = 0;
    while(index < self->capacity){
        if((self->nodes+index)->key.key_base!=NULL){
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);
            self->size--;
        }
        index++;
    }
    self->first = NULL;
    self->rear = NULL;

    pthread_mutex_unlock(&self->write_lock);
    return true;}

bool invalidate_map(hashmap_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = 0;
    while(index < self->capacity){
        if((self->nodes+index)->key.key_base!=NULL){
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);
            self->size--;
        }
        index++;
    }
    self->invalid = true;
    free(self->nodes);


    self->first = NULL;
    self->rear = NULL;
    pthread_mutex_unlock(&self->write_lock);
    return true;
}




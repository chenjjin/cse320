#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "csapp.h"

#define MAP_KEY(base, len) (map_key_t) {.key_base = base, .key_len = len}
#define MAP_VAL(base, len) (map_val_t) {.val_base = base, .val_len = len}
#define MAP_NODE(key_arg, val_arg, tombstone_arg) (map_node_t) {.key = key_arg, .val = val_arg, .tombstone = tombstone_arg}

hashmap_t *create_map(uint32_t capacity, hash_func_f hash_function, destructor_f destroy_function) {
    hashmap_t *hashmap =  calloc(1,sizeof(hashmap_t));
    if(hashmap == NULL){
        errno = EINVAL;
        return NULL;
    }
    else if(hash_function == NULL){
        errno = EINVAL;
        return NULL;
    }
    else if(destroy_function == NULL){
        errno = EINVAL;
        return NULL;
    }
        hashmap->size = 0;

    hashmap->capacity = capacity;
    hashmap->nodes = calloc(capacity,sizeof(map_node_t));
    int n=0;
    while( n < capacity){
        hashmap->nodes[n].key.key_base = NULL;
        hashmap->nodes[n].key.key_len = 0;
        hashmap->nodes[n].tombstone = false;
        hashmap->nodes[n].val.val_base = NULL;
        hashmap->nodes[n].val.val_len = 0;
        n=n+1;

    }
    hashmap->hash_function=hash_function;
    hashmap->destroy_function = destroy_function;
    hashmap->num_readers = 0;
    hashmap->invalid = false;

    if(pthread_mutex_init(&(hashmap->write_lock), NULL) != 0){
        return NULL;
        }
    if(pthread_mutex_init(&(hashmap->fields_lock), NULL) != 0){
        return NULL;
    }


    return hashmap;
}

bool put(hashmap_t *self, map_key_t key, map_val_t val, bool force) {
    if(self == NULL ||self->invalid == true || key.key_base == NULL || key.key_len == 0
        || val.val_base == NULL || val.val_len ==0 ){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = self-> hash_function(key) % self->capacity;
    uint32_t currentposition = 0;

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
            pthread_mutex_unlock(&self->write_lock);

            return true;
        }

        index = (index + 1) % self->capacity;
        currentposition++;


    }
    if(self->size == self->capacity ){
        if(force==true){
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);
            (self->nodes+index)->key = key;
            (self->nodes+index)->val = val;
            pthread_mutex_unlock(&self->write_lock);
            return true;
        }else{
            errno = ENOMEM;
            pthread_mutex_unlock(&self->write_lock);
            return false;
        }
    }

    return false;
}
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
               && memcmp((self->nodes+index)->key.key_base,key.key_base,key.key_len) == 0
               &&!(self->nodes+index)->tombstone){
            pthread_mutex_lock(&self->fields_lock);
            self->num_readers--;
            if(self->num_readers == 0){
                pthread_mutex_unlock(&self->write_lock);
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
        return MAP_VAL(NULL, 0);

    }

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

            if((self->nodes+index)->tombstone){
                pthread_mutex_unlock(&self->write_lock);

                return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
            }
            (self->nodes+index)->tombstone = true;
            self->size = self->size - 1;

            pthread_mutex_unlock(&self->write_lock);

            return MAP_NODE((self->nodes+index)->key, (self->nodes+index)->val, true);
        }

        currentposition++;
        index = (index + 1) % self->capacity;
    }

    pthread_mutex_unlock(&self->write_lock);

    return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
}

bool clear_map(hashmap_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = 0;
    while(index < self->capacity){
        if((self->nodes+index)->key.key_base!=NULL){
            (self->nodes+index)->tombstone = false;
            (self->nodes+index)->key.key_base=NULL;
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);
            // self->size--;
        }
        index++;
    }
    self->size = 0;
    pthread_mutex_unlock(&self->write_lock);
	return true;
}

bool invalidate_map(hashmap_t *self) {
    if(self == NULL||self->invalid==true){
        errno = EINVAL;
        return false;
    }
    pthread_mutex_lock(&self->write_lock);
    uint32_t index = 0;
    while(index < self->capacity){
        if((self->nodes+index)->key.key_base!=NULL){
            (self->nodes+index)->key.key_base=NULL;
            self->destroy_function((self->nodes+index)->key,(self->nodes+index)->val);

        }
        index++;
    }
    self->size = 0;
    self->invalid = true;
    free(self->nodes);
    pthread_mutex_unlock(&self->write_lock);
    return true;
}

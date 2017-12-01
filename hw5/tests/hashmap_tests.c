#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "hashmap.h"
#define NUM_THREADS 100
#define MAP_KEY(kbase, klen) (map_key_t) {.key_base = kbase, .key_len = klen}
#define MAP_VAL(vbase, vlen) (map_val_t) {.val_base = vbase, .val_len = vlen}

hashmap_t *global_map;

typedef struct map_insert_t {
    void *key_ptr;
    void *val_ptr;
} map_insert_t;

/* Used in item destruction */
void map_free_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}

uint32_t jenkins_hash(map_key_t map_key) {
    const uint8_t *key = map_key.key_base;
    size_t length = map_key.key_len;
    size_t i = 0;
    uint32_t hash = 0;

    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

void map_init(void) {
    global_map = create_map(NUM_THREADS, jenkins_hash, map_free_function);
}

void *thread_put(void *arg) {
    map_insert_t *insert = (map_insert_t *) arg;

    put(global_map, MAP_KEY(insert->key_ptr, sizeof(int)), MAP_VAL(insert->val_ptr, sizeof(int)), false);
    return NULL;
}

void map_fini(void) {
    invalidate_map(global_map);
}

Test(map_suite, 00_creation, .timeout = 2, .init = map_init, .fini = map_fini) {
    cr_assert_not_null(global_map, "Map returned was NULL");
}

Test(map_suite, 02_multithreaded, .timeout = 2, .init = map_init, .fini = map_fini) {
    pthread_t thread_ids[NUM_THREADS];

    // spawn NUM_THREADS threads to put elements
    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_put, insert) != 0)
            exit(EXIT_FAILURE);
    }

    // wait for threads to die before checking queue
    for(int index = 0; index < NUM_THREADS; index++) {
        pthread_join(thread_ids[index], NULL);
    }

    int num_items = global_map->size;
    cr_assert_eq(num_items, NUM_THREADS, "Had %d items in map. Expected %d", num_items, NUM_THREADS);



//     map_node_t *current=global_map->nodes;
//         printf("| key   | val    |\n" );
//     for(int i=0;i<global_map->capacity;i++,current++){
//         char *k=current->key.key_base;
//         char *v=current->val.val_base;
//         printf("| %d    |  %d    |\n",*k,*v );
//     }
//     printf("===================\n");
//     int *key_ptr = malloc(sizeof(int));
//     int *val_ptr = malloc(sizeof(int));
//    //  void *val_ptr=malloc(sizeof(int));
//    //  //printf("===========h======\n");
//    //  val_ptr=get(global_map,MAP_KEY(key_ptr,sizeof(int))).val_base;
//    // // printf("============key_ptr======\n");
//    //  if(val_ptr==NULL)
//    //      printf("hhh\n");
//    //  else
//    //  printf("value:%d\n",*((int*)val_ptr));
//     for(int i=0;i<NUM_THREADS;i++){
//     *key_ptr=i;
//     delete(global_map,MAP_KEY(key_ptr,sizeof(int)));
// }
//     current=global_map->nodes;
//         printf("| key   | val    | tombstone   |\n" );
//     for(int i=0;i<global_map->capacity;i++,current++){
//         char *k=current->key.key_base;
//         char *v=current->val.val_base;
//         //if(current->tombstone==false)
//         printf("| %d    |  %d    |  %d   |\n",*k,*v,current->tombstone );
//     }
//     *key_ptr=4;
//     *val_ptr=88;
//     put(global_map,MAP_KEY(key_ptr,sizeof(int)),MAP_VAL(val_ptr,sizeof(int)),true);
//     printf("********************************\n");
//     current=global_map->nodes;
//         printf("| key   | val    | tombstone   |\n" );
//     for(int i=0;i<global_map->capacity;i++,current++){
//         char *k=current->key.key_base;
//         char *v=current->val.val_base;
//         //if(current->tombstone==false)
//         printf("| %d    |  %d    |  %d   |\n",*k,*v,current->tombstone );
//     }



}

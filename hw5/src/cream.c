#include "cream.h"
#include "utils.h"
#include "csapp.h"
#include "queue.h"



void echo_cnt(int connfd);
void destroy_function(map_key_t key, map_val_t val);


void *thread(void *vargp);
void help();
queue_t* queue;
hashmap_t *hashmap;
int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    // char help[2] = "-h";

    for(int i  = 0; i < argc ; i++){
        if (strcmp(argv[i], "-h") == 0){
            help();
            exit(0);
        }
    }


    if (argc != 4) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

     if(strcmp(argv[1],"1") < 0 ){
        printf("%s\n", "NUM_WORKS and NUM_WORKERS must be at least 1");
        exit(0);
     }

    int num = atoi(argv[3]);

    queue = create_queue();
    hashmap  = create_map(num,jenkins_one_at_a_time_hash,destroy_function);
    listenfd = Open_listenfd(argv[2]);
    int n1=atoi(argv[1]);
    for (i = 0; i < n1; i++)  /* Create worker threads */
        Pthread_create(&tid, NULL, thread, NULL);

        while (1) {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
            // printf("123\n");
            enqueue(queue, &connfd); /* Insert connfd in buffer */
        }

}


void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    int *num_item;
    //sem_getvalue(&queue->items,&num_item);
    while (1) {
    num_item = (int*)(dequeue(queue));
     // printf("123\n");
    int connfd = *num_item; /* Remove connfd from buffer */
     // if(num_item !=0){
    echo_cnt(connfd); /* Service client */
    Close(connfd);
// }
    }
}


 void echo_cnt(int connfd)
 {
    // printf("hello\n");

    rio_t rio;
    struct request_header_t *request_thead = malloc(sizeof(request_header_t));
    struct response_header_t *response_head = malloc(sizeof(response_header_t));
// printf("connfd:%d\n",connfd );
    Rio_readinitb(&rio, connfd);
    Rio_readn(connfd,request_thead,sizeof(request_header_t));
    if(request_thead->request_code == PUT){
        if((MIN_KEY_SIZE > request_thead->key_size ||
            request_thead->key_size > MAX_KEY_SIZE ||
            MIN_VALUE_SIZE > request_thead->value_size ||
            request_thead->value_size > MAX_VALUE_SIZE)){
            response_head->response_code = BAD_REQUEST;
            response_head->value_size = 0;
            Rio_writen(connfd,response_head,sizeof(response_header_t));
        }
        else{

            map_key_t key;
            map_val_t val;
            void *key_base = malloc(4);
            void *val_base = malloc(4);



            // printf("connfd %d\n",connfd);

            Rio_readn(connfd,key_base,request_thead->key_size);
            Rio_readn(connfd,val_base,request_thead->value_size);
            // printf("key_base %d\n",*(int*)key_base );

            key.key_len =request_thead->key_size ;
            val.val_len = request_thead->value_size;
            val.val_base = val_base;

            key.key_base = key_base;
    // printf("hello\n");



             bool return_val = put(hashmap,key,val,true);
            if(return_val == false){
                response_head->response_code = BAD_REQUEST;
                response_head->value_size = 0;
                Rio_writen(connfd,response_head,sizeof(response_header_t));
                free(request_thead);
                free(response_head);
    // printf("hello111\n");

            }
            else{
                response_head->response_code = OK;
                response_head->value_size = 0;
                Rio_writen(connfd,response_head,sizeof(response_header_t));
                free(request_thead);
                free(response_head);
            }
        }

    }



    else if(request_thead->request_code == GET){
        if((MIN_KEY_SIZE > request_thead->key_size ||
            request_thead->key_size > MAX_KEY_SIZE )){
            response_head->response_code = BAD_REQUEST;
            response_head->value_size = 0;
            Rio_writen(connfd,response_head,sizeof(response_header_t));
        }
    // printf("hello\n");

        else{
                        map_val_t val;

            map_key_t key;
            void *key_base = malloc(sizeof(4));

            Rio_readn(connfd,key_base,request_thead->key_size);
            key.key_len =request_thead->key_size ;
            val.val_len = request_thead->value_size;

            key.key_base = key_base;

            val = get(hashmap,key);
            if(val.val_base == NULL){
                response_head->response_code = NOT_FOUND;
                response_head->value_size = 0;
                Rio_writen(connfd,response_head,sizeof(response_header_t));
            }
            else{
                response_head->response_code = OK;
                response_head->value_size = val.val_len;
                Rio_writen(connfd,response_head,sizeof(response_header_t));
                Rio_writen(connfd,val.val_base,val.val_len);
            }
        }
    }




    else if(request_thead->request_code == EVICT){
        if((MIN_KEY_SIZE > request_thead->key_size ||
            request_thead->key_size > MAX_KEY_SIZE )){
            response_head->response_code = BAD_REQUEST;
            response_head->value_size = 0;
            Rio_writen(connfd,response_head,sizeof(response_header_t));
        }

        else{
            map_key_t key;
            void *key_base = malloc(sizeof(4));

            Rio_readn(connfd,key_base,request_thead->key_size);
            key.key_len =request_thead->key_size ;
            key.key_base = key_base;
            delete(hashmap,key);

            response_head->response_code = OK;
            response_head->value_size = 0;
            Rio_writen(connfd,response_head,sizeof(response_header_t));
        }

    }


    else if(request_thead->request_code == CLEAR){

        clear_map(hashmap);
        response_head->response_code = OK;
        response_head->value_size = 0;
        Rio_writen(connfd,response_head,sizeof(response_header_t));
    }


    else{
        response_head->response_code = UNSUPPORTED;
        response_head->value_size = 0;
        Rio_writen(connfd,response_head,sizeof(response_header_t));

    }
 }

void help(){
printf("-h                 Displays this help menu and returns EXIT_SUCCESS.\n"
"NUM_WORKERS        The number of worker threads used to service requests.\n"
"PORT_NUMBER        Port number to listen on for incoming connections.\n"
"MAX_ENTRIES        The maximum number of entries that can be stored in `cream`'s                   underlying data store.\n"
);
}



void destroy_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}
/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



size_t padding(size_t size);
size_t paddingSize(size_t size);
void* first_fit(size_t total_size);

void put_in(void* free_pointer,size_t total_size,int check_padding,size_t padding_size);
void set_Header(void* free_pointer,size_t free_size,size_t size_splinter);
void set_Footer(void* pointer,size_t free_size,size_t size_splinter,size_t padding_size);

void removeFromList(void* free_pointer);
void set_free_header(void* new_free_pointer,size_t remaining_free_size);
void set_free_footer(void* new_free_pointer,size_t remaining_free_size);
int get_List_number(uint64_t blockSize);

void insertToList(void* new_free_pointer);

void lower_coalesce(void* next_starting_adress);
void* get_Prev(void* pointer);
size_t get_Size(void* pointer);
size_t get_Alloc(void* pointer);
void upper_coalesce(void* ptr);
size_t get_Padded(void* pointer);
void check_valid(void* ptr);




int check_padding = 0;
int spark_time=0;

/**
 * You should store the heads of your free lists in these variables.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
free_list seg_free_list[4] = {
    {NULL, LIST_1_MIN, LIST_1_MAX},
    {NULL, LIST_2_MIN, LIST_2_MAX},
    {NULL, LIST_3_MIN, LIST_3_MAX},
    {NULL, LIST_4_MIN, LIST_4_MAX}
};


int sf_errno = 0;
int start = 0;
void* starting_adress;

void *sf_malloc(size_t size) {

    void* free_pointer = NULL;

    if(size<=0 || size>PAGE_SZ*4){
        sf_errno = EINVAL;
        return NULL;
    }

//get the starting adress
    if(start == 0){
        starting_adress = sf_sbrk();
        //header
        void* start_adress = get_heap_start();
        seg_free_list[3].head = (sf_free_header*)start_adress;
        seg_free_list[3].head->header.allocated = 0;
        seg_free_list[3].head->header.padded = 0;
        seg_free_list[3].head->header.two_zeroes = 0;
        seg_free_list[3].head->header.block_size = PAGE_SZ>>4;
        seg_free_list[3].head->header.unused = 0;

        seg_free_list[3].head->next= NULL;
        seg_free_list[3].head->prev=NULL;

        //footer
        void* end_adress = (void*)get_heap_end()-8;
        sf_footer* pageFooter = end_adress;
        pageFooter->allocated = 0;
        pageFooter->padded = 0;
        pageFooter->two_zeroes = 0;
        pageFooter ->block_size = PAGE_SZ>>4;
        // pageFooter ->requested_size = 4096;
        spark_time = spark_time+1;
        start = 1;
    }
    size_t get_padding=padding(size);
    size_t padding_size = paddingSize(size);
    size_t total_size = get_padding+16;

//start searching
    while(free_pointer == NULL){
        free_pointer = first_fit(total_size);
        // printf("this is pointer we fit:%p\n", free_pointer);
        if(free_pointer!=NULL){
            put_in(free_pointer,total_size,check_padding,padding_size);
                    // seg_free_list[3].head->next = (void*)0x1;

        // printf("here is free_pointer finally:%p\n", free_pointer);
            return ((void*)free_pointer+8);
        }
        else{//no free block found, request new page
            // printf("you should be heere\n" );
            if(spark_time<4){
                void* next_starting_adress = sf_sbrk();
                spark_time = spark_time+1;
                if(next_starting_adress==NULL){
                    sf_errno = ENOMEM;
                    return NULL;
                }
                set_free_header(next_starting_adress,PAGE_SZ);
                set_free_footer((void*)next_starting_adress+PAGE_SZ-8,PAGE_SZ);
                insertToList(next_starting_adress);
                // sf_snapshot();
                lower_coalesce(next_starting_adress);

            }
            else{
                break;
            }

        }
    }
    sf_errno = ENOMEM;
    return NULL;

}
void lower_coalesce(void* next_starting_adress){
    sf_free_header* pointer = (sf_free_header*)next_starting_adress;
    sf_free_header* previous_Footer= (void*)pointer-8;
    size_t previous_size = previous_Footer->header.block_size<<4;
    // printf("previous size%zd\n",previous_size );
    sf_free_header* previous_Header = (void*)previous_Footer-previous_size+8;

    if(previous_Header->header.allocated == 0){
        size_t whole_size = get_Size(next_starting_adress)+previous_size;
        // printf("previous whole size%zd\n",whole_size );
        removeFromList(previous_Header);
        removeFromList(next_starting_adress);
        set_free_header((void*)previous_Header,whole_size);
        set_free_footer((void*)previous_Header+whole_size-8,whole_size);
        insertToList(previous_Header);

    }


}

size_t paddingSize(size_t size){
    if(size%16 == 0){
        return 0;
    }
    else{

        return 16-(size%16);
    }
}

size_t padding(size_t size){
    if(size%16 == 0){
        return size;
    }
    else{
        check_padding = 1;
        return size+16-(size%16);
    }
}


//return the first adress of satisfy free block
void* first_fit(size_t total_size){
    sf_free_header* free= NULL; // free block to return
    sf_free_header* top;    //check from the top
    size_t currentSize;         //current sizr of a free block
    int list_size = 0;
    list_size = get_List_number(total_size);

    while(list_size <= 3){

        for(top=seg_free_list[list_size].head; top!=NULL; top = top->next){
                // printf("why\n" );

            currentSize = top->header.block_size<<4;
            if(total_size<=currentSize){
                free = top;
                return free;
            }

        }
        list_size = list_size+1;

    }
    return free;
}

//check splinter and arrange list
void put_in(void* free_pointer,size_t total_size,int check_padding,size_t padding_size){
    sf_free_header* free_p = (sf_free_header*)free_pointer;
    size_t total_free_number = free_p->header.block_size<<4;
    size_t remaining_free_size = total_free_number - total_size;

    if(remaining_free_size<32){
        size_t size_splinter = total_free_number-total_size;

        check_padding = 1;
        set_Header(free_pointer,total_free_number,size_splinter);
        set_Footer((void*)free_pointer+total_free_number-8,total_free_number,size_splinter,padding_size);
        removeFromList(free_pointer);
    }
    else{
        removeFromList(free_pointer);

        set_Header((void*)free_pointer,total_size,padding_size);
        set_Footer((void*)free_pointer+total_size-8,total_size,0,padding_size);

        void* new_free_pointer = (void*)free_pointer+total_size;
        set_free_header(new_free_pointer,remaining_free_size);
        set_free_footer((void*)new_free_pointer+remaining_free_size-8,remaining_free_size);
        insertToList(new_free_pointer);

    }

}

void insertToList(void* new_free_pointer){
    sf_free_header* new_free = new_free_pointer;

    int listNumber = get_List_number((new_free->header.block_size)<<4);
    //printf("here is next should be null:%p\n", seg_free_list[listNumber].head);
    // printf("this is list heaf:%p \n",seg_free_list[listNumber].head );

    if(seg_free_list[listNumber].head == NULL){
        seg_free_list[listNumber].head = new_free_pointer;
        seg_free_list[listNumber].head->prev = NULL;
        seg_free_list[listNumber].head->next = NULL;
    }
    else{
        // printf("pointer %p\n", seg_free_list[listNumber].head->next);
        new_free->next =seg_free_list[listNumber].head;
        new_free->prev = NULL;
        seg_free_list[listNumber].head->prev = new_free;
        seg_free_list[listNumber].head = new_free;

    }

    return;


}

int get_List_number(uint64_t blockSize){
    int listNumber;
    if(blockSize<=LIST_1_MAX){
        listNumber = 0;
    }
    else if(blockSize<=LIST_2_MAX){
        listNumber = 1;
    }
    else if(blockSize<=LIST_3_MAX){
        listNumber = 2;
    }
    else
    {
        listNumber = 3;
    }
    return listNumber;
}

void set_free_header(void* new_free_pointer,size_t remaining_free_size){
    sf_free_header* newHead =  new_free_pointer;
    newHead->header.allocated = 0;
    newHead->header.padded=0;
    newHead->header.two_zeroes = 00;
    newHead->header.block_size=remaining_free_size>>4;
}

void set_free_footer(void* new_free_pointer,size_t remaining_free_size){
    sf_footer* newFoot =(sf_footer*)new_free_pointer;
    newFoot->allocated=0;
    newFoot->padded = 0;
    newFoot->two_zeroes=0;
    newFoot->block_size = remaining_free_size>>4;
    newFoot->requested_size = 0;
}

void removeFromList(void* free_pointer){
    int listNumber;
    sf_free_header * prevPointer = ((sf_free_header*)free_pointer)->prev;
    sf_free_header * nextPointer = ((sf_free_header*)free_pointer)->next;
    // printf("here is next should be null:%p\n", nextPointer);
    sf_header* freePointer = (sf_header*)free_pointer;
    uint64_t blockSize = (freePointer->block_size)<<4;

    listNumber=get_List_number(blockSize);

    if(prevPointer!=NULL){
        prevPointer-> next = nextPointer;
        ((sf_free_header*)free_pointer)->prev= NULL;
    }
    else{
         // printf("this is nextPointer%p\n",nextPointer );
         // printf("this is listnumber%i\n",listNumber );
        seg_free_list[listNumber].head=nextPointer;
    }
    if(nextPointer!=NULL){
        nextPointer->prev = prevPointer;
        ((sf_free_header*)free_pointer)->next= NULL;

    }

}

void set_Header(void* free_pointer,size_t free_size,size_t size_splinter){

    sf_header* header = (sf_header*)free_pointer;
    header->allocated = 1;

    if(size_splinter == 0){
        header->padded = check_padding;
    }
    else{
        check_padding =1;
        header->padded=check_padding;
    }
    header->two_zeroes = 0;
    header->block_size = free_size>>4;
    header->unused = 0;
    // printf("free_pointer when seeeting %p\n",free_pointer );

}

void set_Footer(void* pointer,size_t free_size,size_t size_splinter,size_t padding_size){
    sf_footer* footer = (sf_footer*)pointer;
    footer->allocated = 1;

    if(size_splinter == 0){
        footer->padded = check_padding;
    }
    else{
        check_padding = 1;
        footer->padded  = check_padding;
    }
    footer->two_zeroes = 0;
    footer->block_size = free_size>>4;
    footer->requested_size = free_size - size_splinter - padding_size -16;
}

void* get_Next(void* pointer){
    return (void*)pointer+get_Size(pointer);
}

//retutn the previous block header adress
void* get_Prev(void* pointer){
    return (void*)pointer-get_Size((void*)pointer -8);
}

//return the size of curent pointer
size_t get_Size(void* pointer){
    return (*(unsigned int*)pointer) & 0xfffffff0;

}

size_t get_Alloc(void* pointer){
    return (*(unsigned int*)(pointer)) & 0x1;
}

size_t get_Padded(void* pointer){
    return ((*(unsigned int*)(pointer)) & 0x2)>>1 ;
}

size_t get_Request(void* pointer){
    return ((*(unsigned long*)(pointer)) & 0xffffffff)>>32;
}

void sf_free(void *ptr) {
    if(ptr==NULL){
        abort();
    }
    if(ptr<(void*)get_heap_start()||ptr>(void*)get_heap_end()){
        abort();
    }
    if(get_Alloc((void*)ptr-8)==0){
        abort();
    }
    // printf("first:%zd\n", get_Padded((void*)ptr-8));
    // printf("second:%zd\n", get_Padded((void*)ptr+get_Size(ptr-8)-16));
    if(get_Alloc((void*)ptr-8) != get_Alloc((void*)ptr+get_Size(ptr-8)-16)){
        abort();
    }
    if(get_Padded((void*)ptr-8) != get_Padded((void*)ptr+get_Size(ptr-8)-16)){
        abort();
    }

    sf_footer* footer = ptr+get_Size(ptr-8)-16;
    // printf("Third:%u\n", footer->requested_size);

    if(footer->requested_size+16!=get_Size(ptr-8)){
        if(get_Padded((void*)ptr-8)!=1){
            abort();
        }
    }

    ptr= (void*)ptr-8;
    size_t size = get_Size(ptr);
    set_free_header(ptr,size);
    void* ptr_footer = (void*)ptr+size-8;
    set_free_footer(ptr_footer,size);
    insertToList(ptr);
    upper_coalesce(ptr);
}

void upper_coalesce(void* ptr){
    sf_free_header* pointer = (sf_free_header*)ptr;
    size_t ptr_Size = pointer->header.block_size;
    ptr_Size = ptr_Size<<4;
    // printf("%zd\n",ptr_Size );
    sf_free_header* next_Header = ptr + ptr_Size;

    // sf_free_header* next_Header= (sf_free_header*)get_Next(ptr);
    // printf("%u\n", );
    if(next_Header->header.allocated == 0 ){
        size_t whole_size = get_Size(ptr)+(next_Header->header.block_size<<4);

        removeFromList(ptr);
        removeFromList(next_Header);
        set_free_header(ptr,whole_size);
        set_free_footer((void*)ptr+whole_size-8,whole_size);
        insertToList(ptr);
    }
}


// void check_valid(void* ptr){
//     if(ptr==NULL){
//         abort();
//     }
//     if(ptr<(void*)get_heap_start()||ptr>(void*)get_heap_end()){
//         abort();
//     }
//     if(get_Alloc((void*)ptr-8)==0){
//         abort();
//     }
//     // printf("first:%zd\n", get_Padded((void*)ptr-8));
//     // printf("second:%zd\n", get_Padded((void*)ptr+get_Size(ptr-8)-16));
//     if(get_Alloc((void*)ptr-8) != get_Alloc((void*)ptr+get_Size(ptr-8)-16)){
//         abort();
//     }
//     if(get_Padded((void*)ptr-8) != get_Alloc((void*)ptr+get_Size(ptr-8)-16)){
//         abort();
//     }

//     sf_footer* footer = ptr+get_Size(ptr-8)-16;
//     // printf("Third:%u\n", footer->requested_size);

//     if(footer->requested_size+16!=get_Size(ptr-8)){
//         if(get_Padded((void*)ptr-8)!=1){
//             abort();
//         }
//     }
// }
void *sf_realloc(void *ptr, size_t size) {
    if(ptr==NULL){
        errno = EINVAL;
        abort();
    }
    if(ptr<(void*)get_heap_start()||ptr>(void*)get_heap_end()){
        errno = EINVAL;
        abort();
    }
    if(get_Alloc((void*)ptr-8)==0){
        errno = EINVAL;
        abort();
    }
    // printf("first:%zd\n", get_Padded((void*)ptr-8));
    // printf("second:%zd\n", get_Padded((void*)ptr+get_Size(ptr-8)-16));
    if(get_Alloc((void*)ptr-8) != get_Alloc((void*)ptr+get_Size(ptr-8)-16)){
        errno = EINVAL;
        abort();
    }
    if(get_Padded((void*)ptr-8) != get_Padded((void*)ptr+get_Size(ptr-8)-16)){
        errno = EINVAL;
        abort();
    }

    sf_footer* footer = ptr+get_Size(ptr-8)-16;
    // printf("Third:%u\n", footer->requested_size);

    if(footer->requested_size+16!=get_Size(ptr-8)){
        if(get_Padded((void*)ptr-8)!=1){
            errno = EINVAL;
            abort();
        }
    }

    if(size == 0){
        errno = EINVAL;
        sf_free(ptr);
        return NULL;
    }
    void* new_ptr;
    size_t oldSize = get_Size((void*)ptr-8) ;
    size_t make_padding =padding(size);
    size_t get_padding_size= paddingSize(size);
    size_t new_size = make_padding+16;
    if(size>PAGE_SZ*4){
        sf_errno = ENOMEM;
        return NULL;
    }
    if(oldSize == new_size){
        return ptr;
    }
    if(oldSize<new_size){
        // printf("new_size%zd\n",new_size );
        new_ptr = sf_malloc(size);
        if(new_ptr == NULL){
            errno = ENOMEM;
        }
        memcpy(new_ptr,ptr,oldSize-16);
        sf_free(ptr);

        return new_ptr;

    }
    else if(oldSize>new_size){
        if(oldSize-new_size <32){
            set_Footer((void*)ptr-8+oldSize-8,oldSize,1,get_padding_size);
            return ptr;
        }
        else{
            set_Header((void*)ptr-8,new_size,0);
            set_Footer((void*)ptr-8+new_size-8,new_size,0,get_padding_size);
            void* new_free_ptr = (void*)ptr-8+new_size;
            size_t new_free_size = oldSize-new_size;
            set_free_header(new_free_ptr,new_free_size);
            set_free_footer((void*)new_free_ptr+new_free_size-8,new_free_size);
            insertToList(new_free_ptr);
            upper_coalesce(new_free_ptr);
            return ptr;
        }
    }

    return NULL;
}


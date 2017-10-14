#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
char* a = sf_malloc(16);
sf_free(a);
    // printf("sf_realloc size:::%i\n",*b );
    //void *y = sf_realloc(x, sizeof(char));

    // free_list *fl = &seg_free_list[0];

    // sf_header *header = (sf_header*)((char*)x - 8);

     sf_snapshot();
     sf_blockprint((char*)a-8);
     // printf("hre%i\n", seg_free_list[3]->head);
     // sf_free(x);

    // sf_varprint(x);
    //double* ptr = sf_malloc(sizeof(double));

    //*ptr = 320320320e-320;

    //printf("%f\n", *ptr);

    //sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}

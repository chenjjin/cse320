#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
    void *x = sf_malloc(sizeof(int) * 8);
    void *y = sf_realloc(x, sizeof(char));

    // free_list *fl = &seg_free_list[0];

    // sf_header *header = (sf_header*)((char*)x - 8);

     sf_snapshot();
     sf_blockprint((char*)y-8);

    // sf_varprint(x);
    //double* ptr = sf_malloc(sizeof(double));

    //*ptr = 320320320e-320;

    //printf("%f\n", *ptr);

    //sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}

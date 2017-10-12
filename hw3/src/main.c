#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {


    sf_mem_init();
    sf_errno = 0;
    sf_malloc(PAGE_SZ << 2);;

    printf("sf_errno:%i",sf_errno);

    // sf_errno = 0;
    // int *x = sf_malloc(sizeof(int));
    // *x = 4;
    // sf_snapshot();
    // sf_blockprint((char*)x-8);

    // sf_varprint(x);
    //double* ptr = sf_malloc(sizeof(double));

    //*ptr = 320320320e-320;

    //printf("%f\n", *ptr);

    //sf_free(ptr);

    sf_mem_fini();

    return EXIT_SUCCESS;
}

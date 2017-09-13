#include <stdlib.h>

#include "hw1.h"
#include "debug.h"
#include "part2.h"
#include "part3.h"


#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    unsigned short mode;
    unsigned short modeRepl;

    mode = validargs(argc, argv);

    /*printf("Success: %d\n", EXIT_SUCCESS);
    printf("Failure: %d\n", EXIT_FAILURE);*/

    debug("Mode: 0x%X", mode);

        // printf("this is return value: %x\n",mode );


    if (argc == 1){
        USAGE(*argv, EXIT_FAILURE);
    }

    if(mode == 0x8000) {
        USAGE(*argv, EXIT_SUCCESS);
    }

    else if (mode == 0x0000){
        USAGE(*argv, EXIT_FAILURE);
    }


    modeRepl = mode >> 14;
    modeRepl = modeRepl & 0x01;
    if(modeRepl == 0){
        tryPART2(mode);
    }
    else{
        tryPART3(mode);
    }

    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */


#include "hw1.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

unsigned short checkEandD(char **argv);
unsigned short checkFirstRandC(char **argv);
unsigned short checkSecondRandC(char **argv);


/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the program
 * and will return a unsigned short (2 bytes) that will contain the
 * information necessary for the proper execution of the program.
 *
 * IF -p is given but no (-r) ROWS or (-c) COLUMNS are specified this function
 * MUST set the lower bits to the default value of 10. If one or the other
 * (rows/columns) is specified then you MUST keep that value rather than assigning the default.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return Refer to homework document for the return value of this function.
 */
unsigned short validargs(int argc, char **argv) {

    char f = 'f';
    char h = 'h';
    char p = 'p';
    unsigned short shortvalue;
    int flagLeft = argc;
    unsigned short defaultR = 0x00A0;
    unsigned short defaultC = 0x000A;

    //printf("%s\n", *argv);
    //printf("%c\n", *(*(argv+1)+1));

    //check -f flag
    if ( (argc != 1) && (*(*(argv+1)+1) == h)){
        printf("you are good\n");
        return 0x8000;
    }
    //check the argument length
    else if (argc <= 1 || argc > 8){
        printf("you are fail\n");
        return 0x0000;
    }
    //check if  start with p and f
    else if(*(*(argv+1)+1) != p && *(*(argv+1)+1) !=f){
        printf("you are fail\n");
        return 0x0000;
    }
    //case when p appear
    else if(*(*(argv+1)+1) == 'p'){
        flagLeft = flagLeft - 3;
        shortvalue = 0x0000;
        shortvalue = shortvalue | checkEandD(argv);

        if (flagLeft == 0){
            shortvalue = shortvalue | 0x00AA;
        printf("%x\n", shortvalue);
            return shortvalue;
        }

        // add first -r -c -k
        shortvalue = shortvalue | checkFirstRandC(argv);
        flagLeft = flagLeft - 2;
        if(flagLeft == 0){
            if (*(*(argv+3)+1) == 'r'){
                shortvalue = shortvalue | defaultC;
                printf("%x\n come on ,please come", shortvalue );
                return shortvalue;
            }
            else if (*(*(argv+3)+1) == 'c'){
                shortvalue = shortvalue | defaultR;
                return shortvalue;
            }

            else{
                return shortvalue;
            }
        }

        //add second -r -c -k
        shortvalue = shortvalue | checkFirstRandC(argv);
        flagLeft = flagLeft - 2;
        if(flagLeft == 0){
            if (*(*(argv+5)+1) == 'r'){
                printf("%x\n come on ,please come", shortvalue );
                return shortvalue;
            }
            else if (*(*(argv+5)+1) == 'c'){
                return shortvalue;
            }
            else{
                return shortvalue;
            }

        }

        //add third -r -c -k
        shortvalue = shortvalue | checkFirstRandC(argv);
        if (*(*(argv+5)+1) == 'r'){
                printf("%x\n come on ,please come", shortvalue );
                return shortvalue;
            }
            else if (*(*(argv+5)+1) == 'c'){
                return shortvalue;
            }
        return shortvalue;
        printf("%x\n", shortvalue);
    }
    //case when f appear
    else if(*(*(argv+1)+1) == 'f'){
        //flagLeft = flagLeft - 3;
        shortvalue = 0x4000;
        shortvalue = shortvalue | checkEandD(argv);
                printf("%x\n", shortvalue);
        return shortvalue;
        }



    return 0x0000;
}

//checkEandD check if there is -e or -d after
unsigned short checkEandD(char **argv) {
    if(*(*(argv+2)+1) == 'e'){
        return 0x0000;
    }
    //check for -d
    else {
        return 0x2000;
    }
}

unsigned short checkFirstRandC(char **argv) {
    if(*(*(argv+3)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+4) - '0';
        if (firstR == 9){
            firstR = firstR << 4;
            return firstR;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+4)+1) - '0';
            unsigned short totalR = firstR*10 + secondR;
            totalR = totalR << 4;
            printf("%x\n now \n", totalR );
            return totalR;
        }
    }
    //check for -c
    else if(*(*(argv+3)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+4) - '0';
        if (firstC == 9){
            return firstC;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+4)+1) - '0';
            unsigned short totalC = firstC*10 + secondC;
            printf("%x\n come on ,please come", totalC );
            return totalC;
        }
    }
    else{
        return 0x0000;
    }
}

unsigned short checkSecondRandC(char **argv) {
    if(*(*(argv+5)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+6) - '0';
        if (firstR == 9){
            firstR = firstR << 4;
            return firstR;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+6)+1) - '0';
            unsigned short totalR = firstR*10 + secondR;
            totalR = totalR << 4;
            printf("%x\n now \n", totalR );
            return totalR;
        }
    }
    //check for -c
    else if(*(*(argv+5)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+6) - '0';
        if (firstC == 9){
            return firstC;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+6)+1) - '0';
            unsigned short totalC = firstC*10 + secondC;
            printf("%x\n come on ,please come", totalC );
            return totalC;
        }
    }
    else{
        return 0x0000;
    }
}

unsigned short checkThirdRandC(char **argv) {
    if(*(*(argv+7)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+8) - '0';
        if (firstR == 9){
            firstR = firstR << 4;
            return firstR;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+8)+1) - '0';
            unsigned short totalR = firstR*10 + secondR;
            totalR = totalR << 4;
            printf("%x\n now \n", totalR );
            return totalR;
        }
    }
    //check for -c
    else if(*(*(argv+7)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+8) - '0';
        if (firstC == 9){
            return firstC;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+8)+1) - '0';
            unsigned short totalC = firstC*10 + secondC;
            printf("%x\n come on ,please come", totalC );
            return totalC;
        }
    }
    else{
        return 0x0000;
    }
}
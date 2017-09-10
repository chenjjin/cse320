#include "hw1.h"
#include <stdlib.h>

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
unsigned short checkThirdRandC(char **argv);
int checkNumberRC(int argc,char **argv);

int CheckArgument(int argc,char **argv);
unsigned short PolyLength(char* polybius_alphabet);
int rowTimesCol(unsigned short hexNumber,unsigned short totalLength);
unsigned short PolyLength(char* polybius_alphabet);
int validPolyKey(char **argv,int position);
int validFmKey(char **argv,int position);


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
    unsigned short polyLength = PolyLength(polybius_alphabet);

    printf("this is the length:  %x\n ", PolyLength(polybius_alphabet));
    //printf("%c\n", *(*(argv+1)+1));

    //check -f flag
    if ( (argc != 1) && (*(*(argv+1)+1) == h)){
        printf("you are good\n");
        return 0x8000;
    }
    //check the argument length
    else if (argc <= 2 || argc > 9){
        printf("you are fail\n");
        return 0x0000;
    }
    //check if  start with p and f
    else if(*(*(argv+1)+1) != p && *(*(argv+1)+1) !=f){
        printf("you are fail\n");
        return 0x0000;
    }

    //check argument order
    else if(CheckArgument(flagLeft,argv)){
        printf("this is the fault" );
        return 0x0000;
    }

    //case when p appear
    else if(*(*(argv+1)+1) == 'p'){

        //first check valid row and column
        if(checkNumberRC(flagLeft,argv)){
            printf("this is 1\n" );
            return 0x0000;
        }
        flagLeft = flagLeft - 3;
        shortvalue = 0x0000;
        shortvalue = shortvalue | checkEandD(argv);

        if (flagLeft == 0){
             printf("this is 99\n" );

            shortvalue = shortvalue | 0x00AA;
            if (rowTimesCol(shortvalue,polyLength)){
                return 0x0000;
            }
            printf("\nthis is return code: %x\n",shortvalue);
            return shortvalue;
        }

        // add first -r -c -k
        shortvalue = shortvalue | checkFirstRandC(argv);
        flagLeft = flagLeft - 2;
        int Rexist = 1;
        int Cexist = 1;
        if (*(*(argv+3)+1) == 'r'){
             Rexist = 0;
        }
        else if(*(*(argv+3)+1) == 'c'){
             Cexist = 0;
        }
        if(flagLeft == 0){
            if (*(*(argv+3)+1) == 'r'){
                shortvalue = shortvalue | defaultC;
                printf("%x\n come on ,please come1", shortvalue );
                if (rowTimesCol(shortvalue,polyLength)){
                    return 0x0000;
                }
            printf("\nthis is return code: %x\n",shortvalue);

                return shortvalue;
            }
            else if (*(*(argv+3)+1) == 'c'){
                shortvalue = shortvalue | defaultR;
                if (rowTimesCol(shortvalue,polyLength)){
                    return 0x0000;
                }
                return shortvalue;
            }

            else{
                shortvalue = shortvalue | 0x00AA;
                 printf("this is 100\n" );
                 if (rowTimesCol(shortvalue,polyLength)){
                    return 0x0000;
                }
                if(validPolyKey(argv,4)){
                    return 0x0000;
                }

                printf("key1: %c\n",*key );
                return shortvalue;
            }
        }
        if (*(*(argv+3)+1) == 'k'){
            if(validPolyKey(argv,4)){
                return 0x0000;
            }
        }

        //add second -r -c -k
        shortvalue = shortvalue | checkSecondRandC(argv);
        flagLeft = flagLeft - 2;
        if(flagLeft == 0){


            if (*(*(argv+5)+1) == 'r'){
                if (Cexist){
                    printf("cis no here\n");
                    shortvalue = shortvalue | 0x000A;
                }
                printf("%x\n come on ,please come2", shortvalue );
                printf("this is short value: %x\n",shortvalue );
                if (rowTimesCol(shortvalue,polyLength)){
                    printf("here1");
                    return 0x0000;
                }
                return shortvalue;
            }
            else if (*(*(argv+5)+1) == 'c'){
                if (Rexist){

                    shortvalue = shortvalue | 0x00A0;
                }
                printf("\nshortvalue:%x\n",shortvalue);
                 if (rowTimesCol(shortvalue,polyLength)){
                    return 0x0000;
                }

                return shortvalue;
            }
            else{
                if (Cexist){
                    shortvalue = shortvalue | 0x000A;
                }
                if (Rexist){
                    shortvalue = shortvalue | 0x00A0;
                }
                if (rowTimesCol(shortvalue,polyLength)){
                    return 0x0000;
                }
                if(validPolyKey(argv,6)){
                    return 0x0000;
                }
                printf("key2: %c\n",*key );
                return shortvalue;
            }

        }
        if (*(*(argv+5)+1) == 'k'){
            if(validPolyKey(argv,6)){
                return 0x0000;
            }
        }

        //add third -r -c -k
        shortvalue = shortvalue | checkThirdRandC(argv);
        if (rowTimesCol(shortvalue,polyLength)){
            return 0x0000;
        }
        if (*(*(argv+7)+1) == 'k'){
            if(validPolyKey(argv,8)){
                return 0x0000;
            }
        }
        printf("key3: %c\n",*key );
        return shortvalue;
        printf("%x\n", shortvalue);
    }
    //case when f appear
    else if(*(*(argv+1)+1) == 'f'){
        //flagLeft = flagLeft - 3;
        // printf("f aaaaaaa\n" );
        shortvalue = 0x4000;
        shortvalue = shortvalue | checkEandD(argv);
                printf("%x\n", shortvalue);
        if (argc == 5){
            if(validFmKey(argv,4)){
                return 0x0000;
            }
        key = *(argv + 4);
        }

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
            printf("%x\n come on ,please come3", totalC );
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
            printf("%x\n come on ,please come4", totalC );
            return totalC;
        }
    }
    else{
        return 0x0000;
    }
}

//Incorrect arguments for the specified cipher (e.g. -r or -c being passed in with -f)
//also check if the third argument is -d or -e
int CheckArgument(int argc,char **argv){


    if (argc < 2){
        return 1;
    }

    if(*(*(argv+1)+1) == 'f'){
        if (*(*(argv+2)+1) != 'e' && *(*(argv+2)+1) != 'd' ){
            return 1 ;
        }

        if (argc == 3){
            return 0;

         }

        if(*(*(argv+3)+1) == 'r' || *(*(argv+3)+1) == 'c'){
            return 1;

        }

    }


    return 0;

}

//The number of rows or columns is invalid (i.e. more than 16 or less than 9)
int checkNumberRC(int argc,char **argv){
    if (argc == 3){
        return 0;
    }
    argc = argc -5;
    if(*(*(argv+3)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+4) - '0';
        if (firstR != 9 && firstR!= 1){
            return 1;
        }
        if (firstR == 9){
            return 0;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+4)+1) - '0';
            if(secondR > 6){
                return 1;
            }
        }
    }
    //check for -c
    else if(*(*(argv+3)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+4) - '0';
        if (firstC != 9 && firstC != 1){
            return 1;
        }
        if (firstC == 9){
            return 0;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+4)+1) - '0';
            if (secondC > 6){
                return 1;
            }
        }
    }

    if (argc==0){

        return 0;
    }
// second r and c check
    argc = argc - 2;

    if(*(*(argv+5)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+6) - '0';
        if (firstR != 9 && firstR!= 1){
            return 1;
        }
        if (firstR == 9){
            return 0;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+6)+1) - '0';
            if(secondR > 6){
                return 1;
            }
        }
    }
    //check for -c
    else if(*(*(argv+5)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+6) - '0';
        if (firstC != 9 && firstC != 1){
            return 1;
        }
        if (firstC == 9){
            return 0;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+6)+1) - '0';
            if (secondC > 6){
                return 1;
            }
        }
    }

    if (argc == 0){
        return 0;
    }

    // third r and c check

    if(*(*(argv+7)+1) == 'r'){
        unsigned short firstR = (unsigned short)**(argv+8) - '0';
        if (firstR != 9 && firstR!= 1){
            return 1;
        }
        if (firstR == 9){
            return 0;
        }
        else{
            unsigned short secondR = (unsigned short)*(*(argv+8)+1) - '0';
            if(secondR > 6){
                return 1;
            }
        }
    }
    //check for -c
    else if(*(*(argv+7)+1) == 'c'){
       unsigned short firstC = (unsigned short)**(argv+8) - '0';
        if (firstC != 9 && firstC != 1){
            return 1;
        }
        if (firstC == 9){
            return 0;
        }
        else{
            unsigned short secondC = (unsigned short)*(*(argv+8)+1) - '0';
            if (secondC > 6){
                return 1;
            }
        }
    }


    return 0;

}

// length of the polybius_alphabet
unsigned short PolyLength(char* polybius_alphabet){
    int length = 0;
    char* character = polybius_alphabet;
    while(*character!='\0'){
        //printf("%c\n",*character );
        character++;
        length++;
    }
    return (unsigned short)length;
}


int rowTimesCol(unsigned short hexNumber,unsigned short totalLength){
    unsigned short lastDigit= hexNumber & 0x0F;
    unsigned short SecondlastDigit= hexNumber & 0xF0;
    SecondlastDigit = SecondlastDigit >> 4;
    unsigned short multiple= lastDigit * SecondlastDigit;
    //printf("second digit :  %x\n",SecondlastDigit );
    printf("wrong r and c:%d\n",multiple);
    if (multiple < totalLength ){
         return 1;
    }
    return 0;
}

int validPolyKey(char **argv,int position){
    int positionPlus = 0;
    int positionPLusPLus = 1;
    int polyPosition = 0;
    int istrue = 0;
    char* pPointer = polybius_alphabet;

    // printf("first: %c\n",*(*(argv + position)+positionPlus ));

    while(*(*(argv + position)+positionPlus) != '\0'){
        pPointer = polybius_alphabet;
        while( *pPointer != '\0'  && istrue == 0){
            if (*(*(argv + position)+positionPlus) == *pPointer){
                istrue = 1;
            }
            else{
                pPointer++;
            }

        }
        if (istrue == 0){
            return 1;
        }
        else {
            istrue = 0;
            positionPlus++;
        }
    }

    istrue = 0;
    positionPlus = 0;

    while (*(*(argv + position)+positionPlus) != '\0'){
        positionPLusPLus = 1;
        while(*(*(argv + position) + positionPlus + positionPLusPLus) != '\0'){
            // printf("%d\n",*(*(argv + position) + positionPlus + positionPLusPLus ));
            if (*(*(argv + position)+positionPlus) == *(*(argv + position)+positionPlus+positionPLusPLus) ){

                return 1;
            }
            else{
                positionPLusPLus++;
            }
        }
        positionPlus++;
    }
    key = *(argv + position);
    printf("key is here: %s\n",key );

    return 0;
}


int validFmKey(char **argv,int position){
    int positionPlus = 0;
    int positionPLusPLus = 1;
    int polyPosition = 0;
    int istrue = 0;
    const char* pPointer = fm_alphabet;

    // printf("first: %c\n",*(*(argv + position)+positionPlus ));

    while(*(*(argv + position)+positionPlus) != '\0'){
        pPointer = fm_alphabet;
        while( *pPointer != '\0'  && istrue == 0){
            if (*(*(argv + position)+positionPlus) == *pPointer){
                istrue = 1;
            }
            else{
                pPointer++;
            }

        }
        if (istrue == 0){
            return 1;
        }
        else {
            istrue = 0;
            positionPlus++;
        }
    }

    istrue = 0;
    positionPlus = 0;

    while (*(*(argv + position)+positionPlus) != '\0'){
        positionPLusPLus = 1;
        while(*(*(argv + position) + positionPlus + positionPLusPLus) != '\0'){
            // printf("%d\n",*(*(argv + position) + positionPlus + positionPLusPLus ));
            if (*(*(argv + position)+positionPlus) == *(*(argv + position)+positionPlus+positionPLusPLus) ){

                return 1;
            }
            else{
                positionPLusPLus++;
            }
        }
        positionPlus++;
    }

    return 0;
}

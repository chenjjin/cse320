#include "part3.h"
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


int handleMorseTable(char c);
void fillfmKey(int a);



int tablePos = 0;
char* table = polybius_table;



void tryPART3(unsigned short mode){
    //handle fm_key[] first

    if(key == NULL){
        printf("key is null\n");
        fillfmKey(0);
    }
    else{
        fillfmKey(1);
    }
    char c;
    printf("enter a value: ");
    while((c = getchar())!= EOF){
        while(c != '\n' && c!= EOF){

            handleMorseTable(c);

            c = getchar();
        }

        if (c != EOF && c != ' ' &&  c!='\t'){
            printf("\nenter a value: ");
        }
        else{
            return;
        }


    }
}

int handleMorseTable(char c){

    char a = 33;
    int i = 0;
    int n = 0;
    int tableSize = 0;
    int FtableNumber = 0;
    if(c<32 || c>122){
        exit(EXIT_FAILURE);
    }

    //put xx when space encounter

    if(c == 32 ){
        *(table+tablePos) = 'x';
        tablePos = tablePos+1;
        return 1;

    }

    //CHECK INPUT CHAR POSITION IN MORSE_TABLE
    while (a != c){
        i++;
        a++;
    }

    if(**(morse_table+i) == '\0'){
        exit(EXIT_FAILURE);
    }


    //put char into table with morse code
    while(*(*(morse_table+i)+n) != '\0'){
        *(table+tablePos) = *(*(morse_table+i)+n);
        tablePos++;
        n++;

    }

    //put x to table
    *(table+tablePos) = 'x';
    tablePos++;


    // printf("table0: %s",table);
    while(*(table+tableSize) != '\0'){
        tableSize++;

    }
    // printf("tablesize:%d\n",tableSize );


    int d = 3;
    // tablePos = 0;
    while(tableSize>=3){
        tablePos = 0;
        while(FtableNumber<26){
            if(**(fractionated_table+FtableNumber) == *table){
                if(*(*(fractionated_table+FtableNumber)+1) == *(table+1)){
                    if(*(*(fractionated_table+FtableNumber)+2) == *(table+2)){
                        break;
                    }
                }
            }
            FtableNumber++;
                // printf("%d",FtableNumber );


        }
        // printf("this is table1: %s\n",table );
        printf("%c",*(fm_key+FtableNumber));

        if(*(table+3)=='\0'){
            *(table)='\0';
            *(table+1)= '\0';
            *(table+2)='\0';
        }
        else if(*(table+4)=='\0'){
            *(table)=*(table+3);
            *(table+1)= '\0';
            *(table+2)='\0';
            *(table+3) = '\0';
            tablePos ++;
        }
        else if (*(table+5)=='\0'){
            *(table)=*(table+3);
            *(table+1)= *(table+4);
            *(table+2)='\0';
            *(table+3) = '\0';
            *(table+4) = '\0';
            tablePos = tablePos+2;
        }
        else{
        while(*(table+d)!='\0'){
            *(table+tablePos) = *(table+d);
            *(table+d)='\0';
            d++;
            tablePos++;
        }
        }
        tableSize = tableSize -3;
        FtableNumber = 0;
        // printf("this is table2: %s\n",table );


        // printf("TRANSFER NUMBER%d\n",FtableNumber);
    }





    return 1;
}

void fillfmKey(int a){
    int number  = 0;
    int keynumber = 0;
    int keyPositionNumber = 0;
    char *keyPosition = fm_key;
     if(a){
         while(*(key+number) != '\0'){
            *(keyPosition+number) = *(key + number);
            number++;
            keyPositionNumber++;
         }
         number = 0;
         while(keyPositionNumber <= 26){
            keynumber = 0;
            while(*(key+keynumber) != '\0'){
                  // printf("this is number: %d\n",number );
                if(*(key+keynumber) == *(fm_alphabet+number) ){
                    // printf("this is the wronf char: %c\n",*(fm_alphabet+number) );
                    number++;
                    keynumber=0;
                    continue;
                }
                keynumber++;
            }
            *(keyPosition+keyPositionNumber) = *(fm_alphabet+number);
            number++;
            keyPositionNumber++;
         }
     }

     else{
        printf("you sholluld be heere\n" );
        while(number <= 26 ){
            *(keyPosition+number) = *(fm_alphabet+number);
            number++;
        }
     }
     printf("fm_key:%s\n", fm_key);
}
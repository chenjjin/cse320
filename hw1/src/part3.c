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


int decryptMorse(char c);
void encryptMorse(char c);
void fillfmKey(int a);
void translate();


int tablePos = 0;
char* table = polybius_table;
int Flag = 0 ;



void tryPART3(unsigned short mode){
    //handle fm_key[] first

    if(key == NULL){
        fillfmKey(0);
    }
    else{
        fillfmKey(1);
    }
    char c;

    unsigned short type = mode >> 13;
    type = type & 0x0001;
    // printf("value type%x\n",type );

    // printf("enter a value: ");
    while((c = getchar())!= EOF){
        tablePos=0;
        Flag=0;
        while(c != '\n' && c!= EOF){
            if (!type){
                decryptMorse(c);
            }
            else{
                encryptMorse(c);
            }

            c = getchar();
        }

    if (!type){
        if(c=='\n'){
            // printf("this is table at last %s\n",table );
            if(*(table+1)!='\0'){
                *(table+2)='x';
                int FtableNumber=0;
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
            }
        }
    }

        if (c != EOF ){
            printf("\n");
        }
        else{
            return;
        }


    }
}

int decryptMorse(char c){

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
        if(!Flag){
        *(table+tablePos) = 'x';
        tablePos = tablePos+1;
        Flag=1;
        return 1;
        }
    return 1;
    }
    else{
        Flag=0;
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
        while(FtableNumber<sizeof(fm_key)){
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
        d=3;
        tableSize = tableSize -3;
        FtableNumber = 0;
         // printf("this is table2: %s\n",table );


        // printf("TRANSFER NUMBER%d\n",FtableNumber);
    }





    return 1;
}


void encryptMorse(char c){
    //find the position of char
    int fmNumber = 0;
    while(*(fm_key+fmNumber) != c){
        fmNumber++;
    }
    int n = 0; //start posiiton of the char in fractionated_table
    while(*(*(fractionated_table+fmNumber)+n) != '\0'){
        // printf("1\n");
        if (*(*(fractionated_table+fmNumber)+n) == 'x'){
            if (Flag){
                printf(" ");
                n++;
                continue;
            }
            Flag = 1;
            translate();
            n++;
            continue;
        }
        *(table+tablePos) = *(*(fractionated_table+fmNumber)+n);
        tablePos++;
        n++;
        Flag = 0;
    }

}

void translate(){
    int tableNumber = 0;
    int FratableNumber = 0;

    while(FratableNumber < 89){
        // printf("table:%s\n",table );
        while(*(table+tableNumber)!='\0'){

            if(*(table+tableNumber) == *(*(morse_table+FratableNumber)+tableNumber)){
                tableNumber++;
            }
            else{
                tableNumber = 0;
                break;
            }
        }
        if((*(table+tableNumber)=='\0') && *(*(morse_table+FratableNumber)+tableNumber) == '\0'){
            tableNumber = 0;
            while(*(table+tableNumber)!='\0'){
                *(table+tableNumber)='\0';
                tableNumber++;

            }

            break;
        }
        tableNumber = 0;
        FratableNumber++;
    }
    tablePos = 0;
    // printf("FratableNumber:%d\n",FratableNumber );
    printf("%c",FratableNumber+33 );
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
        // printf("you sholluld be heere\n" );
        while(number <= 26 ){
            *(keyPosition+number) = *(fm_alphabet+number);
            number++;
        }
     }
     // printf("fm_key:%s\n", fm_key);
}
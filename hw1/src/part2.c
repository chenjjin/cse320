#include "part2.h"
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
void putArray(char* array,char* poly_alpha);
unsigned short colValue(unsigned short mode);
unsigned short rowValue(unsigned short mode);
unsigned short counterPosition(char c,char* array,unsigned short mode);
void encrypt(unsigned short col,unsigned short row,unsigned short position);
void putmoreArray(char* array,char* poly_alpha,const char *key);




void try(unsigned short mode){
    char c;
    printf("key value:%s\n",key );
    if(key == NULL){
        putArray(polybius_table,polybius_alphabet);
    }
    else{
        putmoreArray(polybius_table,polybius_alphabet,key);
        int i = 0;
        // while((polybius_table[i])!='\0'){
        //     printf("%c", polybius_table[i]);
        //     i++;
        // }
    }
    printf("enter a value: ");
    while((c = getchar())!= EOF){
        while(c != '\n' && c!= EOF){
            // printf("char: %c\n",c );
            // putchar(c);
            if(c==' '){
                break;
            }
            else if (c == '\t'){
                break;
            }
            unsigned char position = counterPosition( c, polybius_table,mode);
            encrypt(colValue(mode),rowValue(mode),position);

            c = getchar();
        }
        if (c != EOF && c != ' ' &&  c!='\t'){
            printf("\nenter a value: ");
        }
        else if(c == ' '){
            printf("%c",c);
        }
        else if(c=='\t'){
            printf("%c",c);
        }
        else{
            return;
        }
    }

    return ;
}

void putArray(char* array,char* poly_alpha){
    while(*poly_alpha != '\0'){
        *array = *poly_alpha;
        array++;
        poly_alpha++;
    }


}

void putmoreArray(char* array,char* poly_alpha,const char *key){
    const char *checkKey = key;

    while(*checkKey != '\0'){
        *array = *checkKey;
        array++;
        checkKey++;
    }
    while(*poly_alpha != '\0'){
        checkKey = key;
        while(*checkKey != '\0'){
            if(*poly_alpha == *checkKey){
                poly_alpha++;
                checkKey = key;
            }
            checkKey++;
        }
        *array = *poly_alpha;
        array++;
        poly_alpha++;
    }



}

unsigned short colValue(unsigned short mode){
    unsigned short col = mode & 0x0F;
    // printf("this is col:%d\n",col );
    return col;
}

unsigned short rowValue(unsigned short mode){
    unsigned short row = mode & 0xF0;
    row = row >> 4;
    // printf("this is row: %d\n",row );
    return row;
}

unsigned short counterPosition(char c,char* array,unsigned short mode){
    unsigned short position = 0;
    unsigned short col = colValue(mode);
    unsigned short row = rowValue(mode);
    unsigned short total = col * row;
    while(*array != c){
        array++;
        position++;
        if(position >total){
            exit(EXIT_FAILURE);
        }
    }
    return position;
}

void encrypt(unsigned short col,unsigned short row, unsigned short position){

    unsigned short quotient = position / col;
    unsigned short remainder = position % col;
    char quo;
    char rem;
    if (quotient<10){
        quo = quotient +48;
    }
    else{
        quo = quotient + 55;
    }

    if (remainder<10){
        rem = remainder +48;
    }
    else{
        rem = remainder + 55;
    }

    printf("%c%c",quo,rem );


}
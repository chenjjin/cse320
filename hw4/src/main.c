#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <signal.h>

#include "sfish.h"
#include "debug.h"

void asigintHandler(int sig_num);
void bsigintHandler(int sig_num);
void handler(int sign);
// char**
char* input;
pid_t processGroup[10];
int return_result;
char processName[10][10];
int list[10];



int main(int argc, char *argv[], char* envp[]) {


    signal(SIGINT, asigintHandler);
    // signal(SIGTSTP,bsigintHandler);

    struct sigaction sa;

    sa.sa_handler=handler;
    if(sigaction(SIGTSTP,&sa,NULL) == -1){
        perror("sigaction");
    }


    bool exited = false;

    char my_prompt[1024];
    // char new_promp[1024];

           // printf("\033[1;31m");
    self_promp(my_prompt);


        // printf("\033[0m");

    if(!isatty(STDIN_FILENO)) {
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }
    }

    do {
         // signal(SIGINT, asigintHandler);



        input = readline(my_prompt);

        //         strcpy(new_promp,my_prompt);
        // strcpy(my_prompt,"\e[1;31m");
        // strcat(my_prompt,new_promp);
        // strcat(my_prompt,"\e[m");

        // write(1, "\e[s", strlen("\e[s"));
        // write(1, "\e[20;10H", strlen("\e[20;10H"));
        // write(1, "SomeText", strlen("SomeText"));
        // write(1, "\e[u", strlen("\e[u"));

        // If EOF is read (aka ^D) readline returns NULL
        if(input == NULL) {
            continue;
        }
        return_result = eval(input,my_prompt);

        if (return_result == -1){
            break;
        }
        if(return_result == 2){
            int size = 0;
            while(list[size]!=0){
                size++;
            }
            for(int i= 0;i<size;i++){
                printf(JOBS_LIST_ITEM, i,processName[i]);
            }
        }
        if(return_result == 3){
            char* token;
            char* parameter = " ";
            // int number ;
            token = strtok(input, parameter);
            token = strtok(NULL, parameter);
            token = token+1;

            // number = atoi (token);
            // debug("this is string: %i\n", number);
            int size = 0;
            while(processGroup[size]!=0){
                size++;
            }
            for(int i= 0;i<size;i++){
                kill(processGroup[i],SIGCONT);
            }

        }

        if(return_result ==4){
            int modula = 0;
            for (int i = 0;input[i]!= 0;i++){
                if(input[i] == '%'){
                    modula = 1;
                }
            }
            if (modula == 1){
                int size = 0;
                while(processGroup[size]!=0){
                    size++;
                }
                for(int i= 0;i<size;i++){
                    kill(processGroup[i],SIGKILL);
                }
            }

        }


        // Currently nothing is implemented
        // printf(EXEC_NOT_FOUND, input);

        // You should change exit to a "builtin" for your hw.
        // exited = strcmp(input, "exit") == 0;

        // Readline mallocs the space for input. You must free it.
        rl_free(input);

    } while(!exited);

    debug("%s", "user entered 'exit'");

    return EXIT_SUCCESS;
}

void asigintHandler(int sig_num)
{
    pid_t currennt_process = getpgrp();
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigprocmask(SIG_BLOCK,&mask,&prev_mask);

    signal(SIGINT, asigintHandler);

    if(currennt_process<0){
        kill(currennt_process,SIGKILL);
        printf("\n program terminated using Ctrl+C\n");
    }
    // exit(0);
    // fflush(stdin);
    // else{
    //     printf("\r\n");
    // }

    fflush(stdout);

    sigprocmask(SIG_BLOCK,&prev_mask,NULL);

}

// void bsigintHandler(int sig_num)
// {

//     pid_t currennt_process = getpgrp();
//     sigset_t mask, prev_mask;
//     sigemptyset(&mask);
//     sigaddset(&mask,SIGTSTP);
//     sigprocmask(SIG_BLOCK,&mask,&prev_mask);

//     signal(SIGTSTP, asigintHandler);
//     main(0,0,0);

//     // if(currennt_process<0){
//         kill(currennt_process,SIGHUP);
//         printf("\n program terminated using Ctrl+Z\n");
//      // }
//     // exit(0);
//     // fflush(stdin);
//     // else{
//     //     printf("\r\n");
//     // }
//      // kill(currennt_process,SIGKILL);
//     fflush(stdout);

//     sigprocmask(SIG_BLOCK,&prev_mask,NULL);
// }




void bsigintHandler(int sig_num)
{

    pid_t currennt_process = getpgrp();
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGTSTP);
    sigprocmask(SIG_BLOCK,&mask,&prev_mask);

    signal(SIGTSTP, asigintHandler);

    if(currennt_process<0){
        // kill(currennt_process,SIGTSTP);
        // printf("\n program terminated using Ctrl+Z\n");
     // }
    // exit(0);
    // fflush(stdin);
    // else{
    //     printf("\r\n");
     }
     // kill(currennt_process,SIGKILL);
    fflush(stdout);

    sigprocmask(SIG_BLOCK,&prev_mask,NULL);

}


void handler(int sign){
    char string[10];
    int size = 0;
    int i = 1;
    // pid_t currennt_process = getpgrp();
    debug("this process id is %d\n",currennt_process );
    while(list[size]!=0){
        size = size+1;
    }
    list[size] = size;
    while(input[i]!=' '){
        i++;
    }
    strncpy(string,input,i);
    strcpy(processName[size],string);
    debug("this is input:%s\n", string);


    printf("\n");
}


// int builtin_command(char** argv){
//     if (!strcmp(argv[0], "jobs")) {
//         return 1;
//     }

//     if(!strcmp(argv[0], "fg")){
//         return 1;
//     }

//     if(!strcmp(argv[0], "kill")){
//         return 1;

//     }

//     return 0;
// }
#include "sfish.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



#define MAXARGS 128
int argc;
char bufArray[1024];
int parseLine(char *buf, char **argv);
int builtin_command(char** argv);
void help_print();
void self_pwd(char* bufArray);
void self_cd(char** argv);
char *new_prompt;
char previous_direc[1024];
void Executables(char* input,char** argv);
char* getpath(char* argv);
int slashexit(char* first_argv);
int check_execut(char* first_argv);

int eval(char* input,char* prompt){
    char *argv[MAXARGS];
    char* buf = strdup(input);
    new_prompt = prompt;
    argc = parseLine(buf, argv);
    if (argv[0] == NULL)
        return 0;

    else if (strcmp(argv[0],"exit") == 0){
        return -1;
    }

    else if(builtin_command(argv)!=1){
        Executables(input,argv);
    }

    return 0;
}

void Executables(char* input,char** argv){
    pid_t pid;
    char* path = strdup(argv[0]);
    int status;

    if(slashexit(argv[0]) == 1){
        if(check_execut(argv[0]) == 0){
            printf(EXEC_ERROR,argv[0]);
            return;
        }
    }

    else{
        path = getpath(argv[0]);
        if(path == NULL){
            printf(EXEC_NOT_FOUND,argv[0]);
            return;
        }
    }


    if((pid = fork()) == 0){
    int execv_return = execv(path,argv);
        // printf("%s\n","wtf???");
    if(execv_return < 0){
        printf(EXEC_ERROR,"cannot execute! " );
    }
        exit(0);
    }

    else{
        wait(&status);
    }



}

int check_execut(char* first_argv){
    struct stat buf;
    if(stat(first_argv,&buf) == 0){
        return 1;
    }
    return 0;

}

int slashexit(char* first_argv){
    int length = strlen(first_argv);
    for(int i = 0;i <= length;i++){
        if(*first_argv == '/'){
            return 1;
        }
        first_argv +=1;
    }
    return 0;
}
char* getpath(char* argv){
    char* env_path = strdup(getenv("PATH"));
    char* env_path_dup = strdup(env_path);
    int env_length = strlen(env_path);
    // int argv_length = strlen(argv);
    struct stat buf;
    int begin = 0;
    int size_of_tar = 0;
    // size_t total_size = 0;
    // printf("???%s\n",env_path_dup );
    for(int i = 0;i<=env_length;i++){
        if(env_path[i] == ':' || env_path[i] == '\0'){
            // last = i;
            size_of_tar = i - begin;
            // total_size = argv_length + size_of_tar + 2;

            char* new_path = malloc(1024);
            // printf("env_dup:%s\n", env_path_dup);
            memset(new_path,'\0',1024);
            strncpy(new_path,env_path_dup,size_of_tar);

            // printf("path before ls:%s\n", new_path);
            strcat(new_path,"/");
            // printf("path before ls:%s\n", new_path);
            strcat(new_path,argv);
            // printf("argv:%s\n", argv);

            // printf("path being test:%s\n", new_path);
            if(stat(new_path,&buf) == 0){
                return new_path;
            }
            else{
                begin = i+1;
                env_path_dup += size_of_tar + 1;
                free(new_path);
            }


        }
    }
    return NULL;
}

int builtin_command(char** argv){
    if (!strcmp(argv[0], "help")) {
        help_print();
        return 1;
    }

    if(!strcmp(argv[0], "pwd")){
        self_pwd(bufArray);
        printf("%s\n", bufArray);
        return 1;
    }

    if(!strcmp(argv[0], "cd")){
        self_cd(argv);
        self_promp(new_prompt);
        return 1;

    }

    return 0;
}

void self_promp(char* mypromp){
    char newPromp[1024];
    self_pwd(bufArray);
    strcpy(mypromp,bufArray);
    char* env = getenv("HOME");
    int length = strlen(env);
    if(strlen(mypromp) == length){
        strcpy(mypromp,"~");
    }
    else if(strlen(mypromp) > length){
        strcpy(newPromp, mypromp);
        strcpy(mypromp,"~");
        memmove(mypromp+1,newPromp+length,strlen(newPromp));
    }


    // char* token = strtok(mypromp, env);
    //  printf( "this is token: %s\n", token );
    strcat(mypromp," :: CHJJIN >>");
}


void self_cd(char** argv){
    char* path = argv[1];
    //only cd

    if(path == NULL){

        self_pwd(bufArray);
        strcpy(previous_direc,bufArray);
        chdir(getenv("HOME"));
        return;

    }

    if(strcmp(path,".") == 0){
        self_pwd(bufArray);
        strcpy(previous_direc,bufArray);
        return;
    }

    if(strcmp(path,"..") == 0){
        self_pwd(bufArray);
        strcpy(previous_direc,bufArray);
        // if(chdir("..") == -1){
        //     fprintf(stderr,"can't access parent \n");
        // }
    }

    if(strcmp(path,"-") == 0){
        if(previous_direc == NULL){
            fprintf(stderr,"No previous directory \n");
        }
        else{
            self_pwd(bufArray);
            chdir(previous_direc);
            strcpy(previous_direc,bufArray);
        }
    }
    else{
        self_pwd(bufArray);
        strcpy(previous_direc,bufArray);
        if ((chdir(path)) == -1){
            fprintf(stderr,"No such file or directory \n");
        }

    }
}

void self_pwd(char* bufArray){
    getcwd(bufArray,1024);
}
int parseLine(char *buf, char **argv){
    char *delim;
    int argc;

    buf[strlen(buf)] = ' ';
    while (*buf && (*buf == ' '))
        buf++;

    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))
            buf++;
     }
     argv[argc] = NULL;

     if (argc == 0)
        return 1;

    return argc;

}

void help_print(){
    printf("%s\n", "help:Print a list of all builtins and their basic usage in a single column. ");
    printf("%s\n", "exit: Exits the shell ");
    printf("%s\n", "cd: Changes the current working directory of the shell");
    printf("%s\n", "pwd: Prints the absolute path of the current working directory.");
}
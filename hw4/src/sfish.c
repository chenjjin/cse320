#include "sfish.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "debug.h"




#define MAXARGS 128
int argc;
char bufArray[1024];
char *new_prompt;
char previous_direc[1024];
char color[10];
char actual_color[4];


int parseLine(char *buf, char **argv);
int builtin_command(char** argv);
void help_print();
void self_pwd(char* bufArray);
void self_cd(char** argv);
void Executables(char* input,char** argv);
char* getpath(char* argv);
int slashexit(char* first_argv);
int check_execut(char* first_argv);
int redirection_exist(char* input);
void Redirection(char* input,char** argv);
char check_direction_char(char* input);
char** parse_redirection_argument(char* input,char** directionList,char* parameter );
char* getfile_path(char* path);
int check_buildin_commond(char* execute_path);
int check_less_brefore_greater(char* input);
int check_greater_brefore_less(char* input);
void execute_pipe(int in,int out,char** first_token_list);
void handle_pipe(int array_size, char** directionList);
int build_process_command(char** argv);


int eval(char* input,char* prompt){
    // previous_direc[0] = '\0';
    char *argv[MAXARGS];
    char* buf = strdup(input);
    new_prompt = prompt;
    argc = parseLine(buf, argv);
    int return_value=0;
    if (argv[0] == NULL)
        return 0;

    else if (strcmp(argv[0],"exit") == 0){
        return -1;
    }
    else if (redirection_exist(input)){
        Redirection(input,argv);
    }
    else if(( return_value = build_process_command(argv)) > 1){
        return return_value;
    }
    else if(builtin_command(argv)!=1){
        Executables(input,argv);
    }

    return 0;
}


void Redirection(char* input,char** argv){


    if(check_less_brefore_greater(input) == 1){

        debug("%s\n", "it is rerally ok mow dude");
        char** directionList = malloc(sizeof(char*));
        char* parameter = "<";
        char* space = " ";
        char* greater = ">";
        directionList=parse_redirection_argument(input,directionList,parameter);
        char* first_token = directionList[0];
        char* second_token  = directionList[1];
        debug("this is first token:%s\n", first_token);
        debug("this is second token:%s\n", second_token);

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);

        char** second_token_list = malloc(sizeof(char*));
        second_token_list = parse_redirection_argument(second_token,second_token_list,greater);

        char** second_argv = malloc(sizeof(char*));
        second_argv = parse_redirection_argument(second_token_list[0],second_argv,space);

        char** third_argv = malloc(sizeof(char*));
        third_argv = parse_redirection_argument(second_token_list[1],third_argv,space);
        debug("this is first token:%s\n", first_token);
        debug("this is second argv token:%s\n", second_argv[0]);
        debug("this is third argv token:%s\n", third_argv[0]);



        char* input_path = getfile_path(second_argv[0]);
        debug("this is first_token_list[0]:%s\n", first_token_list[0]);

        int inputfile = open(input_path,O_RDONLY);

        char* output_path = getfile_path(third_argv[0]);

        int outputfile = open(output_path,O_WRONLY|O_CREAT|O_TRUNC);


        if(inputfile == -1 ){
            printf(EXEC_ERROR, "error when open inputfile");
        }
        if(outputfile ==-1){
            printf(EXEC_ERROR, "error when open outputfile");
        }


        int status;
        pid_t pid;

        if((pid = fork()) == 0){
            dup2(inputfile,STDIN_FILENO);
            dup2(outputfile, STDOUT_FILENO);
            close(inputfile);
            close(outputfile);
            // printf("%s\n", "you can never be here");
            // printf("this is :%s\n", first_token_list[0]);
            // printf("first:%s\n", first_token_list[2]);

                char* execute_path = getpath(first_token_list[0]);
                int execv_return = execv(execute_path,first_token_list);
                if(execv_return == -1){
                    printf(EXEC_ERROR,"cannot execute! " );
                    exit(0);
                }

        }
        else{
            wait(&status);
        }
        free(directionList);
        free(first_token_list);
        free(second_token_list);
        free(second_argv);
        free(third_argv);

    }


    /////////////////////////////////////////////////////////////////////////////////
    else if(check_greater_brefore_less(input) == 1){

        debug("%s\n", "it is rerally ok mow dude");
        char** directionList = malloc(sizeof(char*));
        char* parameter = ">";
        char* space = " ";
        char* greater = "<";
        directionList=parse_redirection_argument(input,directionList,parameter);
        char* first_token = directionList[0];
        char* second_token  = directionList[1];
        debug("this is first token:%s\n", first_token);
        debug("this is second token:%s\n", second_token);

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);

        char** second_token_list = malloc(sizeof(char*));
        second_token_list = parse_redirection_argument(second_token,second_token_list,greater);

        char** second_argv = malloc(sizeof(char*));
        second_argv = parse_redirection_argument(second_token_list[0],second_argv,space);

        char** third_argv = malloc(sizeof(char*));
        third_argv = parse_redirection_argument(second_token_list[1],third_argv,space);
        debug("this is first token:%s\n", first_token);
        debug("this is second argv token:%s\n", second_argv[0]);
        debug("this is third argv token:%s\n", third_argv[0]);



        char* input_path = getfile_path(third_argv[0]);
        debug("this is first_token_list[0]:%s\n", first_token_list[0]);

        int inputfile = open(input_path,O_RDONLY);

        char* output_path = getfile_path(second_argv[0]);

        int outputfile = open(output_path,O_WRONLY|O_CREAT|O_TRUNC);


        if(inputfile == -1 ){
            printf(EXEC_ERROR, "error when open inputfile");
        }
        if(outputfile ==-1){
            printf(EXEC_ERROR, "error when open outputfile");
        }


        int status;
        pid_t pid;

        if((pid = fork()) == 0){
            dup2(inputfile,STDIN_FILENO);
            dup2(outputfile, STDOUT_FILENO);
            close(inputfile);
            close(outputfile);
            // printf("%s\n", "you can never be here");
            // printf("this is :%s\n", first_token_list[0]);
            // printf("first:%s\n", first_token_list[2]);

                char* execute_path = getpath(first_token_list[0]);
                int execv_return = execv(execute_path,first_token_list);
                if(execv_return == -1){
                    printf(EXEC_ERROR,"cannot execute! " );
                    exit(0);
                }

        }
        else{
            wait(&status);
        }
        free(directionList);
        free(first_token_list);
        free(second_token_list);
        free(second_argv);
        free(third_argv);

    }
    ///////////////////////////////////////////////////////////////////////////////
    else if(check_direction_char(input) == '>'){
        char** directionList = malloc(sizeof(char*));
        char* parameter = ">";
        char* space = " ";
        directionList=parse_redirection_argument(input,directionList,parameter);
        char* first_token = directionList[0];
        char* second_token  = directionList[1];
        // free(directionList);
        debug("this is first token:%s\n", first_token);
        debug("this is second token:%s\n", second_token);

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);

        char** second_token_list = malloc(sizeof(char*));
        second_token_list = parse_redirection_argument(second_token,second_token_list,space);

        char* output_path = getfile_path(second_token_list[0]);
        debug("this is out put path:%s\n", output_path);
        debug("this is first_token_list[0]:%s\n", first_token_list[0]);

        int file = open(output_path,O_WRONLY|O_CREAT|O_TRUNC);
        if(file == -1){
            printf(EXEC_ERROR, "error when open file");
        }
        int status;
        pid_t pid;

        if((pid = fork()) == 0){
            dup2(file,STDOUT_FILENO);
            close(file);
            // int build_Comand = check_buildin_commond(first_token_list[0]);
            if(builtin_command(first_token_list) == 1){
                exit(1);
            }
            else{
                char* execute_path = getpath(first_token_list[0]);
                int execv_return = execv(execute_path,first_token_list);
                if(execv_return == -1){
                    printf(EXEC_ERROR,"cannot execute! " );
                    exit(0);
                }
            }
        }
        else{
            wait(&status);

        }
        free(directionList);
        free(first_token_list);
        free(second_token_list);

    }

/////////////////////////////////////////////////////////////////////////////////
    else if(check_direction_char(input) == '<'){
        char** directionList = malloc(sizeof(char*));
        char* parameter = "<";
        char* space = " ";
        directionList=parse_redirection_argument(input,directionList,parameter);
        char* first_token = directionList[0];
        char* second_token  = directionList[1];
        // free(directionList);
        debug("this is first token:%s\n", first_token);
        debug("this is second token:%s\n", second_token);

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);

        char** second_token_list = malloc(sizeof(char*));
        second_token_list = parse_redirection_argument(second_token,second_token_list,space);

        char* input_path = getfile_path(second_token_list[0]);
        debug("this is in put path:%s\n", input_path);
        debug("this is first_token_list[0]:%s\n", first_token_list[0]);

        int file = open(input_path,O_RDONLY);
        if(file == -1){
            printf(EXEC_ERROR, "file not exit");
        }

        int status;
        pid_t pid;

        if((pid = fork()) == 0){
            dup2(file,STDIN_FILENO);
            close(file);
            // int build_Comand = check_buildin_commond(first_token_list[0]);

                char* execute_path = getpath(first_token_list[0]);
                int execv_return = execv(execute_path,first_token_list);
                if(execv_return == -1){
                    printf(EXEC_ERROR,"cannot execute! " );
                    exit(0);
                }
        }
        else{
            wait(&status);

        }
        free(directionList);
        free(first_token_list);
        free(second_token_list);

    }

//////////////////////////////////////////////////////////////////////////////////
    if(check_direction_char(input) == '|'){
        int status;
        pid_t pid;
        char** directionList = malloc(sizeof(char*));
        char* parameter = "|";
        // char* space = " ";
        directionList=parse_redirection_argument(input,directionList,parameter);

        int array_size  = 0;
        while(directionList[array_size]!= NULL){
            array_size++;
        }
        debug("array size:%i\n",array_size );

        if((pid = fork()) == 0){
                // if(builtin_command(first_token_list) == 1){
                //     exit(1);
                    // }



                handle_pipe(array_size,directionList);
                exit(0);

            }


        else{
            wait(&status);

        }
        free(directionList);


        }

}

void handle_pipe(int array_size, char** directionList){
    char* space = " ";
    int fd[2];


    int in_file = 0 ;
    int out_file = 0;
    for (int i = 0;i<array_size-1;i++){
        char* first_token = directionList[i];

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);
        debug("first_token_list:%s\n", first_token_list[0]);
        pipe(fd);
        out_file = fd[1];
        execute_pipe(in_file,out_file,first_token_list);
        close(fd[1]);
        in_file = fd[0];
    }
    if(in_file!=STDIN_FILENO){
        dup2(in_file,STDIN_FILENO);
        close(in_file);
        char* first_token = directionList[array_size-1];

        char** first_token_list = malloc(sizeof(char*));
        first_token_list = parse_redirection_argument(first_token,first_token_list,space);
        char* execute_path = getpath(first_token_list[0]);
        execv(execute_path,first_token_list);
    }

}

void execute_pipe(int in_file,int out_file,char** first_token_list){
    // int status;
    pid_t pid;
    char* execute_path;
    if((pid = fork()) == 0){
        if(in_file!=STDIN_FILENO){
          dup2(in_file,STDIN_FILENO);
          close(in_file);
        }
        if(out_file!= STDOUT_FILENO){
            dup2(out_file,STDOUT_FILENO);
            close(out_file);
        }

        // if(builtin_command(first_token_list) == 1){
        //     // if (strcmp(first_token_list[0], "help") == 0) {
        //     //     help_print();
        //     // }

        //     // if(!strcmp(first_token_list[0], "pwd")){
        //     //     self_pwd(bufArray);
        //     //     printf("%s\n", bufArray);
        //     // }
        // }

        // else{
            execute_path = getpath(first_token_list[0]);
            int execv_return = execv(execute_path,first_token_list);
            if(execv_return == -1){
                printf(EXEC_ERROR,"cannot execute! " );
                exit(0);
            // }
        }


    }


}



int check_greater_brefore_less(char* input){
    int length = strlen(input);
    int greater = 0;
    int less = 0;
    for(int i =0;i<length;i++){
        if(*(input+i) == '>' ){
            greater = i;
        }
        else if(*(input+i) == '<'){
            less  = i;
        }
    }

    if(greater<less  && less !=0 && greater!=0){
        return 1;
    }
    return 0;
}


int check_less_brefore_greater(char* input){
    int length = strlen(input);
    int greater = 0;
    int less = 0;
    for(int i =0;i<length;i++){
        if(*(input+i) == '>' ){
            greater = i;
        }
        else if(*(input+i) == '<'){
            less  = i;
        }
    }

    if(less<greater  && less !=0 && greater!=0){
        return 1;
    }
    return 0;
}

int check_buildin_commond(char* execute_path){
    char help[5];
    strcpy(help,"help");
    char pwd[4];
    strcpy(pwd,"pwd");
    if(strcmp(execute_path,help) == 0){
        return 1;
    }
    if(strcmp(execute_path,pwd) == 0){
        return 1;
    }
    return 0;
}
char* getfile_path(char* path){
    int flag = 0;
    int length = strlen(path);
    char* filepath = NULL;
    for (int i =0;i<length;i++){
        if(*(path+i) == '/'){
            flag = 1;
        }
    }
    if(flag == 0){
        self_pwd(bufArray);
        filepath = bufArray;
        filepath = strcat(filepath,"/");
        filepath = strcat(filepath,path);
    }
    return filepath;
}

char** parse_redirection_argument(char* input,char** directionList,char* parameter ){
    char* token;
    int total_size = 0;

    token = strtok(input, parameter);
    while(token!= NULL){
        total_size = total_size+1;
        directionList = realloc(directionList,total_size*sizeof(char*));
        directionList[total_size-1] = token;

        token = strtok(NULL, parameter);
    }

    directionList[total_size] = 0;      //null pointer
    return directionList;

}

char check_direction_char(char* input){
    int length = strlen(input);
    for(int i =0;i<length;i++){
        if(*(input+i) == '>' ){
            return '>';
        }
        else if(*(input+i) == '<'){
            return '<';
        }
        else if(*(input+i) == '|'){
            return '|';
        }
    }
    return 0;
}
int redirection_exist(char* input){
    int length = strlen(input);
    for(int i =0;i<length;i++){
        if(*(input+i) == '>' || *(input+i) == '<'  || *(input+i) == '|'){
            return 1;
        }
    }
    return 0;
}

void Executables(char* input,char** argv){
    pid_t pid;
    char* path = strdup(argv[0]);
    int status ;

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
    if(!strcmp(argv[0], "color")){
        strcpy(color , "a");
        strcpy(actual_color,argv[1]);
        self_promp(new_prompt);

        return 1;
    }

    return 0;
}

void self_promp(char* mypromp){
     // printf("\033[1;31m");
    // *color = 0;
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
    // printf("\033[0m");
    if(!strcmp(color,"a")){
        if(!strcmp(actual_color,"RED")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;31m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"GRN")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[[1;32m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"YEL")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;33m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"BLU")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;34m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"MAG")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;35m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"CYN")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;36m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"WHT")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[1;37m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
        if(!strcmp(actual_color,"BWN")){
            strcpy(newPromp,mypromp);
            strcpy(mypromp,"\e[0;33m");
            strcat(mypromp,newPromp);
            strcat(mypromp,"\e[m");
        }
    }
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
            printf(BUILTIN_ERROR,"No previous directory");
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
            printf(BUILTIN_ERROR,"No such file or directory");
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


int build_process_command(char** argv){
    if (!strcmp(argv[0], "jobs")) {
        return 2;
    }

    if(!strcmp(argv[0], "fg")){
        return 3;
    }

    if(!strcmp(argv[0], "kill")){
        return 4;

    }

    return 0;
}
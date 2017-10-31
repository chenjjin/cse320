#ifndef SFISH_H
#define SFISH_H
#include <stdio.h>
#include <sys/stat.h>



/* Format Strings */
#define EXEC_NOT_FOUND "sfish: %s: command not found\n"
#define JOBS_LIST_ITEM "[%d] %s\n"
#define STRFTIME_RPRMT "%a %b %e, %I:%M%p"
#define BUILTIN_ERROR  "sfish builtin error: %s\n"
#define SYNTAX_ERROR   "sfish syntax error: %s\n"
#define EXEC_ERROR     "sfish exec error: %s\n"



int eval(char* input,char* prompt);
void self_promp(char* mypromp);
#endif

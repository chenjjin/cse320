#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int opterr;
int optopt;
int optind;
char *optarg;

state_t *program_state;

  const char *  STR_UTF16BE  = "UTF16BE";
  const char *  STR_UTF16LE = "UTF16LE";
  const char  * STR_UTF8  = "UTF8";


void
parse_args(int argc, char *argv[])
{
  int i;
  char option;
  // char *joined_argv;

  // joined_argv = join_string_array(argc, argv);
  // info("argc: %d argv: %s", argc, joined_argv);
  // free(joined_argv);
  int checknegetiveone = 0;

  program_state = Calloc(1, sizeof(state_t));
  for (i = 0; optind < argc; ++i) {
    debug("%d opterr: %d", i, opterr);
    debug("%d optind: %d", i, optind);
    debug("%d optopt: %d", i, optopt);
    debug("%d argv[optind]: %s", i, argv[optind]);
    int flag = optind;
    if ((option = getopt(argc, argv, "he:")) != -1) {
      checknegetiveone = 1;
      switch (option) {
        case 'e': {
          // printf("%s\n","e" );
          info("Encoding Argument: %s", optarg);
          if ((program_state->encoding_to = determine_format(optarg)) == 0)
            // goto errorcase;
            print_state();
          break;

        }
        case '?': {
            fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM,
                    optopt);
            USAGE(argv[0]);
          exit(1);

          break;

        }
        case 'h':{
          // printf("%s\n", "123");
          USAGE(argv[0]);
          exit(0);
        }
        default: {
          break;
        }
      }
    }

    elsif(argv[optind = flag] != NULL)
    {
      if(!checknegetiveone){
        USAGE(argv[0]);
          exit(1);
      }
      if (program_state->in_file == NULL) {
        program_state->in_file = argv[optind];
      }
      elsif(program_state->out_file == NULL)
      {
        program_state->out_file = argv[optind];
      }
      // print_state();
      optind++;
    }
  }
  // free(joined_argv);
}

format_t
determine_format(char *argument)
{
  if (strcmp(argument, STR_UTF16LE) == 0)
    return UTF16LE;
  if (strcmp(argument, STR_UTF16BE) == 0)
    return UTF16BE;
  if (strcmp(argument, STR_UTF8) == 0)
    return UTF8;
  return 0;
}

const char*
bom_to_string(format_t bom){
  switch(bom){
    case UTF8: return  STR_UTF8;
    case UTF16BE: return  STR_UTF16BE;
    case UTF16LE: return  STR_UTF16LE;
  }
  return "UNKNOWN";
}

char*
join_string_array(int count, char *array[])
{
  char *ret;
  // char charArray[count];
  int i;
  int len = 0, cur_str_len;
  // int str_len;
//delete idont kneo why
  // str_len = array_size(count, array);
  ret = (char*) malloc(count * sizeof(int));

  for (i = 0; i < count; ++i) {
    cur_str_len = strlen(array[i]);
    memecpy(ret + len, array[i], cur_str_len);
    len += cur_str_len;
    memecpy(ret + len, " ", 1);
    len += 1;
  }

  return ret;
}

int
array_size(int count, char *array[])
{
  int i, sum = 1; /* NULL terminator */
  for (i = 0; i < count; ++i) {
    sum += strlen(array[i]);
    ++sum; /* For the spaces */
  }
  return sum+1;
}

void
print_state()
{
// errorcase:
  if (program_state == NULL) {
    error("program_state is %p", (void*)program_state);
    exit(EXIT_FAILURE);
  }
  info("program_state {\n"
         "  format_t encoding_to = 0x%X;\n"
         "  format_t encoding_from = 0x%X;\n"
         "  char *in_file = '%s';\n"
         "  char *out_file = '%s';\n"
         "};\n",
         program_state->encoding_to, program_state->encoding_from,
         program_state->in_file, program_state->out_file);
}

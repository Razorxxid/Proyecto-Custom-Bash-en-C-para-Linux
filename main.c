#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "builtin.h"
#include "parser.h"
#include "execute.h"
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {

    char *pathvar;
    putenv("PS1=$ ");
    pipeline pipe=pipeline_new();
    while(1){
        pathvar = getenv("PS1");
        printf("%s",pathvar);
        pipe = parse_pipeline(parser_new(stdin));
        if(pipe!=NULL) {
            execute_pipeline(pipe);
        }

        
    }
    return 0;
}

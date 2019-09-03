#include <stdio.h>
#include "execute.h"
#include <stdlib.h>
#include <unistd.h>
#include "builtin.h"
#include "command.h"
#include <fcntl.h> 
#include <errno.h>
#include <gmodule.h>
#include <assert.h>
#include <sys/wait.h>
#include "./tests/syscall_mock.h"


void execute_pipeline(pipeline apipe) {
    scommand self;
    char * commandstr;
    int cid;
    int ret_ex;
    assert(apipe!=NULL);

    if(!pipeline_is_empty(apipe)) {
        self = pipeline_front(apipe);
        
        if (pipeline_length(apipe) == 1) {
            if (builtin_index(apipe) == BUILTIN_CHDIR || builtin_index(apipe) == BUILTIN_EXIT) {
                builtin_run(apipe);    
            }else{
                commandstr = bstr2cstr(scommand_front(self),'\0');
                cid = fork();

                if(cid!=0){
                    wait(NULL);

                }else if(cid==-1){
                	fprintf(stderr, "child process failed\n");

                }else{
                    int index=0;
                    char* parsed;
                    char* argv[scommand_length(self)];
                    /*deberiamos cambiar esto por pipe()*/
                    int fdin = open (bstr2cstr (scommand_get_redir_in (self),'\0'), O_RDWR,0700);
                    int fdout = open (bstr2cstr (scommand_get_redir_out (self),'\0'), O_RDWR,0700);
                    /*-----------------------------------*/
                    if (fdin>0) {dup2 (fdin,0);}

                    if (fdout>0) {dup2 (fdout,1);}

                    parsed = strtok(commandstr, " ");;
                    
                    while(parsed!=NULL) {
                        argv[index]=parsed;
                        index++;
                        parsed= strtok(NULL, " ");
                    }
                    argv[index] = NULL;
                    close (fdin);
                    close (fdout);
                    ret_ex=execvp(argv[0], argv);

                    if (ret_ex==-1){
                    	fprintf(stderr, "invalid command\n");
                    }
                }
            }
        } else {
            
        }
    }  
}



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
    int cid;
    int ret_ex;
	int fd[2];
    assert(apipe!=NULL);

    if(!pipeline_is_empty(apipe)) {
        self = pipeline_front(apipe);
        
        if (pipeline_length(apipe) == 1) {

            if (builtin_index(apipe) == BUILTIN_CHDIR || builtin_index(apipe) == BUILTIN_EXIT) {
                builtin_run(apipe);    

            }else{

                int index=0;
                char* parsed;
                char* argv[scommand_length(self)];
                const_bstring comando = scommand_to_string(self);
                char* commandStr = bstr2cstr(comando,'\0');
                pipe(fd);

                    parsed = strtok(commandStr, " ");;
                    // TODO: read g_queue_peek_nth ()

                    while(parsed!=NULL && (strcmp(parsed, ">")!=0 || strcmp(parsed, "<")!=0)){
                        argv[index]=parsed;
                        index++;
                        parsed= strtok(NULL, "");
                    }
                    argv[index] = NULL;

                 

                cid = fork();

                if(cid==-1){

                	fprintf(stderr, "child process failed\n");

                }else if(cid!=0){

                    wait(NULL);


                }else{

                if(scommand_get_redir_in(self)!=NULL) {
                	int fd_in = open(bstr2cstr(scommand_get_redir_in(self),'\0'),O_RDWR,0);
                	if(fd_in<=0){
                		fprintf(stderr, "failed open input\n");
                	}
                	dup2(fd[1], fd_in);
                }
                if(scommand_get_redir_out(self)!=NULL) {
                	int fd_out = open(bstr2cstr(scommand_get_redir_out(self),'\0'),O_RDWR,0);
                	if(fd_out<=0){
                		fprintf(stderr, "failed open output\n");
                	}
                	dup2(fd[0], fd_out);
                }
	                    ret_ex = execvp(argv[0], argv);

	                    if (ret_ex==-1){
	                    	fprintf(stderr, "invalid command\n");
	                    }

	               exit(EXIT_FAILURE);


	            }
	        }
        } else {
            
        }
    }  
}


/*                    deberiamos cambiar esto por pipe()
                    int fdin = open (bstr2cstr (scommand_get_redir_in (self),'\0'), O_RDWR,0700);
                    int fdout = open (bstr2cstr (scommand_get_redir_out (self),'\0'), O_RDWR,0700);
                    -----------------------------------
                    if (fdin>0) {dup2 (fdin,0);}

                    if (fdout>0) {dup2 (fdout,1);}
                    close (fdin);
                    close (fdout);
*/

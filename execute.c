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
    assert(apipe!=NULL);
    unsigned int length=pipeline_length(apipe);
    
    if(length==1 && (builtin_index(apipe) == BUILTIN_CHDIR || builtin_index(apipe) == BUILTIN_EXIT)) {  
        builtin_run(apipe);
    } else if (length >=1 && !(builtin_index(apipe) == BUILTIN_CHDIR || builtin_index(apipe) == BUILTIN_EXIT)) {
        int status;
        int pipefds[2*(length-1)];        
        int pipestatus;
        int pid;
        int ret_ex;
        int WCONTINUED=0;    
        bool background = !pipeline_get_wait(apipe);        
        //array of pipes
        if(length > 1) {
            for(unsigned int i = 0u; i < length-1; i++){
                pipestatus= pipe(pipefds + i*2);
                if (pipestatus<0) {            
                    fprintf(stderr,"pipe creation failed\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        unsigned int j = 0u;
        while(j < length) {
            pid = fork();
            /////Child process
            if(pid==0) {
                scommand current = pipeline_front(apipe);
                char** argv=calloc(scommand_length(current)+1,sizeof(char));
                int fd_in=0;
                int fd_out=1;
                unsigned int sclength=scommand_length(current);
                /////Redirectors
                if(scommand_get_redir_in(current)!=NULL) {
                    fd_in = open(bstr2cstr(scommand_get_redir_in(current),'\0'), O_RDONLY | O_CREAT ,0777);
                    if(fd_in<=0){
                        fprintf(stderr, "failed open input\n");
                    }
                    dup2(fd_in,0);
                    close(fd_in);
                }
                if(scommand_get_redir_out(current)!=NULL) {
                    fd_out = open(bstr2cstr(scommand_get_redir_out(current),'\0'), O_WRONLY | O_CREAT ,0777);
                    if(fd_out<=0){
                        fprintf(stderr, "failed open output\n");
                    }
                    dup2(fd_out,1);
                    close(fd_out);
                }
                ////if not first command&& j!= 2*numPipes
                if(j!=0 && j!= 2*(length-1)){
                    if(dup2(pipefds[(j-1)*2], 0) < 0){
                        fprintf(stderr,"dup2 error pipefds2\n");///j-2 0 j+1 1
                        exit(EXIT_FAILURE);
                    }
                }

                ////If not last command
                if(j < length-1){
                    if(dup2(pipefds[(j*2)+1], 1) < 0){
                        fprintf(stderr,"dup2 error pipefds1 \n");
                        exit(EXIT_FAILURE);
                    }
                }
             
                ////Close stdin of fds
                if(length>1) {
                    for(unsigned int i=0u; i < 2*(length-1); i++){
                        close(pipefds[i]);
                    }
                }
                ////Get command and arguments for execution
                unsigned int index=0u;
                while(index<sclength){
                    argv[index]= bstr2cstr(scommand_front(current),'\0');
                    scommand_pop_front(current);
                    index++;
                }
                argv[index]='\0';

                ret_ex=execvp(argv[0], argv);
                if(ret_ex < 0) {
                    fprintf(stderr, "execvp failure\n"); 
                }
                exit(1);
            } else if(pid < 0){
                fprintf(stderr,"error fork failed");
                exit(EXIT_FAILURE);
            }
            pipeline_pop_front(apipe);
            j++;
        }
        ///Parent closes Fds and waits for its childs
        if(length>1){
            for(unsigned int i=0u; i < 2*(length-1); i++){
                close(pipefds[i]);
            }
        }
        ///Check for background    
        if(!background){
            for(unsigned int i=0u; i < length; i++) {
                waitpid(-1,&status,WNOHANG);  
            }
        }else {
            if (length>1) {
                for(unsigned int i=0u; i < length; i++) {
                    waitpid(-1,&status,WCONTINUED);
                }
            }
        }
    } 
}
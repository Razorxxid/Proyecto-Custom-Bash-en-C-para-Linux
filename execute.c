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
    int cid;
    int ret_ex; 
    unsigned int length;
    scommand current;
    char* commandStr;

    assert(apipe!=NULL);
    length=pipeline_length(apipe);
    current = pipeline_front(apipe);
    

    if(length>0) {
        if (length==1){
            if (builtin_index(apipe) == BUILTIN_CHDIR || builtin_index(apipe) == BUILTIN_EXIT) {
                    builtin_run(apipe);    
            }else{ 
                int background=pipeline_get_wait(apipe);
                cid = fork();


                if(cid==-1){
                    fprintf(stderr, "failed fork\n");
                }
                else if(cid==0){
                    char* argv[scommand_length(current)];
                    char* parsed;
                    int fd_in=0;
                    int fd_out=1;
                    unsigned int index=0;
                    commandStr = bstr2cstr(scommand_basic(current),'\0');
                        if(scommand_get_redir_in(current)!=NULL) {
                            fd_in = open(bstr2cstr(scommand_get_redir_in(current),'\0'), O_RDWR | O_CREAT ,0777);
                            if(fd_in<=0){
                                fprintf(stderr, "failed open input\n");
                            }
                            dup2(fd_in,0);
                        }
                        if(scommand_get_redir_out(current)!=NULL) {
                           fd_out = open(bstr2cstr(scommand_get_redir_out(current),'\0'),O_RDWR|O_CREAT ,0777);
                           if(fd_out<=0){
                           fprintf(stderr, "failed open output\n");
                           }
                           dup2(fd_out,1);
                        }  

                        parsed = strtok(commandStr, " ");
                        while(parsed!=NULL){
                            argv[index]=parsed;
                            index++;
                            parsed= strtok(NULL, "");
                        }
                        argv[index] = NULL;


                        ret_ex =  execvp(argv[0], argv);
                        
                        if(ret_ex<0) {
                            fprintf(stderr, "Comando erróneo\n"); 
                        }
                        exit(1);
                    


                }else{ //parent process
                    /* me fijo si tengo que esperar*/
                    int status;
                    if(!background){

                       waitpid(-1,&status,WNOHANG);

                    }else{
                       waitpid(-1,&status,WCONTINUED);
                    } 
                
                }
            }




        }else if (length==2){
            if(builtin_index(apipe) == BUILTIN_UNKNOWN){    
            	int fd_in=0;
                int fd_out=1;
                int fd1[2];
            	int ret_pp;
            	pid_t c_id;

                int background=pipeline_get_wait(apipe);
                
                scommand first = pipeline_front(apipe);
                char* argv[scommand_length(first)];
                char* parsed;
                unsigned int index=0;
                char* commandStr = bstr2cstr(scommand_basic(first),'\0');

            	ret_pp=pipe(fd1);
            	if(ret_pp!=0){
            		fprintf(stderr, "failed pipe\n" );
            	}

                c_id = fork();

                if(c_id==-1){
                    fprintf(stderr, "failed fork\n");
                }
                else if(c_id == 0) {

                    dup2(fd1[1],fd_out);
                    close(fd1[0]);
                    close(fd1[1]);

                    if(scommand_get_redir_in(first)!=NULL) {
                        fd_in = open(bstr2cstr(scommand_get_redir_in(first),'\0'), O_RDWR | O_CREAT ,0777);
                        if(fd_in<=0){
                            fprintf(stderr, "failed open input\n");
                        }
                        dup2(fd_in,0);
                    }
                    if(scommand_get_redir_out(first)!=NULL) {
                        fd_out = open(bstr2cstr(scommand_get_redir_out(first),'\0'),O_RDWR|O_CREAT ,0777);
                        if(fd_out<=0){
                            fprintf(stderr, "failed open output\n");
                        }
                        dup2(fd_out,1);
                    }  

                    parsed = strtok(commandStr, " ");
                    while(parsed!=NULL){
                        argv[index]=parsed;
                        index++;
                        parsed= strtok(NULL, "");
                    }
                    argv[index] = NULL;


                    ret_ex =  execvp(argv[0], argv);
                            
                    if(ret_ex<0) {
                        fprintf(stderr, "execvp failure\n"); 
                    }
                    
                    exit(1);
                }
                else {
                    pipeline_pop_front(apipe);
                    scommand second = pipeline_front(apipe);
                    
                    char* argv2[scommand_length(second)];
                    char* commandStr2;
                    char* parsed2;
                    unsigned int index2;

                    dup2(fd1[0], fd_in);
                    close(fd1[0]);
                    close(fd1[1]);
                    

                    commandStr2 = bstr2cstr(scommand_basic(second),'\0');
                    index2=0;

                    parsed2 = strtok(commandStr2, " ");
                    while(parsed2!=NULL){
                        argv2[index2]=parsed2;
                        index2++;
                        parsed2= strtok(NULL, "");
                    }
                    argv2[index2] = NULL;


                    ret_ex =  execvp(argv2[0], argv2);
                            
                    if(ret_ex<0) {
                        fprintf(stderr, "execvp failure\n"); 
                    
                   }
                   int status;
                    if(!background){

                       waitpid(-1,&status,WNOHANG);

                    }else{
                       waitpid(-1,&status,WCONTINUED);
                    } 
                }

        }
    }

    }
}



























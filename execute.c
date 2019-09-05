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
                int status=0;
                int background=0;
                background=pipeline_get_wait(apipe);
                cid = fork();


                if(cid==-1){
                    fprintf(stderr, "failed fork\n");
                }
                else if(cid==0){
                    char* argv[scommand_length(current)];
                    char* parsed;
                    unsigned int index=0;
                    commandStr = bstr2cstr(scommand_basic(current),'\0');
                        if(scommand_get_redir_in(current)!=NULL) {
                            int fd_in = open(bstr2cstr(scommand_get_redir_in(current),'\0'), O_RDWR | O_CREAT ,0777);
                            if(fd_in<=0){
                                fprintf(stderr, "failed open input\n");
                            }
                            dup2(fd_in,0);
                        }
                        if(scommand_get_redir_out(current)!=NULL) {
                           int fd_out = open(bstr2cstr(scommand_get_redir_out(current),'\0'),O_RDWR|O_CREAT ,0777);
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


                    /* me fijo si tengo que esperar*/

                    if(!background) {
                        fclose(stdin); // cierro el stdin del hijo
                    

                        ret_ex =  execvp(argv[0], argv);

                        if(ret_ex<0) {
                        fprintf(stderr, "execvp failure\n"); 
                        }
                        exit(1);

                    }else{ // sino tengo que esperar al hijo
                        ret_ex =  execvp(argv[0], argv);
                        if(ret_ex<0) {
                            fprintf(stderr, "execvp failure\n"); 
                        }
                        exit(1);
                    }


                }else{ //parent process
                    if(background){
                        wait( &status);
                    }
                
                }
            }










        }else{
                for (unsigned int i=0u; i<length; i++){
                    scommand current = pipeline_front(apipe);
                    char* argv[scommand_length(current)];
                    char* commandStr;
                    char* parsed;
                    unsigned int index=0;
                    if(scommand_get_redir_in(current)!=NULL) {
                        int fd_in = open(bstr2cstr(scommand_get_redir_in(current),'\0'), O_RDWR | O_CREAT ,0777);
                        if(fd_in<=0){
                            fprintf(stderr, "failed open input\n");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_in,0);
                    }
                    if(scommand_get_redir_out(current)!=NULL) {
                       int fd_out = open(bstr2cstr(scommand_get_redir_out(current),'\0'),O_RDWR|O_CREAT ,0777);
                       if(fd_out<=0){
                       fprintf(stderr, "failed open output\n");
                       }
                       dup2(fd_out,1);
                    }  
                    
                    commandStr = bstr2cstr(scommand_basic(current),'\0');
                    parsed = strtok(commandStr, " ");
                    while(parsed!=NULL){
                        argv[index]=parsed;
                        index++;
                        parsed= strtok(NULL, "");
                    }
                    argv[index] = NULL; 
                    
                    cid = fork();
                    if(cid!=0){
                        wait(NULL);
                    }

                    if(cid==0){
                        ret_ex =  execvp(argv[0], argv);
                        if(ret_ex<0) {
                           fprintf(stderr, "execvp failure\n"); 
                        }
                        exit(EXIT_SUCCESS);
                    }
                    
               }
            
        }

    }


}








































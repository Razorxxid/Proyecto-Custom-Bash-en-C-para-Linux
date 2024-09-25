#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtin.h"
#include "command.h"
#include "./tests/syscall_mock.h"

cmd_id builtin_index (const pipeline pipe){
	cmd_id builtin;
	bstring pipeAux = pipeline_to_string(pipe);
	char* pipeStr = bstr2cstr(pipeAux,'\0');

	if(strstr(pipeStr,"cd") != NULL){
 		builtin = BUILTIN_CHDIR;
	} else if(strstr(pipeStr,"exit") != NULL) {
		builtin = BUILTIN_EXIT;
	} else {
		builtin = BUILTIN_UNKNOWN;
	}

	return builtin;
}

bool builtin_is_exit (const pipeline pipe) {

	return(builtin_index(pipe) == BUILTIN_EXIT);
}

void builtin_run (const pipeline pipe){
	const char *path=NULL;
	cmd_id com_id = builtin_index(pipe);
	int ret;
	scommand sc;
		if (builtin_is_exit(pipe)){
			printf("terminated\n");
			exit(0);
		}
				sc = pipeline_front(pipe); 
				if(com_id == BUILTIN_CHDIR){
					if (scommand_length(sc)==2){
					scommand_pop_front(sc);
					path= bstr2cstr(scommand_front(sc), '\0');
					
					ret=chdir(path);
						if(ret!=0){
							fprintf(stderr, "path %s not found \n", path);
						}
					}else if(scommand_length(sc)<2){
						chdir(getenv("HOME"));
						
					}else{
						fprintf(stderr, "many arguments\n");
					}
				}else{
					printf("incorrect command\n");
				}
}


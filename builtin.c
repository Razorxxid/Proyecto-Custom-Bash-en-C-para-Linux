#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtin.h"
#include "command.h"
#include "./tests/syscall_mock.h"


cmd_id builtin_index (const pipeline pipe){
	cmd_id builtin;

	bstring cd = bfromcstr("cd");
	bstring exit = bfromcstr("exit");
	for(unsigned int i=0; i<pipeline_length(pipe); ++i){
		scommand c = pipeline_front(pipe);

		if(!bstrcmp(scommand_front(c), cd)) {
			builtin = BUILTIN_CHDIR;
		}else if (!bstrcmp(scommand_front(c), exit)) {
			builtin = BUILTIN_EXIT;
		}else {
			builtin = BUILTIN_UNKNOWN;
		}

	}

	return builtin;
}

bool builtin_is_exit (const pipeline pipe) {
	bstring exit = bfromcstr("exit");
	bool isExit=false;

	for(unsigned int i=0; i<pipeline_length(pipe); ++i){
		scommand c = pipeline_front(pipe);

		if(!bstrcmp(scommand_front(c), exit)) {
			isExit = true;
		}
	}

	return isExit;
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
						}else{
							printf("$ %s\n",path );
						}
					}else if(scommand_length(sc)<2){
						fprintf(stderr, "few arguments\n");
					}else{
						fprintf(stderr, "many arguments\n");
					}
				}else{
					printf("incorrect command\n");
				}
				

		
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtin.h"
#include "command.h"


cmd_id builtin_index (const pipeline pipe){
	cmd_id builtin;

	bstring cd = bfromcstr("cd");
	bstring exit = bfromcstr("exit");
	for(unsigned int i=0; i<pipeline_length(pipe)-1; ++i){
		scommand c = pipeline_front(pipe);

		if(bstrcmp(scommand_front(c), cd)) {
			builtin = BUILTIN_CHDIR;
		}
		else if (bstrcmp(scommand_front(c), exit)) {
			builtin = BUILTIN_EXIT;
		}
		else {
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
	bstring cd=bfromcstr("cd");
	int ret;
		if (builtin_is_exit(pipe)){
			exit(0);
		}
			for(unsigned int i=0; i<pipeline_length(pipe); ++i){
				scommand com = pipeline_front(pipe); 
				if(!bstrcmp(scommand_front(com),cd)){
					scommand_pop_front(com);
					path= bstr2cstr(scommand_front(com), '\0');
					
				}
				
			}
			ret=chdir(path);
			if(ret!=0){
				fprintf(stderr, "path %s not found \n", path);
			}

		
}
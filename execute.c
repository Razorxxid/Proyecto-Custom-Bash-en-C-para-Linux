#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h> 
#include <unistd.h> 
#include "execute.h"
#include "builtin.h"
#include "command.h"



void execute_pipeline(pipeline apipe) {

	int pp;	//para ver si pipe funciona bien
	int fd[2]; //file descriptors//
 	pid_t cid;
 	cmd_id cmid=builtin_index(apipe);
 	scommand sc;
 	char *sc_str;
	assert(apipe!=NULL);
 	cmid= builtin_index(apipe);

 	if(!pipeline_is_empty(apipe)){

	 	if (pipeline_length(apipe)==1){
	 		if(cmid==BUILTIN_CHDIR|| cmid== BUILTIN_EXIT){
	 			builtin_run(apipe);
	 		}else{
			 	sc=pipeline_front(apipe);
			 	sc_str=bstr2cstr(scommand_to_string(sc), ' ');
	 			execv("/bin/ls", &sc_str ); //bin contiene todos los programas del bash
	 		}
	 	}else{

		 	pp = pipe(fd); 

		 	if (pp!=0){
				exit(EXIT_FAILURE); 
		 	}
		 	cid=fork();

		 		if (cid==0){
		 			close(fd[0]); //cierra file descriptor 0(stdin) 

		 		}else{
		 			wait(NULL);

		 		}
	 	}

	}
}













/*	scommand sc;
	bstring cd;
	int pid;
	int ls;
	assert(apipe!=NULL);

	assert(apipe!=NULL);
	cd=bfromcstr("cd");
	if(!pipeline_is_empty(apipe)){
		if (pipeline_length(apipe)==1){
			builtin_run(apipe);
		}
	
		for (unsigned int i=0; i<pipeline_length(apipe);i++){
			sc=pipeline_front(apipe);
			if(!bstricmp(scommand_front(sc),cd)){
				pid=fork();
				if (pid==0){
					builtin_run(apipe);
				}else{
					pipeline_pop_front(apipe);
					ls=system("cd /home");
					exit(ls);
				}
			}
		}
	}
}
*/
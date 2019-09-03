#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "builtin.h"
#include "parser.h"
#include "execute.h"

int main(int argc, char const *argv[])
{
	
	pipeline pipe=pipeline_new();
	while(1){
	printf("$ ");
	
	pipe = parse_pipeline(parser_new(stdin));
	execute_pipeline(pipe);	
}
	return 0;
}

#include "builtin.h"
#include "parser.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define MAX_SIZE 1000
int main(int argc, char const *argv[])
{
	
	pipeline pipe=pipeline_new();
	printf("$ ");
	
	pipe = parse_pipeline(parser_new(stdin));
	builtin_run(pipe);
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char ** argv)
{
    while(1)
    {
        char * line = readline("> ");
        if(!line) break;
	printf("%s", line);
        if(*line) add_history(line);
        /* Do something with the line here */
        free(line);
    }

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"


int 
main (int argc, char ** argv)
{
    init() ;
	
	fp_niv = fopen("NIV.txt", "r") ;

	char * s = 0x0 ;

	while ((s = read_a_line())) {
		#ifdef DEBUG
			char * new_line = (char *) malloc(sizeof(char) * 12) ;
			char * tmp = strchr(s, ':') ;
			strncpy(new_line, s, strlen(s) - strlen(tmp) + 1) ;
			char * ws = strchr(tmp, ' ') ;
			if (ws == 0x0) strcat(new_line, tmp + 1) ;
			else strncat(new_line, tmp + 1, strlen(tmp) - strlen(ws)) ;
			printf("%s\t", new_line) ;
		#endif

        if (term_exists(s)) {
			#ifdef DEBUG
				printf("\n    >>>>> 	") ;
			#endif
            printf("%s\n", s) ;
        }
		free((char *)s) ;

		#ifdef DEBUG
			printf("\n") ;
			free((char *)new_line) ;
		#endif
	}
	printf("\n") ;
	fclose(fp_niv) ;
}
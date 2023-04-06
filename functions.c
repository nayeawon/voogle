#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

char * tstmnt ;

void
_help ()
{
	printf("NAME: voogle - a simple bible search engine\n") ;
	printf("SYNOPSIS: ./voogle <SEARCH TERMS>\n") ;
	printf("DESCRIPTION: voogle is a simple bible search engine that searches the NIV Bible.\n") ;
	printf("\tIt can search for a single word, a phrase, or a combination of both.\n") ;
	printf("\tIt can also search for a specific book and/or chapter.\n") ;
	printf("\tThe <SEARCH TERMS> are as follows:\n") ;
	printf("\t\ttoken\t\t\tSearch for all verses containing the given token (an alphanumeric string w/o whitespace) in case-insensitive way\n\n") ;
	printf("\t\ttoken*\t\t\tSearch for all verses containing the given prefix as a token in case-insensitive way\n\n") ;
	printf("\t\t-token\t\t\tSearch for all verses that do not contain the given token in case-insensitive way\n\n") ;
	printf("\t\t\"string\"\t\tSearch for all verses containing the given string\n\n") ;
	printf("\t\t{book|책}:{EngCode|EngName|KorCode|KorName}\t\tSearch for all verses in the given book\n\n") ;
	printf("\t\t{chapter|장}:{chapternumber}\tSearch for all verses in the given chapter\n\n") ;
	printf("\t\t{verse|절}:{versenumber}\tSearch for the given verse\n\n") ;
	printf("\t\t@{old|구약}\t\tSearch for old testament\n\n") ;
	printf("\t\t@{new|신약}\t\tSearch for new testament\n\n") ;
	printf("EXAMPLES:\n") ;
	printf("\t./voogle abraham* isaac -esau \" and \"\n\n") ;
	printf("\t./voogle book:창 장:35 절:1 @old\n\n") ;
	printf("\t./voogle @신약 책:ezra verse:3\n\n") ;

}

void
_error (char * msg, int exit_code)
{
	fprintf(stderr, "%s\n\n", msg) ;
	_help() ;
	exit(exit_code) ;
}

void
makeargv (char * input)
{
	if (input != 0x0)
	{
		argc = 0 ;
		char * str = input ;
		char * arg = 0x0 ;
		regex_t regex_specific, regex_general ;
		do
		{
			if (argc > 8)
			{
				_error("Too many search terms", 3) ;
			}
			char * q_ptr = strchr(str, '"') ;
			char * s_ptr = strchr(str, ' ') ;
			if (q_ptr != 0x0 && (q_ptr < s_ptr || s_ptr == 0x0))
			{
				if (strstr((char *)q_ptr + sizeof(char), "\"") == 0x0)
				{
					char * err = "Invalid search term: " ;
					strcat(err, str) ;
					_error(err, 3) ;
				}
				arg = strtok((char *)q_ptr + sizeof(char), "\"") ;
				search_terms[argc].term = (char *) malloc(sizeof(char) * (strlen(arg) + 1)) ;
				strcpy(search_terms[argc].term, "\"") ;
				strcat(search_terms[argc].term, arg) ;
				strcat(search_terms[argc].term, "\"") ;
				search_terms[argc].type = TYPE_STRING ;
				s_ptr = strtok(0x0, "\0") ;
			}
			else
			{
				arg = strtok(str, " ") ;
				if (s_ptr != 0x0)
					s_ptr = s_ptr + sizeof(char) ;

				int match_regex_s = regcomp(&regex_specific,"^book:[A-Za-z0-9가-힣]+|^(chapter|verse):[0-9]+$|^@(old|new|구약|신약$)|^책:[A-Za-z가-힣0-9]+$|^장:[0-9]+$|^절:[0-9]+$", REG_EXTENDED) ;
				int match_regex_g = regcomp(&regex_general,"^(-|@)?[A-Za-z0-9가-힣]+\\*?", REG_EXTENDED) ;
				
				if (match_regex_s && match_regex_g)
				{
					regfree(&regex_specific) ; regfree(&regex_general) ;
					_error("Could not compile regex", 2) ;
				}

				match_regex_s = regexec(&regex_specific, arg, 0, 0x0, 0) ; match_regex_g = regexec(&regex_general, arg, 0, 0x0, 0) ;
				if ((strstr(arg, ":") || strstr(arg, "@")) && match_regex_s != 0)
				{
					regfree(&regex_specific) ; regfree(&regex_general) ;
					char err[128] ;
					strcpy(err, "Invalid search term: ") ; strcat(err, arg) ;
					_error(err, 3) ;
				}
				else if (match_regex_g != 0 || (strstr(arg, "-") && strstr(arg, "*")))
				{
					regfree(&regex_specific) ; regfree(&regex_general) ;
					char err[128] ;
					strcpy(err, "Invalid search term: ") ; strcat(err, arg) ;
					_error(err, 4) ;
				}
				
				search_terms[argc].term = (char *) malloc(sizeof(char) * (strlen(arg) + 1)) ;
				strcpy(search_terms[argc].term, arg) ;

				if (strstr(arg, "*") != 0x0) search_terms[argc].type = TYPE_WILDCARD ;
				else if (strstr(arg, "-") != 0x0) search_terms[argc].type = TYPE_EXCEPT ;
				else if (strstr(arg, "book:") != 0x0 || strstr(arg, "책:") != 0x0) search_terms[argc].type = TYPE_BOOK ;
				else if (strstr(arg, "chapter:") != 0x0 || strstr(arg, "장:") != 0x0) search_terms[argc].type = TYPE_CHAPTER ;
				else if (strstr(arg, "verse:") != 0x0 || strstr(arg, "절:") != 0x0) search_terms[argc].type = TYPE_VERSE ;
				else if (strstr(arg, "@old") != 0x0 || strstr(arg, "@구약") != 0x0) search_terms[argc].type = TYPE_OLD ;
				else if (strstr(arg, "@new") != 0x0 || strstr(arg, "@신약") != 0x0) search_terms[argc].type = TYPE_NEW ;
				else search_terms[argc].type = TYPE_NORMAL;
				s_ptr = strtok(0x0, "\0") ;
			}
			#ifdef DEBUG
				printf("%d {%s}\n", search_terms[argc].type, search_terms[argc].term) ;
			#endif
			str = s_ptr;
			argc++;
		} while (str != 0x0) ;
	}
}


void
init() 
{
    printf(" _   _                   _       \n") ;
    printf("| | | |                 | |      \n") ;
    printf("| | | | ___   ___   __ _| | ___ \n") ;
    printf("| | | |/ _ \\ / _ \\ / _` | |/ _ \\ \n") ;
    printf("\\ \\_/ / (_) | (_) | (_| | |  __/ \n") ;
    printf(" \\___/ \\___/ \\___/ \\__, |_|\\___|  \n") ;
    printf("                    __/ |        \n") ;
    printf("                   |___/         \n\n") ;
    printf("[ 🔍 ] ") ;
    
	tstmnt = (char *) malloc(sizeof(char) * 4) ;
	strcpy(tstmnt, "Gen") ;
    char buf[BUFSIZ] = {0};
    fgets(buf, sizeof(buf), stdin) ;
	buf[strlen(buf) - 1] = 0x0;
	if (strlen(buf) == 0)
	{
		_error("No search terms", 3) ;
	}
    makeargv(buf) ;
	printf("\n") ;
}

char * 
read_a_line ()
{
	static char buf[BUFSIZ] ;
	static int buf_n = 0 ;
	static int curr = 0 ;

	if (feof(fp_niv))
		return 0x0 ;

	char * s = 0x0 ;
	size_t s_len = 0 ;
	do {
		int end = curr ;
		while (!(end >= buf_n || !iscntrl(buf[end]))) {
			end++ ;
		}
		curr = end ;
		while (!(end >= buf_n || iscntrl(buf[end]))) {
			end++ ;
		}
		if (s == 0x0) {
			s = strndup(buf + curr, end - curr) ;
			s_len = end - curr ;
		}
		else {
			s = realloc(s, s_len + end - curr + 1) ;
			s = strncat(s, buf + curr, end - curr) ;
			s_len = s_len + end - curr ;
		}
		if (end < buf_n) {
			curr = end + 1 ;
			break ;
		}

		buf_n = fread(buf, 1, sizeof(buf), fp_niv) ;
		curr = 0 ;
	} while (buf_n > 0) ;
	return s ;
}

char *
case_insensitive (char * line)
{
	for (unsigned long i = 0 ; i < strlen(line) ; i++)
	{
		line[i] = tolower(line[i]) ;
	}
	return line ;
}

char *
remove_quotmark (char * line)
{
	char * new_line = (char *) malloc(sizeof(char) * (strlen(line) + 1)) ;
	strncpy(new_line, line + 1, strlen(line) - 2) ;
	new_line[strlen(line) - 2] = 0x0 ;
	return new_line ;
}

char *
extract_verse (char * line)
{
	char * new_line = (char *) malloc(sizeof(char) * (strlen(line) + 1)) ;
	char * tmp = strchr(line, ':') ;
	tmp = strchr(tmp + 1, ' ') ;
	if (tmp == 0x0) strcpy(new_line, "") ;
	else strcpy(new_line, tmp + 1) ;
	return new_line ;
}

char *
get_code_from_term (char * term)
{
	FILE * info_file = fopen("info.csv", "r") ;

	if (info_file == 0x0)
		exit(4);
	
	char buf[BUFSIZ] ;
	int exist = 0 ;
	char * eng_code ;

	while (fgets(buf, BUFSIZ, info_file))
	{
		char * brief_check = strstr(buf, term) ;
		if (brief_check != 0x0) 
		{
			char * split_check = (char *) malloc(sizeof(char) * (strlen(term) + 1)) ;
			strncpy(split_check, brief_check, strlen(term)) ;
			if (strcmp(split_check, term) == 0) 
			{
				eng_code = strtok(buf, ",") ;
				#ifdef DEBUG
					printf("code:{%s,%s}\t", split_check, eng_code) ;
				#endif

				exist = 1 ;
				break ;
			}
		}
	}

	fclose(info_file) ;
	if (exist) return eng_code ;
	_error("Cannot find the book in info.csv", 3) ;
}

int
is_term_kor (char * search_term)
{
	regex_t regex_eng ;
	int match_regex_e = regcomp(&regex_eng, "^[A-Za-z]+$", REG_EXTENDED) ;
	if (match_regex_e)
	{
		regfree(&regex_eng) ;
		_error("Could not compile regex", 2) ;
	}
	match_regex_e = regexec(&regex_eng, search_term, 0, 0x0, 0) ;
	regfree(&regex_eng) ;
	if (match_regex_e != 0) return 1 ;
	else return 0 ;
}

int
normal_check (char * line, char * search_term)
{
	char copy_term[strlen(search_term) + 1] ;
	strcpy(copy_term, search_term) ;

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, extract_verse(line)) ;

	char * brief_check = strtok(case_insensitive(copy_line), " ") ;
	while (brief_check != 0x0) 
	{
		if (strcmp(brief_check, case_insensitive(copy_term)) == 0) 
		{
			#ifdef DEBUG
				printf("normal:{%s}\t", brief_check) ;
			#endif

			return 1 ;
		}
		brief_check = strtok(0x0, " ") ;
	}
	return 0 ;
}

int
wildcard_check (char * line, char * search_term)
{
	char copy_term[strlen(search_term) + 1] ;
	strncpy(copy_term, search_term, strlen(search_term) - 1) ;
	copy_term[strlen(search_term) - 1] = 0x0 ;

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, extract_verse(line)) ;

	char * check = strstr(case_insensitive(copy_line), case_insensitive(copy_term)) ;
	if (check != 0x0) 
	{
		#ifdef DEBUG
			printf("wildcard:{%s}\t", copy_term) ;
		#endif
		return 1 ;
	}
	return 0 ;
}

int
except_check (char * line, char * search_term)
{
	char copy_term[strlen(search_term) + 1] ;
	strcpy(copy_term, search_term + 1) ;

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, extract_verse(line)) ;
	
	char * check = strstr(case_insensitive(copy_line), case_insensitive(copy_term)) ;
	if (check != 0x0) return 0 ;

	#ifdef DEBUG
		printf("except: {%s}\t", copy_term) ;
	#endif
	return 1 ;
}

int
string_check(char * line, char * search_term)
{
	search_term = remove_quotmark(search_term) ;
	char * brief_check = strstr(extract_verse(line), search_term) ;
	if (brief_check != 0x0) 
	{
		char * split_check = (char *) malloc(sizeof(char) * (strlen(search_term) + 1)) ;
		strncpy(split_check, brief_check, strlen(search_term)) ;
		if (strcmp(split_check, search_term) == 0) 
		{
			#ifdef DEBUG
				printf("string:{%s}\t", split_check) ;
			#endif

			return 1 ;
		}
	}
	return 0 ;
}

int
book_check (char * line, char * search_term)
{
	char copy_term[strlen(search_term)] ;
	strcpy(copy_term, strchr(search_term, ':') + 1) ;
	copy_term[strlen(copy_term)] = 0x0 ;
	
	if (is_term_kor(copy_term) || strlen(copy_term) > 3)
	{
		char * code = get_code_from_term(case_insensitive(copy_term)) ;
		search_term = code ;
		strcpy(copy_term, search_term);
		copy_term[strlen(copy_term)] = 0x0 ;
	}

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, line) ;

	char * book_check = strtok(copy_line, " ") ;
	if (book_check != 0x0) 
	{
		if (strcmp(book_check, copy_term) == 0) 
		{
			#ifdef DEBUG
				printf("book:{%s}\t", book_check) ;
			#endif

			return 1 ;
		}
	}
	return 0 ;
}

int
chapter_check (char * line, char * search_term)
{
	int ch_num = atoi(strchr(search_term, ':') + 1) ;

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, line) ;

	int chapter_check = atoi(strtok(strchr(copy_line, ' ') + 1, ":")) ;

	if (chapter_check == ch_num) 
	{
		#ifdef DEBUG
			printf("chapter:{%d}\t", chapter_check) ;
		#endif

		return 1 ;
	}

	return 0 ;
}

int
verse_check (char * line, char * search_term)
{

	int vs_num = atoi(strchr(search_term, ':') + 1) ;

	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, line) ;

	int verse_check = atoi(strtok(strchr(copy_line, ':') + 1, " ")) ;

	if (verse_check == vs_num) 
	{
		#ifdef DEBUG
			printf("verse:{%d}\t", verse_check) ;
		#endif

		return 1 ;
	}

	return 0 ;
}

int
old_check (char * line)
{
	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, line) ;

	char * book_check = strtok(copy_line, " ") ;
	
	if (strcmp(book_check, tstmnt) != 0)
	{
		#ifdef DEBUG
			printf("old:{%s}\t", book_check) ;
		#endif
		
		if (strcmp(tstmnt, "al") == 0) return 0 ;

		free(tstmnt) ;
		tstmnt = (char *) malloc(sizeof(char) * (strlen(book_check) + 1)) ;
		strcpy(tstmnt, book_check) ;
	}
	return 1 ;
}

int
new_check (char * line)
{
	char copy_line[strlen(line) + 1] ;
	strcpy(copy_line, line) ;

	char * book_check = strtok(copy_line, " ") ;
	
	if (strcmp(book_check, tstmnt) != 0)
	{
		#ifdef DEBUG
			printf("new:{%s}\t", book_check) ;
		#endif

		if (strcmp(tstmnt, "al") == 0) return 1 ;

		strcpy(tstmnt, book_check) ;
	}
	return 0 ;
}

int
term_exists (char * line)
{   
	int satisfied = 1; 
	for (int i = 0; i < argc; i++)
	{
		if (!satisfied) break;
		switch (search_terms[i].type)
		{
			case TYPE_NORMAL :
				satisfied *= normal_check(line, search_terms[i].term) ; break ;

			case TYPE_WILDCARD :
				satisfied *= wildcard_check(line, search_terms[i].term) ; break;

			case TYPE_EXCEPT:
				satisfied *= except_check(line, search_terms[i].term) ; break ;

			case TYPE_STRING:
				satisfied *= string_check(line, search_terms[i].term) ; break ;

			case TYPE_BOOK:
				satisfied *= book_check(line, search_terms[i].term) ; break ;

			case TYPE_CHAPTER:
				satisfied *= chapter_check(line, search_terms[i].term) ; break ;

			case TYPE_VERSE :
				satisfied *= verse_check(line, search_terms[i].term) ; break ;

			case TYPE_OLD :
				satisfied *= old_check(line) ; break ;

			case TYPE_NEW :
				satisfied *= new_check(line) ; break ;
		}
	}
    return satisfied;
}
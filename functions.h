#include <stdio.h>

FILE * fp_niv ;

enum types { 
    TYPE_NORMAL,
    TYPE_WILDCARD,
    TYPE_EXCEPT,
    TYPE_STRING,
    TYPE_BOOK,
    TYPE_CHAPTER,
    TYPE_VERSE,
    TYPE_OLD,
    TYPE_NEW
} type ;

struct term {
    char * term ;
    enum types type ;
} search_terms[8] ;

int argc ;

void
_help () ;

void
_error (char * msg, int exit_code) ;

void
makeargv (char * input) ;

void
init () ;

char * 
read_a_line () ;

char *
case_insensitive (char * line) ;

char *
remove_quotmark (char * line) ;

char *
extract_verse (char * line) ;

char *
get_code_from_term (char * term) ;

int
is_term_kor (char * search_term) ;

int
normal_check (char * line, char * search_term) ;

int
wildcard_check (char * line, char * search_term) ;

int
except_check (char * line, char * search_term) ;

int
string_check(char * line, char * search_term) ;

int
book_check (char * line, char * search_term) ;

int
chapter_check (char * line, char * search_term) ;

int
verse_check (char * line, char * search_term) ;

int
old_check (char * line) ;

int
new_check (char * line) ;

int
term_exists (char * line) ;
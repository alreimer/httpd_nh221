/* copy.c:
 *
 * Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>,
 *		       The Silver Hammer Group, Ltd.
 *		 2009-2020  Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

//add by snop to include definition of data type
#include "include/httpd.h"
#include "include/httpd_config.h"
//#include "include/httpd_sysconf.h"
#include "copy.h"
#include "parser.h"
#include "parse_CGI.h"	//char *w_strtok(char **str, char mark); void DoCGI(FILE *out, char *filename)
#include "copy_CGI.h"
#include "copy_tbl.h"	//parse_tbl() & free_tbl()

static char copybuf[16384];
#define BUFSIZE 128
#define DEBUG

#undef DEBUG

extern int TIMEOUT;
extern struct cfg_parse1 *cfg1;
struct cfg_parse1 **cfg_p = &cfg1;


/*is old! use parsestr instead!*/
/*char *strstrend( char *a, char *b)	//check b string in a string and gives part of a-str where b is ending, case ignored
{
    //b must not be NULL!
    char *c;
    char *d;

    while (*a)
    {
	c = (char *) b;
	d = (char *) a;

	while (*c)
	{
	    if (tolower(*c) != tolower(*d))  break;
	    if	(!(*d))	  return NULL;

	    c++;
	    d++;
	}

	if (!(*c))
	    return (char *) d;
	a++;
    }

    return NULL;
}*/

void print_pstr(FILE *out, char *tmp){
	char ch, *tmp1, *tmp2;
	unsigned int i;

	while(*tmp){
	    ch = *tmp;
	    if(*tmp == '/'){
		tmp++;
		if(*tmp == '?'){
			tmp++;
			char *m;
			tmp1 = tmp;
			tmp2 = NULL;
			while(*tmp1){	//   /?var/?
			    if(*tmp1 == '/' && *(tmp1+1) == '?'){
				i = tmp1 - tmp + 1;
				if((i > 1) && (i < 33)){
				m = malloc(i);
				if(m){
				    strmycpy(m, tmp, i);
				    tmp2 = get_var(NULL, m);		//get_var and get_variable
				    free(m);
				}
				}
				tmp=tmp1+2;
				break;
			    }
			    tmp1++;
			}
			if(tmp2 && *tmp2){
				    unsigned long long size = strncpy_(NULL, tmp2, 0)+1;	//this is max. size of tmp2-string
				    if(size > 1 && (m = malloc(size))){
				    strncpy_(m, tmp2, size);
				    fprintf(out, "%s", m);
				    free(m);
				    }
			}
			continue;
		} else if(*tmp != '/') tmp--;
	    } else
	    if(*tmp == '\\'){
		tmp++;
		switch(*tmp){
		    case 't':	ch = '\t'; break;
		    case 'n':	ch = '\n'; break;
		    case '\"':
		    case '\\': ch = *tmp; break;
		    case '0': return;
		}
	    }
	    putc(ch, out);
	    tmp++;
	}
}

unsigned char *point[2];		//0 - place where is ch_zero by /]; 1 - end of string
unsigned char ch_zero = '\0';
unsigned long number = 0;
unsigned long value_ = 0;
unsigned long stack_ = 0;

unsigned char *parsestr1(unsigned char *d, unsigned char *c)	//try identic strings!, "xxx*NULL" combination
{
	unsigned char *tmp, *tmp2, *tmp3, ch;
	unsigned int i;
	unsigned long digi;

	while (*c)
	{
	    
	    if(*c == '/' ){
		c++;
		switch (*c){
    		    case '*':/* printf("d1:%s\n", d);*/
			      c++; while(1){tmp = parsestr1(d, c); if (tmp) return tmp; if(! *d) break; d++;} return NULL;//depend on continues parsing
							//if * in b string -> some symbols between
//	!a*(b+c) = !(a+!(b+c))->	/!/Ba/\/!/Bb/\c/\/E/E
		    case 'B': c++; tmp = NULL; tmp2 = c; i = 0;
					while(*tmp2){
					    if(*tmp2 == '/' && *(tmp2+1) == '/') tmp2++; //tmp2+=2
					    else if(*tmp2 == '/' && *(tmp2+1) == 'B' && i <= 1024) i++; //if(/B)
					    else if(*tmp2 == '/' && *(tmp2+1) == 'E'){	//if(/E)	-optional
						if(i == 0){tmp = tmp2; break;}
						else i--;
					    }
					    tmp2++;
					}
				//tmp - end of compare-string
				//tmp2 - can be used,
				while(1){	//not matched
				    tmp2 = parsestr1(d, c);
				    if(tmp2) break;
				    i = 0;
				    while(1){
					if(*c == '/' && *(c+1) == '/') c++; //c+=2
					else if(*c == '/' && *(c+1) == 'B' && i <= 1024) {i++; c++;}
					else if(i != 0 && *c == '/' && *(c+1) == 'E'){ i--; c++;}
					else if(i == 0 && *c == '/' && *(c+1) == '\\'){c = c + 2; break;} // if(/\) 
					else if((tmp && (c == tmp)) || (*c == '\0')) return NULL;	//end of compare-strings - no matches - return NULL
					c++;
				    }
				}
//printf("str:%s d=%s c=%s\n", tmp2,d,c);
				if(tmp && *tmp2) {
//					return parsestr1(tmp2, tmp+2);}
					d = tmp2; /*tmp2 is not always the end of string!! check it out*/
					c = tmp + 2; continue;}
				else return tmp2;
		    case '{': c++; ch = *c; c++; tmp = NULL; tmp2 = c; i = 0;
					while(*tmp2){
					    if(*tmp2 == '/' && *(tmp2+1) == '/') tmp2++; //tmp2+=2
					    else if(*tmp2 == '/' && *(tmp2+1) == '{' && i <= 1024) i++; //if(/{)
					    else if(*tmp2 == '/' && *(tmp2+1) == '}'){
						if(i == 0){tmp = tmp2 + 2; break;}
						else i--;
					    }
					    tmp2++;
					}
				//tmp - end of compare-string
				//tmp2 - can be used,
				
				//  /t/{*/*="/*/N\\N"/t/}-->
				//  /t/{-/*="/*/N\\N"/t/}-->
				//	 |		 |
				//	 c		tmp
				if(ch == '*'){//  /{*..../}  - 0 Times or more
					while(1){
					    if(tmp){
						tmp2 = parsestr1(d, tmp);
						if(tmp2) return tmp2;
					    }
					    tmp2 = parsestr1(d, c);
					    if(tmp2 == NULL) return NULL;
					    if(*tmp2 == '\0') return tmp2;
					    d = tmp2;
					}
				}
				if(ch == '-'){//  /{-..../}  - 1 Time or more
					while(1){
					    tmp2 = parsestr1(d, c);
					    if(tmp2 == NULL) return NULL;
					    if(*tmp2 == '\0') return tmp2;
					    d = tmp2;
					    if(tmp){
						tmp2 = parsestr1(d, tmp);
						if(tmp2) return tmp2;
					    }
					}
				}
				if(ch == 'R' || ch == 'r'){//  /{R..../}  - Repead 1 Time or more
					i = 0;			// /{r..../}  - Repead 0 Time or more
					while(1){
					    tmp2 = parsestr1(d, c);
					    if(tmp2 == NULL) break;
					    if(tmp2 == d) break;	//no progress
					    d = tmp2; i = 1;
					}
					if(ch == 'R' && i == 0) return NULL;
					c = tmp; continue;
				}
				if(ch == 'C'){//  /{C..../}  - Compare witch pointer is bigger
					tmp2 = parsestr1(d, c);
					if(tmp2 == NULL) return NULL;
					tmp3 = parsestr1(d, tmp);
					if(tmp3 == NULL) return NULL;
					if(tmp3 > tmp2) return tmp3;
					return NULL;
				}
				return NULL;	//default, if not '*' and not '-'
		    case '(': c++; ch = *c; c++; tmp = c; i = 0;
					while(1){
					    if(*tmp == '\0') return NULL;
					    else if(*tmp == '/' && *(tmp+1) == '/') tmp++; //tmp+=2
					    else if(*tmp == '/' && *(tmp+1) == '(' && i <= 1024) i++; //if( /( )
					    else if(*tmp == '/' && *(tmp+1) == ')'){	//if( /) )
						if(i == 0){tmp = tmp + 2; break;}//think about!! this is /) before /: found
						else i--;
					    }
					    else if(i == 0 && *tmp == '/' && *(tmp+1) == ':'){
						tmp = tmp + 2;
						break;	//if( /) )
					    }
					    tmp++;
					}
				// in c - plus, and in tmp - minus
					if(ch == 'S'){		///(S.../:.../) - use Stack with !manual! inc- decrimintation
					i = 0; tmp2 = d; tmp3 = d;
					while(1){
						tmp2 = parsestr1(d, c);
						if(tmp2){
						    if(tmp2 != d) d = tmp2;	//progress!!
						    else tmp2 = NULL;
						}
						if(i && stack_ == 0) break;
						if(tmp2 == NULL && tmp3 == NULL) return NULL;

						tmp3 = parsestr1(d, tmp);
						if(tmp3){
						    if(tmp3 != d) d = tmp3;	//progress!!
						    else tmp3 = NULL;
						}
						if(i && stack_ == 0) break;
						if(tmp2 == NULL && tmp3 == NULL) return NULL;

						i = 1;
					}
					} else return NULL;
				// in d - end of parsed string
					i = 0;
					while(1){
					    if(*tmp == '\0') return NULL;
					    else if(*tmp == '/' && *(tmp+1) == '/') tmp++; //tmp+=2
					    else if(*tmp == '/' && *(tmp+1) == '(' && i <= 1024) i++; //if( /( )
					    else if(*tmp == '/' && *(tmp+1) == ')'){	//if( /) )
						if(i == 0){c = tmp + 2; break;}
						else i--;
					    }
					    tmp++;
					}
				continue;
		    case '\\':
		    case 'E':
		    case ':':
		    case ')':
		    case '}':	point[1] = d;
				return d;
	    /*  set number to /n10n   or stack to /n15s*/
		    case 'n': c++; tmp = c;
				while(*tmp && *tmp != 'n' && *tmp != 's') tmp++;
				    digi = 0;
				    i = 0;
				    while(c[i] >= '0' && c[i] <= '9'){ digi = digi*10 + (c[i] - '0'); i++; if(i > 5) break;}
				    c = tmp;
				    if(*c == 'n') number = digi;	//set number to digi
				    else if(*c == 's') stack_ = digi;
				    if(*c) c++; //if not end - increase c.
				    continue;
	    /* by /sn   - set number = source */
	    /* by /sv   - set value = source */
	    /* by /ss   - set stack = source */
		    case 's': c++; ch = *c; i = 0;
				digi = 0;
				while(*d >= '0' && *d <= '9'){
				    if(i < 7){ digi = digi*10 + (*d - '0'); i++;}
				    d++;
				}
				if(i == 0) return NULL; //in *d no digits
				if(ch == 'n'){ c++; number = digi; continue;}
				else if(ch == 'v'){ c++; value_ = digi; continue;}
				else if(ch == 's'){ c++; stack_ = digi; continue;}
				else { c = c - 2; break;} //go back to c = /sv(n) and parse it
	    /* by /hn   - set number = source (hex) */
	    /* by /hv   - set value = source (hex) */
	    /* by /hs   - set stack = source (hex) */
		    case 'h': digi = 0; ch = 0;
				while(1){
				    i = *d;
				    if(i >= '0' && i <= '9') ch = '0';
				    else if(i >= 'A' && i <= 'F') ch = 'A' - 10;
				    else if(i >= 'a' && i <= 'f') ch = 'a' - 10;
				    else break;
				    digi = (digi<<4) + i - ch;
				    d++;
				}
				if(ch == 0) return NULL;
				c++;
				if(*c == 'n'){ c++; number = digi; continue;}
				else if(*c == 'v'){ c++; value_ = digi; continue;}
				else if(*c == 's'){ c++; stack_ = digi; continue;}
				else { c = c - 2; break;} //go back to c = /hv(n) and parse it
	    /* by /iv<10v or /iv>5v or /in<45n or /in>24n or /is>0s - if not matches return NULL */
	    /* by /iv+10v or /iv-5v or /in-45n or /in+24n or /is+1s */
		    case 'i': c++; ch = *c; 
				if(ch != 'v' && ch != 'n' && ch != 's'){ c = c - 2; break;}
				c++; i = *c;
				if(i != '<' && i != '>' && i != '+' && i != '-'){ c = c - 3; break;}
				c++; digi = 0;
				while(*c){
				    if(*c >= '0' && *c <= '9'){ digi = digi*10 + (*c - '0'); c++; continue;}
				    else if(*c == ch) break;
				    else{ break;}
				}
				if(*c == ch){
				    if(i == '+'){
					if(ch == 'v'){ value_ += digi;}
					else if(ch == 'n'){ number += digi;}
					else if(ch == 's'){ stack_ += digi;}
					c++; continue;
				    }else if(i == '-'){
					if(ch == 'v'){ value_ -= digi;}
					else if(ch == 'n'){ number -= digi;}
					else if(ch == 's'){ stack_ -= digi;}
					c++; continue;
				    }
				    unsigned long val = 0;
				    if(ch == 'v'){ val = value_;}
				    else if(ch == 'n'){ val = number;}
				    else if(ch == 's'){ val = stack_;}
				    if(i == '>'){ if(val <= digi) return NULL;}
				    else if(i == '<'){ if(val >= digi) return NULL;}
				    c++; continue;
				}else return NULL;
	    /* skip zero or one character*/
		    case '0': tmp = d; while(tmp <= d+1){tmp = parsestr1(tmp, c+1); if (tmp) return tmp; tmp++;} return NULL;
	    /* skip one symbol in d, exept \0 */
		    case '|': c++; if(*d == '\0') return NULL; d++; continue;
	    /* by x/.x. skip x-symbol 1 or more times */
	    /* by /.x. skip x-symbol all at once (0 or more times)*/
		    case '.': if(*c != *(c+2) || *(c+1)=='\0') return NULL; //error! - or make it with continue
				    c=c+1; while(*d == *c) d++; c=c+2; continue;
	    /* by /,x, skip x-symbol 0 or 1-time */
		    case ',': if(*c != *(c+2) || *(c+1)=='\0') return NULL; //error!
				    c=c+1; tmp = d; while((d <= tmp+1) && (*d == *c)) d++; c=c+2; continue; //used in *search[]
	    /* by /lxl get last x-symbol in string 1 or more times */
		    case 'l':
	    /* by /LxL get last x-symbol in string 0 or more times*/
		    case 'L': if(*c != *(c+2) || *(c+1)=='\0') return NULL; //error!
				    i=0;c=c+1; tmp=d; while(*tmp){if(*tmp == *c){ d=tmp+1;i=1;} tmp++;}
				    if(*(c+1)=='l'&&i==0)return NULL;c=c+2; continue; //used in serv-others_1.htm
	    /* by /NxN x-symbol is not in string */
		    case 'N': if(*c != *(c+2) || *(c+1)=='\0') return NULL; //error!
				    c=c+1; if(*c == *d) return NULL; c=c+2; d++; continue; //used in 
	    /* skip all blanks or tabs or enters in d */
//		    case ' ': tmp = d; while(*d == ' ' || *d == '\t' || *d == '\n' || *d == '\r') d++; if(tmp == d) return NULL; c++; continue;
		    case ' ': while(*d == ' ' || *d == '\t' || *d == '\n' || *d == '\r') d++; c++; continue;
	    /* skip all blanks and tabs */
		    case 't': while(*d == ' ' || *d == '\t') d++; c++; continue;
	    /* /<xCHARS/> matches CHARS with d, continue if matches and return null if not */
		    case '<': c++;
			switch(*c){
			    case '-': //  /<-CHARS/> matches chars 1 or more times
			    case '*': //  /<*CHARS/> matches chars zero or more times
				c++; tmp = c; i=0; while((*c != '/'|| c[1] != '>') && *c){
				if(*c == '\\'){
				    c++;
				    switch(*c){
				    case '\\':
				    case '-':
				    case '\"':	ch = *c;break;
				    case 't':	ch = '\t';break;
				    case 'n':	ch = '\n';break;
				    default: c--; ch = *c; break;
				    }
				} else {
				    ch = *c;
				    if(*(c+1) == '-' && *(c+2)){	//if /<-a-z/> --> (ch=a <= *d <= *(c+2)=z)
					if((ch < *(c+2)) && (*d >= ch) && (*d <= *(c+2))){
					    do{if(*d == '\0') break; d++;}while((*d >= ch) && (*d <= *(c+2)));
					    i = 1;
					    if(*d){ c = tmp; continue;}
					}
					c = c + 3; continue;
				    }
				}
				if(ch==*d){ do{d++;}while(ch==*d);
				    c = tmp; i=1; continue;
				}
				c++;}
				if(*(tmp-1)=='-' && i==0) return NULL;
				if(*c == '/') c=c+2; continue;
			    case '0': //  /<0CHARS/> matches chars zero or 1 time
			    case '1': //  /<1CHARS/> matches chars one time
			    case 'N': //  /<NCHARS/> all chars not matches one time
				i = 0;tmp = c; c++; while((*c != '/'|| c[1] != '>') && *c){
				if(*c == '\\'){
				    c++;
				    switch(*c){
				    case '\\':
				    case '-':
				    case '\"':	ch = *c;break;
				    case 't':	ch = '\t';break;
				    case 'n':	ch = '\n';break;
				    case '0':	ch = '\0';break;//check if *d == '\0'.
							//In /* is set until zero, inclusive zero!
				    default: c--; ch = *c; break;
				    }
				} else {
				    ch = *c;
				    if(*(c+1) == '-' && *(c+2)){	//if /<1a-z/> --> (ch=a <= *d <= *(c+2)=z)
					if((ch < *(c+2)) && (*d >= ch) && (*d <= *(c+2))) i = 1;
					c = c + 3; continue;
				    }
				}
				if(ch == *d) i = 1; c++;}
				if(*(tmp)=='1' && i==0) return NULL;
				if(*(tmp)=='N' && i==1) return NULL;
				if(*c == '/') c=c+2; if(*d != '\0' && (*(tmp)=='1' || *(tmp)=='N' || (*(tmp)=='0' &&  i==1))) d++; continue;
			    default:
			    case '\0': c--; continue; //  rest='<\0'
			}
		    case '[': if(parsestr1(d, c+1)){ /*point[0] = d;*/ return d;} return NULL;		//if [ in b so this pointer will be returned, and c is move forward. BEGINNofSTR

		    case ']':/* printf("d:%s\n", d);*/
			      tmp = parsestr1(d, c+1);
			      if(!tmp) return NULL;
			      //case tmp=d (*(c+1) = '/0')[ end braces in end of string]
			      if(tmp == d && *tmp){ tmp++; point[1] = tmp;}
			      /*if(*d) point[0] = d+1; else */
			      point[0] = d;	// point[0] points on character to be zeroed
			      ch_zero = *d;
			      *d = '\0'; return tmp;//depend on continues(if /[ goes after /] - begin)
							// make end of string hier!, check rest as (]...) and return next to end char.
	    /* second '/' charakter is to match */
		    case '/': break;
//		    case '\0': continue;//need to check it!!!!!!

	    /* end of string is to match */
		    case '\0': if(*d != '\0') return NULL; return d;
		
		    case '!':		//Negotiation		NEW
					//  \!\*ab	-no more "ab" in rest of string
			    tmp = parsestr1(d, c+1);
			    if(tmp)	return NULL;
			    else { point[1] = d; return d;}
		    case '?':		// /?variable?/	- nicht eingebetet(simple)
		    case 'Q':		// /Qvar?/	- eingebetet
			ch = *c;	//in ch: Q or ?
			c++;
			char *m;
			tmp = c;
			tmp2 = NULL;
			while(*tmp){	//   /?var?/ - is old, /?var/? - is new
			    if((*tmp == '?' && *(tmp+1) == '/') || (*tmp == '/' && *(tmp+1) == '?')){
				i = tmp - c + 1;
				if((i > 1) && (i < 33)){
				m = malloc(i);
				if(m){
				    strmycpy(m, c, i);
				    tmp2 = get_var(NULL, m);		//get_var and get_variable
				    free(m);
				}
				}
				c=tmp+2;
				break;
			    }
			    tmp++;
			}

			if(tmp2 && *tmp2){
			    if(ch == '?'){
				i = strlen(tmp2);
				if(i && strncmp(d, tmp2, i)) return NULL;
				d = d + i;
			    }else{
				static int time = 0;
				time++;
				if(time < 10){
				    unsigned long long size = strncpy_(NULL, tmp2, 0)+1;	//this is max. size of tmp2-string
				    if(size > 1 && (m = malloc(size))){
				    strncpy_(m, tmp2, size);
				    tmp = parsestr1(d, m);
				    free(m);
				    if(!tmp){time--; return NULL;}
				    d = tmp;
				    //if(*d != '\0') d++;		//????????
				    }
				}else printf("parsestr: max. counter\n");
				time--;
			    }
			}
			else return NULL;

			continue;		//it's or c++ or c=tmp+2

		    case '-': if(*(c+1) == '-') { d=d-1; c=c+2;} //  /--   -means d-1
			continue;

		    case 'm': c++; ch = *c; c++; tmp2 = c;// /m-WARN: not matched!\0 -thisIsNotMatched!
			while(*tmp2){
			    if(*tmp2 == '\\' && *(tmp2+1) == '\\') tmp2++;	//tmp2 = tmp2 + 2
			    else if(*tmp2 == '\\' && *(tmp2+1) == '0'){ tmp2 = tmp2 + 2; break;}	//string must have \\0 at the end
			    tmp2++;						//and don't have /\\ or /}, /), /E, /:
			}
			if(ch == 'w') print_pstr(fdcr, c); //putout to WEB: /mw....\0  without matching!
			tmp = parsestr1(d, tmp2);
			if(ch == '-' && tmp == NULL) print_pstr(stderr, c);
			if(ch == '+' && tmp != NULL) print_pstr(stderr, c);
			if(ch == 'N' && tmp == NULL) print_pstr(fdcr, c); //if not mantched the rest - putout to WEB: /mN...\0
			if(ch == 'W' && tmp != NULL) print_pstr(fdcr, c); //if matched the rest - putout to WEB: /mW....\0
			return tmp;
			// /mw..\0/mW..\0/?par/?	==	/mw..\0/?par/?/mW..\0

		    default: c--;
		}
	    } else if(*c == '\\'){
		c++;
		switch(*c){
	    /* new string is to match */
		    case 'n': if(*d != '\n') return NULL; c++; d++; continue;
		    case 't': if(*d != '\t') return NULL; c++; d++; continue;
		    case 'r': if(*d != '\r') return NULL; c++; d++; continue;
	    /* \" string is to match */
		    case '\"': if(*d != '\"') return NULL; c++; d++; continue;	//need to change it
	    /* \0 the same as "bla/"; if after is "bla\02" -> "bla\0" + number=2 */
		    case '0': if(*d != '\0') return NULL;
				    c++; int digi = 0; i = 0;
				    while(c[i] >= '0' && c[i] <= '9'){ digi = digi*10 + (c[i] - '0'); i++; if(i > 5) break;}
				    if(i) number = digi;	//set number to digi
				point[1] = d;//new 14.07.2019
				return d;
		    case '\\': break;
		    default: c--;
		}
	    }

//	    if (tolower(*c) != tolower(*d))  break;
	    if (*c != *d) return NULL;
	    if (!(*d))	  {printf("PARSESTR1<<<<<<</n");break;}	//not happens *d=*c=='\0'

	    c++;
	    d++;
	}

	    point[1] = d;
	    return (char *) d;//ENDofSTR
}

char *parsestr1_( char *d, char *c){		//push and pop the pointers
    char *tmp, *a, *b;

    a = point[0];
    b = point[1];

    tmp = parsestr1(d, c);

    point[0] = a;
    point[1] = b;

    return tmp;
}

char *parsestr2( struct parsestr *ptr, char *d, char *c){		//use the pointers in struct
    char *tmp;

    number = 0;
    point[0] = NULL;
    if(tmp = parsestr1(d, c) /*&& (ptr != NULL)*/){
	ptr->ch = ch_zero;
	ptr->num = number;
	ptr->zero = point[0];	//if NULL -> restore_str is not made
	ptr->end = point[1];
    }

    return tmp;
}

char *restore_str( struct parsestr *ptr){	//note: in parsestring MUST BE-> "/]"
    if(ptr->zero){
	*(ptr->zero) = ptr->ch;
    }
    return ptr->end;
}

int copy(FILE *read_f, FILE *write_f)	//used by rename_ aswell
{
  int n;
  int wrote;

//  alarm(TIMEOUT);
  while ((n = fread(copybuf,1,sizeof(copybuf),read_f)) != 0) {
////    alarm(TIMEOUT);
    wrote = fwrite(copybuf,n,1,write_f);
////    alarm(TIMEOUT);
    if (wrote < 1)
	return -1;
  }
//  alarm(0);
  return 0;
}

int copy_file(char *file, FILE *out)
{
  FILE *fp;
  int n;
  int wrote;
  char *err_str="[ERR] File: %s cannot open\n";

  if((fp=fopen(file,"r")) == NULL){
    printf(err_str, file);
    fprintf(out, err_str, file);
    return 1;
  }
  else{
    while ((n = fread(copybuf,1,sizeof(copybuf),fp)) != 0) {
	wrote = fwrite(copybuf,n,1,out);
	if (wrote < 1){
	    fclose(fp);
	    return 1;
	}
    }
    fclose(fp);
  }
  return 0;
}

/*if file is *.inc - then parse it by handle_get, otherwise put out without parsing*/
int copy_file_include(char *file, FILE *out)
{
	char *ptr = file;
	int i = 0, ret = 0;
	
	while(*ptr){
	    if(*ptr == '?'){
		*ptr = '\0';
		i = 1;
		break;
	    }
	    ptr++;
	}
	
	if(i){
//here must be arg_handle for ptr+1
	handle_arg(1, ptr+1);
	}

//        if(parsestr1_(file, "/l.linc/") || parsestr1_(file, "/l.lhtm/")){ // 
	if(parsestr1_(file, "/l.l/Binc\\0/\\htm\\0/\\")){		//like: include.inc or incude.htm
	    FILE *infile;
	    char *data;
	    struct stat stbuf;

	    if(stat(file, &stbuf) || (stbuf.st_size>1024*64) || !(infile = fopen(file,"r"))) {
		fprintf(out, "Unable to open HTML file %s, %d\n", file, errno);
		if(i){
		    *ptr = '?';
		    free_arg(1);
		}
		return 1;
	    }

	    data = (char *)malloc(stbuf.st_size+1);
	    if(data){
		fread(data, stbuf.st_size, 1, infile);
		data[stbuf.st_size] = '\0';
		handle_get(data, out);
		free(data);
	    } else {
		fprintf(out, "ERR: Unable allocate memory.\n");
		//ret = 1;//error alocate
	    }
	    fclose(infile);
        } else ret = copy_file(file, out);		//all others will only copied
  if(i){
    *ptr = '?';
    free_arg(1);
  }
  return ret;
}

int copy_file_buf(char *file, char *buf, long long size)
{
  FILE *fp;
  int n;
  int wrote;
  char *err_str="[ERR] File: %s cannot open\n";

  if(!size) return -1;
  if((fp=fopen(file,"r")) == NULL){
    printf(err_str, file);
    return -1;
  }
  else{
//    while ((n = fread(buf,1,size-1,fp)) != 0) {
//	}
    fread(buf,size-1,1,fp);
    fclose(fp);
  }
  buf[size-1] = '\0';
  return 0;
}

// find there is a input type is "radio" in this line and insert the value for displaying
//inline char *radio_value_insert(char *line, char *var_head, FILE *out)
char *radio_value_insert(char *line, char *var_head, FILE *out)
{
    char *file_head1, *file_head2, *radio_var, *ptr;

    if((radio_var=parsestr1(var_head,"/*value=\"")) && (file_head1=parsestr1(var_head,"/*name=\"")))
    {
	if(file_head1<radio_var){
	    file_head2=radio_var;	//last pointer
	    ptr=file_head1;		//neaher pointer
	}else{
	    file_head2=file_head1;	//last pointer
	    ptr=radio_var;		//neaher pointer
	}
	file_head2=strchr(file_head2,'\"');
	file_head2[0]='\0';
	file_head2++;
	fprintf(out,"%s\"", line);

	ptr=strchr(ptr,'\"');
	ptr[0]='\0';

	ptr = get_cfg_value(NULL, file_head1, 0);	//another ptr
	if(ptr && (strcmp(ptr,radio_var)==0) )
		    fprintf(out,"checked");
	return file_head2;
    } else {
	putc(*line, out);	//put out "<" the rest will be made by handle_get
	return (line+1);
    }
}

//register new parameter called name, from value, size-lang.
//return 1=success and 0=false
int reg_par(char *name, char *value, long long size)
{
	struct cfg_parse1 *ptr = cfg1;
	while(ptr){
		if(ptr->type == CFG_TMP && !strcmp(name, ptr->web_name)){
			ptr->size = size;
			ptr->value = value;
			ptr->new_value = value;
			return 1;
		}
	ptr = ptr->next;
	}
	return 0;

}

//inline void include_(char *var_head, FILE *out)	//SSI made total brutal, no checks of file path, and include must be along in string of file!
void include_(char *var_head, FILE *out)
{
    char *var_head2;
    struct parsestr strct;

do{
    if ((var_head2 = parsestr2(&strct, var_head,"par=\"/[/*/]\"/ "))!=NULL){	//<!--#include par="size:web_name:name:pattern" -->
	/*collect parameters in a array*/
	char *a, *ptr;
	int i = 0, str_size, val_size;
	struct cfg_parse1 *cfg_pointer;
	*cfg_p = (struct cfg_parse1 *)malloc(sizeof(struct cfg_parse1));

	cfg_pointer = *cfg_p;
	if(cfg_pointer == NULL) continue;
	ptr = var_head2;

	    while(i < 3){
		a = w_strtok(&ptr, ':');
	        if(a)
		    switch(i){
		    case 0:	val_size = atoi(a);
				if(!val_size || val_size > 300) return;	//size is in impassable range - so skip it all (whole parameter string will be skipped!)
				a = ptr;
				str_size = strlen(a)+1;
				ptr = (char*)malloc(str_size + 2*val_size);//str = "[part of var_head2][value][new_value]"
				if(ptr == NULL){
					i = 5; continue;
				}
				strcpy(ptr, a);
				cfg_pointer->type = CFG_PAR;
				cfg_pointer->str = ptr;
				cfg_pointer->size = val_size;
				cfg_pointer->changed = 0;
				cfg_pointer->saved = 0;
				cfg_pointer->value = ptr + str_size;
//				*(cfg_pointer->value) = '\0';
				cfg_pointer->new_value = ptr + str_size + val_size;
//				*(cfg_pointer->new_value) = '\0';
				memset(cfg_pointer->value, 0, 2*val_size);//fill with zeros
				cfg_pointer->name = NULL;
				cfg_pointer->pattern = NULL;
				cfg_pointer->web_name = NULL;
				cfg_pointer->next = NULL;
				break;
	    	    case 1:	cfg_pointer->web_name = a; break;
		    case 2:	cfg_pointer->name = a; cfg_pointer->pattern = ptr; break;
		    }
		else {
		    printf("par=\"\" not full, broken by %d\n", i+1);
		    break;
		}
	    i++;
	    }
	if(i != 5){
printf("Collected parameter: %s:%s:%lld:%s\n", cfg_pointer->web_name, cfg_pointer->name, cfg_pointer->size, cfg_pointer->pattern);
	    cfg_p = &(cfg_pointer->next);
	}else{
	    printf("ERROR allocate memory\n");
	    if(cfg_p && *cfg_p){
		free(*cfg_p);
		*cfg_p = NULL;	//main criteria to abort moving in array.
	    }
	}

    } else if ((var_head2 = parsestr2(&strct, var_head,"readcfg/ "))!=NULL){	//<!--#include readcfg -->
	/*start to fill parameters from config to collected array*/
	ReadConfiguration();
    } else if ((var_head2 = parsestr2(&strct, var_head,"area=\"/[/*/]\"/ "))!=NULL){	//<!--#include area="size:web_name" -->
	/*collect parameters in a array*/
	char *a, *ptr;
	int i = 0, str_size;
	long long val_size;
	struct cfg_parse1 *cfg_pointer;
	*cfg_p = (struct cfg_parse1 *)malloc(sizeof(struct cfg_parse1));

	cfg_pointer = *cfg_p;
	if(cfg_pointer == NULL) continue;
	ptr = var_head2;

	    while(i < 2){
		a = w_strtok(&ptr, ':');
	        if(a)
		    switch(i){
		    case 0:	val_size = atoll(a);
				if(val_size > (100 *1024)) return;	//size is in impassable range - so skip it all (whole parameter string will be skipped!)
				a = ptr;
				str_size = strlen(a)+1;
				ptr = (char*)malloc(str_size + val_size);//str = "[part of var_head2][value]"
				if(ptr == NULL){
					i = 5; continue;
				}
				strcpy(ptr, a);
				cfg_pointer->type = (val_size != 0) ? CFG_AREA : CFG_TMP;
				cfg_pointer->str = ptr;
				cfg_pointer->size = val_size;
				cfg_pointer->changed = 0;
				cfg_pointer->saved = 0;
				if(val_size != 0){//AREA="size:name"
				cfg_pointer->value = ptr + str_size;
				cfg_pointer->new_value = ptr + str_size;
				memset(cfg_pointer->value, 0, val_size);//fill memory with 0
				}else{//AREA="0:temp_name"
				cfg_pointer->value = NULL;
				cfg_pointer->new_value = NULL;
				}
				cfg_pointer->name = NULL; //ptr + str_size - 1;
				cfg_pointer->pattern = NULL; //ptr + str_size - 1;
				cfg_pointer->web_name = NULL;
				cfg_pointer->next = NULL;
				break;
		    case 1:	cfg_pointer->web_name = a; break;
		    }
		else {
		    printf("area=\"\" not full, broken by %d\n", i+1);
		    break;
		}
	    i++;
	    }
	if(i != 5){
printf("Collected parameter: [%s:%lld]\n", cfg_pointer->web_name, cfg_pointer->size);
	    cfg_p = &(cfg_pointer->next);
	}else{
	    printf("ERROR allocate memory\n");
	    if(cfg_p && *cfg_p){
		free(*cfg_p);
		*cfg_p = NULL;	//main criteria to abort moving in array.
	    }
	}

    } else
    
//printf("include file: %s\n", var_head);
    if ((var_head2 = parsestr2(&strct, var_head,"file=\"/[/*/N\\N/]\"/ "))!=NULL){	//<!--#include file="..\".." -->

//printf("include file: %s\n", var_head2);
	copy_file_include(var_head2, out);
    } else
    if ((var_head2 = parsestr2(&strct, var_head,"exec=\"/[/*/]\"/ "))!=NULL){	//<!--#include exec="..." -->

        my_system(out, var_head2);
    } else if ((var_head2 = parsestr2(&strct, var_head,"shell=\"/[/*/N\\N/]\"/ "))!=NULL){	//<!--#include shell="..." -->

        my_shell(out, var_head2);
    } else if ((var_head2 = parsestr2(&strct, var_head,"cgi=\"/[/*/]\"/ "))!=NULL){	//<!--#include cgi="..." -->

	if(DoCGI(out, var_head2, 1))  fprintf(out, "Not Found: %s\n", var_head2);// what about arg??
	else  free_par_tmp();//clear all "temp"-parameters

    } else if((var_head2 = parsestr2(&strct, var_head, "tbl_select=\"/[/*/]\"/ "))!=NULL){

	show_tbl(var_head2, out);
    } else if((var_head2 = parsestr2(&strct, var_head, "tbl_check=\"/[/*/]\"/ "))!=NULL){

	show_tbl_chck(var_head2, out);
    } else if((var_head2 = parsestr2(&strct, var_head, "table=\"/[/*/N\\N/]\"/ "))!=NULL){

	tabs(var_head2, out);
    } else if((var_head2 = parsestr2(&strct, var_head, "exchange=\"/[/*/N\\N/]\"/ "))!=NULL){

	reg_ticket(var_head2, 0);
    } else if((var_head2 = parsestr2(&strct, var_head, "ticket=\"/[/*/N\\N/]\"/ "))!=NULL){

	reg_ticket(var_head2, 1);
    } else if((var_head2 = parsestr2(&strct, var_head, "chtbl_stat=\"/[/*/]\"/ "))!=NULL){

	change_tbl_stat(var_head2);
    } else if((var_head2 = parsestr2(&strct, var_head, "wr_shell=\"/[/*/N\\N/]\"/ "))!=NULL){

	wr_shell(var_head2, 0);//0->write_system, not cat!!
    } else if((var_head2 = parsestr2(&strct, var_head, "InIt/ "))!=NULL){		//<!--#include InIt -->

	ReadConfiguration();
    } else if((var_head2 = parsestr2(&strct, var_head, "write_par=\"/[/*/N\\N/]\"/ "))!=NULL){	//<!--#include write_par="par:value" -->

		// write_par  par:value		this is used in copy_CGI.c - the same code!
		char *tmp, *tmp2;
		long long size;
			    tmp2 = w_strtok(&var_head2, ':');
			    if(tmp2 && *tmp2 && *var_head2){
			    	tmp = get_var(&size, tmp2);
				if(tmp && size){
				    strncpy_(tmp, var_head2, size-1);
//				    tmp[size-1] = '\0';
				}
//				if(tmp2 != var_head2) *(var_head2-1) = ':';
			    }
    }//need some end hier (else)

}while(var_head2 && (var_head = strct.end));	//end of parsing string

}

//free all parameter marked as "temp"
void free_par_tmp(void){
	struct cfg_parse1 *ptr = cfg1;
	while(ptr){
	if(ptr->type == CFG_TMP){
		ptr->size = 0;
#ifdef DEBUG
		printf("-->clear: %s\n", ptr->web_name);
#endif
	}
	ptr = ptr->next;
	}
}

//clear all parameters
void free_par(struct cfg_parse1 **ptr){
	if(!ptr || !*ptr) return;
	free_par(&((*ptr)->next));
printf("FREEpar %s\n", (*ptr)->str);
	if((*ptr)->str) free((*ptr)->str);
	free(*ptr);
	*ptr = NULL;
}

void free_page_mem(void){
    free_par(&cfg1);
    cfg_p = &cfg1;

    free_cgi(cgi_name);
    cgi_name = NULL;
    free_tbl();
    free_ticket();
}

extern config CONFIG;	//used by get_var function


char *get_var(unsigned long long *size_ptr, char *var_index){

    char *ptr = NULL;
    unsigned long long size = 0;	//if Zero - not write able

    if(*var_index == '_'){	/* global variables */
	var_index++;

        if(*var_index == '_'){	//??__variable?? - show environment variable
	    var_index++;
	    ptr = getenv(var_index);
	}else if(*var_index == '#'){	//??_#variable?? - show command variable index.html?variable=5
	    var_index++;
	    if(*var_index == '#'){	//??_##variable?? - show local command variable index.html?variable=5
		var_index++;
		return get_arg(var_index, size_ptr, 1);
	    }else if(*var_index == '&'){	//??_#&variable?? - show local command variable, if not exist - show global variable index.html?variable=5
		var_index++;
		ptr = get_arg(var_index, size_ptr, 1);
		if(ptr) return ptr;
	    }
	    return get_arg(var_index, size_ptr, 0);
//	    if(ptr) size = strlen(ptr) + 1;//for remove_show_chars needed
	}else if(*var_index == '%'){	//??_%variable?? - show new_variable
	    var_index++;
	    return get_cfg_value(size_ptr, var_index, 1);
	}else if(*var_index == '&'){	//??_&variable?? - show fresh_variable
	    var_index++;
	    return get_cfg_value(size_ptr, var_index, 2);
	}else if(*var_index == '@'){	//??_@variable?? - show variable from rnd table
	    var_index++;
	    ptr = get_tbl(var_index);
	}else if(*var_index == '?'){	//??_?file|expression?? -> in file this expression
	    var_index++;

	FILE *fip;
	char *ptr1;
	
	
	ptr1 = w_strtok(&var_index, '|');
	if (ptr1){
	
	    if((fip = fopen(ptr1,"r")) == NULL)
		printf("No file: %s\n", ptr1);
	    else{
	    while(fgets(copybuf,sizeof(copybuf),fip) != NULL){
		if((ptr1 = parsestr1_(copybuf,var_index)) != NULL){
		    ptr = ptr1;
		    break;
		}
	    }
	    fclose(fip);
	    }
	}
	return ptr;
	} else if(!strcmp(var_index,"referer")){
	    ptr = referer;
	} else if(!strcmp(var_index,"short_referer")){
/*	    ptr = "";
    	    char *c = referer;	//get only name of html
    	    while (*c){
		if(*c == '/') ptr = c + 1;
		c++;
	    }
*/		ptr = parsestr1_(referer, "/L/L");	//get the last '/'
	}else if(!strcmp(var_index,"user_agent")){
	    ptr = user_agent;
	}else if(!strcmp(var_index,"ip")){
	    ptr = ip;
	    size = 20;
	}else if(!strcmp(var_index,"port")){
	    ptr = port;
	    size = 10;
	}else if(!strcmp(var_index,"srv_ip")){
	    ptr = CONFIG.IP;
	}else if(!strcmp(var_index,"srv_port")){
	    ptr = CONFIG.ADMIN_PORT;
	}else if(!strcmp(var_index,"dns_name")){
	    ptr = dns_name;
	}else if(!strcmp(var_index,"etc_save")){
	    ptr = etc_save;
	    size = 2;
	}else if(!strcmp(var_index,"file_name")){
	    ptr = file;
	}else if(!strcmp(var_index,"buf")){
	    ptr = buf;
	    size = 16384;
	}

    
    }/*end of global variables*/
    else  return get_cfg_value(size_ptr, var_index, 0); /*if not found - return NULL*/
    
    if(size_ptr) *size_ptr = size; 
    return ptr;
}
/**used by DoHTML for scanning some values in http-files and insert DATA and send them to browser**/
//inline char *handle_get_(char *data, FILE *write_f, int make, int loop)
char *handle_get_(char *data, FILE *write_f, int make, int loop)
{
    char *var_index, *ticket;
    char *sel_value='\0';
    int mk = 1;

//in search used parsestr1()

    struct parsestr strct;
    char *file_head;

while(*data){
    if(var_index = parsestr2(&strct, data, "/<1<?/>/--/n0n/B"
		  "<!--/ if=\"/[/*/N\\N/]\"/ -->/,\n,/n1n"	//1
		  "/\\<!--/ else/ -->/,\n,/n2n"		//2
		  "/\\<!--/ //if/ -->/,\n,/n3n"		//3
		  "/\\<!--/ CGI:/m-ERR:incorrect cgi\n\\0/t/[/<-a-zA-Z0-9._/>\n/{-/n0s"
				"/(S/B/is>0s{/\\/E"
				    "/{R/{*/ /B//*/**///\\/////*\n/E/}/ /<-a-zA-Z0-9_/>/ /B\"\"/\\\"/*/N\\N\"/\\/E/ "
										    "/B;/\\{/--/is+1s/E/}"
				"/:/is>0s/{*/ /B//*/**///\\/////*\n/E/}/ }/is-1s/)"
			"/}/{*/ /B//*/**///\\/////*\n/E/}/ /]END_CGI/ -->/n4n"	//4
//		  "/\\<!--/ CGI:/ /[/*/]END_CGI/ -->/n4n"	//4
		  "/\\??/[/*/]??/n5n"		//5
		  "/\\<!--/ #include/ /[/{-/BInIt/\\readcfg/\\/*=\"/*/N\\N\"/E/ /}/]-->/,\n,/n6n"
		  "/\\<!--/ TABLE:/ /[/*\n/]END_TABLE/ -->/n7n"	//7
//		  "/\\<input/ type=\"/[/*/]\""	//maybe replace via 4
		  "/\\<input/ type=\"radio\"/n8n"
		  "/\\<select/*name=\"/[/*/]\"/n9n"		//9
		  "/\\<option/*value=\"/[/*/]\"/n10n"
		  "/\\<!--/{*/B\"\"/\\\"/*/N\\N\"/\\//*/**///\\/////*\n/\\\\\"/\\/|/E/}/B-->/,\n,/n11n/\\\\011/E"
//		  "/\\<!--/*-->/n11n"
		  )){
	//do something with i.
	switch(strct.num){
	    case 1:
		mk = 1;
		if(make){
		    if(cfg_arg_strcmp(var_index, 0)){//par -not exist or not match
			mk = 0;
		    }
		}
		data = restore_str(&strct);
		data = handle_get_(data, write_f, (make && mk), loop + 1);
		if(var_index = parsestr1(data, "<!--/ else/ -->/,\n,")){
		    if(mk) mk = 0;
		    else mk = 1;
		    data = handle_get_(var_index, write_f, (make && mk), loop + 1);
		}
		if(var_index = parsestr1(data, "<!--/ //if/ -->/,\n,")){
		    data = var_index;
		}
		continue;
	    case 3: //end of if, go out
//		    data = restore_str(&strct);
	    case 2: //end of else
		if(loop) return data;
		else {
		    printf("handle_get: \"if\" or \"else\" not present!\n");
		    break;
		}
	}
	if(make) switch(strct.num){
	    case 4: //CGI script
		    parse_cgi_script(var_index);
	    case 5:
		    //show variables value
		    file_head = get_var(NULL, var_index);

		    if(file_head) print(write_f, file_head);/*fprintf(write_f,"%s", file_head);*/
#ifdef DEBUG
		    printf(" Var_index %s file_head %s\n", var_index, file_head);
#endif
		    break;
	    case 6:
		    //include descriptor
#ifdef DEBUG
printf("INCLUDE:%s:END\n", var_index);
#endif
			include_(var_index, write_f);
		    break;
	    case 7:
		    //table
#ifdef DEBUG
printf("TAB:%s:END\n", var_index);
#endif
			parse_tbl(var_index, 0);//without clean
		    break;
	    case 8:
		    //input-form-atribute
/*        	    if (!strcmp(var_index, "text") || 
			!strcmp(var_index, "password") ||
			!strcmp(var_index, "hidden"))	//main used for def. input text
			text_value_insert(file_head, point[1], write_f);
//			text_value_insert(file_head, ptr, write_f);
		    else */ 
//		    if (!strcmp(var_index, "radio"))
			data = radio_value_insert(data, var_index, write_f);
//		    else fprintf(write_f,"%s\"", data);
//		    break;
		    continue;
	    case 9:
		    //select-form-attribute
		    // find there is a 'select drop-down list' in this line and insert the value for displaying
		    fprintf(write_f,"%s\"", data);
		    sel_value = get_cfg_value(NULL, var_index, 0);
		    break;
	    case 10:
		    //option-form-attribute
		    fprintf(write_f,"%s\"", data);
		    if(sel_value && strcmp(sel_value,var_index)==0)
			fprintf(write_f, " selected");
#ifdef DEBUG
		    printf("select_value_insert: %s, =? value: %s\n", var_index, sel_value);
#endif
		    break;
	    case 11: break;//comments not show!!
	}
	data = restore_str(&strct);
//printf("INDEX %d\n", i);
    }
    else {
	if(make) {
	    if(ticket = ticket_find(&data)){ fprintf(write_f, "%s", ticket); continue;}
	    putc(*data, write_f);
	}
	data++;
    }
}

    if(loop) printf("handle_get: \"fi\" not present!\n");
    return data;
}


int handle_get(char *data, FILE *write_f)
{

static int loop_counter = 0;	// used if loops exissted
if(loop_counter > 10){
	fprintf(write_f, "max loop counter reached\n");
	return 0;
	}
loop_counter++;

//    alarm(TIMEOUT);//RAW

//    free_page_mem();	//free memory of page includes


handle_get_(data, write_f, 1, 0);

loop_counter--;
//    printf("Time to ALARM: %d\n", alarm(0));//RAW
  return 0;
}

//begin of ticket------------------------------------
struct ticket *ticket_list = NULL;

void reg_ticket(char *name, int type){
//if type == 0 -> exchange=""
//overweis == 1 -> ticket=""
    char *str;
//    if(type == 0){
    unsigned long long size;
    if(type == 0) size = strncpy_(NULL, name, 0)+1;
    else size = strlen(name)+1;
    if(size > 1 && (str = malloc(size))){
	if(type == 0) strncpy_(str, name, size);
	else sprintf(str, "%s", name);
    } else {
	printf("ERR: len_size of argument <= 1 or unable allocate memory for ticket\n");
	return;
    }
//    }else str = name;

    struct ticket *tmp, **t = &ticket_list;
    while(*t){
	if(!strcmp((*t)->fname, str)){//if exist - free end exit
	    free(str);
	    return;
	}
	t = &((*t)->next);
    }
    tmp = (struct ticket *)malloc(sizeof(struct ticket));
    if(tmp == NULL){
	printf("ERR: Unable allocate memory for ticket\n");
	return;
    }
    *t = tmp;
    tmp->type = type;
    tmp->fname = str;
    if(type == 0) sprintf(tmp->ticket, "%ld_%ld", random(), random());//max 64byte
    else *(tmp->ticket) = '\0';
    tmp->next = NULL;
//#ifdef DEBUG
printf("%s\n", tmp->ticket);
printf("Ticket registered: %s\n", tmp->fname);
//#endif
}

char *check_ticket(char *name, int type){
    struct ticket *t = ticket_list;
    char *ptr;

    if(type == 0){
	while(t){
	    if(t->type == 0 && !strcmp(t->ticket, name)) return t->fname;
	t = t->next;
	}
    }else{
	while(t){
	    if(t->type == 1 && (ptr = parsestr1(name, t->fname))!=NULL) return ptr;
	t = t->next;
	}
    }

    return NULL;
}

char *ticket_find(char **name){
    if(name == NULL) return NULL;
    struct ticket *t = ticket_list;
    unsigned long size;
    while(t){
    if(t->type == 0){
	size = strlen(t->fname);
	if(!strncmp(t->fname, *name, size)){ *name = *name + size; return t->ticket;}
    }
	t = t->next;
    }
    return NULL;
}

void free_ticket_1(struct ticket **ptr){
    if(!ptr || !*ptr) return;
    free_ticket_1(&((*ptr)->next));
#ifdef DEBUG
    printf("free ticket '%s'\n", (*ptr)->fname);
#endif
    free((*ptr)->fname);	//at hier action
    free(*ptr);
    *ptr = NULL;
}

void free_ticket(void){
    free_ticket_1(&ticket_list);
//    ticket_list = NULL;
}


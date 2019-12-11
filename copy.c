/* copy.c:
 *
 * Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>,
 *		       The Silver Hammer Group, Ltd.
 *		 2009-2018  Alexander Reimer <alex_raw@rambler.ru>
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

char *point[2];		//0 - place where is ch_zero by /]; 1 - end of string
char ch_zero = '\0';
unsigned int number = 0;

char *parsestr1( char *d, char *c)	//try identic strings!, "xxx*NULL" combination
{
	char *tmp, *tmp2, ch;
	unsigned int i;

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
				    while(1){
					if(*c == '/' && *(c+1) == '/') c++; //c+=2
					else if(*c == '/' && *(c+1) == '\\'){c = c + 2; break;} // if(/\) 
					else if((tmp && (c == tmp)) || (*c == '\0')) return NULL;	//end of compare-strings - no matches - return NULL
					c++;
				    }
				}
//printf("str:%s d=%s c=%s\n", tmp2,d,c);
				if(tmp) {d = tmp2; /*tmp2 is not always the end of string!! check it out*/
					c = tmp + 2; continue;}
				else return tmp2;

		    case '\\':
		    case 'E': point[1] = d;//NEW 20.03.2016
				return d;
	    /*  set number to /n10n   */
		    case 'n': c++; tmp = c;
				while(*tmp && *tmp != 'n') tmp++;
				    int digi = 0; i = 0;
				    while(c[i] >= '0' && c[i] <= '9'){ digi = digi*10 + (c[i] - '0'); i++; if(i > 5) break;}
				    number = digi;	//set number to digi
				    c = tmp;
				    if(*tmp) c++; //if not end - increase c.
				    continue;
	    /* skip zero or one character*/
		    case '0': tmp = d; while(tmp <= d+1){tmp = parsestr1(tmp, c+1); if (tmp) return tmp; tmp++;} return NULL;
	    /* skip one symbol in d, exept \0 */
		    case '|': c++; if(*d == '\0') return NULL; d++; continue;
	    /* by /.x. skip x-symbol all at once */
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
	    /* /<xCHARS/> matches CHARS with d, continue if matches and return null if not */
		    case '<': c++;
			switch(*c){
			    case '-': //  /<-CHARS/> matches chars 1 or more times
			    case '*': //  /<*CHARS/> matches chars zero or more times
				c++; tmp = c; i=0; while((*c != '/'|| c[1] != '>') && *c){
				if(*c == '\\'){
				    c++;
				    switch(*c){
				    case '\\':	ch = '\\';break;
				    case 't':	ch = '\t';break;
				    case 'n':	ch = '\n';break;
				    case '\"':	ch = '\"';break;
				    default: c--; ch = *c; break;
				    }
				} else ch = *c;
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
				    case '\\':	ch = '\\';break;
				    case 't':	ch = '\t';break;
				    case 'n':	ch = '\n';break;
				    case '\"':	ch = '\"';break;
				    case '0':	ch = '\0';break;//check if *d == '\0'.
							//In /* is set until zero, inclusive zero!
				    default: c--; ch = *c; break;
				    }
				} else ch = *c;
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
			while(*tmp){
			    if(*tmp == '?' && *(tmp+1) == '/'){
				i = tmp - c;
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
void radio_value_insert(char *line, char *var_head, FILE *out)
{
    char *file_head1, *file_head2, *radio_var, *ptr;

    if((radio_var=parsestr1(var_head,"/*value=\"")) && (file_head1=parsestr1(var_head,"/*name=\"")))
    {
	if(file_head1<radio_var){
	    file_head2=radio_var;
	    ptr=file_head1;
	}else{
	    file_head2=file_head1;
	    ptr=radio_var;
	}
	file_head2=strchr(file_head2,'\"');
	file_head2[0]='\0';
	file_head2++;
	fprintf(out,"%s\" %s\"", line, var_head);

	ptr=strchr(ptr,'\"');
	ptr[0]='\0';
	ptr++;

	ptr = get_cfg_value(NULL, file_head1, 0);	//another ptr
	if(ptr && (strcmp(ptr,radio_var)==0) )
		    fprintf(out,"checked");
	point[1] = file_head2;
    }
    else fprintf(out, "%s\"", line);	/* send out the remain of line */
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

void include_(char *line, char *var_head, FILE *out)	//SSI made total brutal, no checks of file path, and include must be along in string of file!
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
				if(val_size > 32 *1024) return;	//size is in impassable range - so skip it all (whole parameter string will be skipped!)
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
    } else if ((var_head2 = parsestr2(&strct, var_head,"shell=\"\"/[/*/]\"\"/ "))!=NULL){	//<!--#include shell=""..."" -->

        my_shell(out, var_head2);
    } else if ((var_head2 = parsestr2(&strct, var_head,"cgi=\"/[/*/]\"/ "))!=NULL){	//<!--#include cgi="..." -->

	if(DoCGI(out, var_head2))  fprintf(out, "Not Found: %s\n", var_head2);// what about arg??
	else  free_par_tmp();//clear all "temp"-parameters

    } else if((var_head2 = parsestr2(&strct, var_head, "tbl_select=\"/[/*/]\"/ "))!=NULL){

	show_tbl(var_head2, out);
    } else if((var_head2 = parsestr2(&strct, var_head, "tbl_check=\"/[/*/]\"/ "))!=NULL){

	show_tbl_chck(var_head2, out);
    } else if((var_head2 = parsestr2(&strct, var_head, "chtbl_stat=\"/[/*/]\"/ "))!=NULL){

	change_tbl_stat(var_head2);
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
int handle_get(char *data, FILE *write_f)
{
    char *var_index;
    char *sel_value='\0';
    int if_prm = 1;

static int loop_counter = 0;	// used if loops exissted
if(loop_counter > 10){
	fprintf(write_f, "max loop counter reached\n");
	return 0;
	}
loop_counter++;

//    alarm(TIMEOUT);//RAW

//    free_page_mem();	//free memory of page includes

//in search used parsestr1()
char *search[] = {"<!--/ if=\"/[/*/]\"/ -->/,\n,",	//0
		  "<!--/ else/ -->/,\n,",		//1
		  "<!--/ //if/ -->/,\n,",		//2
		  "<!--/ CGI:/ /[/*/]END_CGI/ -->",	//3
		  "??/[/*/]??",		//4
		  "<!--/ #include/ /[/*/]-->/,\n,",
		  "<!--/ TABLE:/ /[/*/]END_TABLE/ -->",	//6
//		  "<input/ type=\"/[/*/]\"",	//maybe replace via 4
		  "<input/ type=\"radio\"",
		  "<select/*name=\"/[/*/]\"",		//8
		  "<option/*value=\"/[/*/]\"",
		  "<!--/*-->",
		  NULL};



int i = 0;
char *ptr, *file_head;

while(*data){
    if(*data == *search[i] && (var_index = parsestr1(data+1, search[i]+1))){
    ptr = point[1];
	//do something with i.
	switch(i){
	    case 0:
	    	    if(cfg_arg_strcmp(var_index, 0)){		//compare WEB_NAME
		    //not matching, skip
		    if_prm = 0;
		    } else
		    // is matching, print
		    if_prm = 1;
		    break;
	    case 1: if_prm = !if_prm;	//invers
		    break;
	    case 2:
		    //is matching, print
		    if_prm = 1;
		    break;
	}
	if(if_prm) switch(i){
	    case 3: //CGI script
		    parse_cgi_script(var_index);
		    point[1] = ptr;
	    case 4:
		    //show variables value
		    file_head = get_var(NULL, var_index);

		    if(file_head) print(write_f, file_head);/*fprintf(write_f,"%s", file_head);*/
#ifdef DEBUG
		    printf(" Var_index %s file_head %s\n", var_index, file_head);
#endif
			point[1] = ptr;
		    break;
	    case 5:
		    //include descriptor
			include_(data, var_index, write_f);
			point[1] = ptr;
		    break;
	    case 6:
		    //table
			parse_tbl(var_index);
			point[1] = ptr;
		    break;
	    case 7:
		    //input-form-atribute
/*        	    if (!strcmp(var_index, "text") || 
			!strcmp(var_index, "password") ||
			!strcmp(var_index, "hidden"))	//main used for def. input text
			text_value_insert(file_head, point[1], write_f);
//			text_value_insert(file_head, ptr, write_f);
		    else */ 
//		    if (!strcmp(var_index, "radio"))
			radio_value_insert(data, point[1], write_f);
//		    else fprintf(write_f,"%s\"", data);
		    break;
	    case 8:
		    //select-form-attribute
		    // find there is a 'select drop-down list' in this line and insert the value for displaying
		    fprintf(write_f,"%s\"", data);
		    sel_value = get_cfg_value(NULL, var_index, 0);
		    break;
	    case 9:
		    //option-form-attribute
		    fprintf(write_f,"%s\"", data);
		    if(sel_value && strcmp(sel_value,var_index)==0)
			fprintf(write_f, " selected");
#ifdef DEBUG
		    printf("select_value_insert: %s, =? value: %s\n", var_index, sel_value);
#endif
		    break;
	    case 10: break;//comments not show!!
	}
	data = point[1];
	i = 0;
//printf("INDEX %d\n", i);
    }
    else {
        i++;
//	if(!if_prm && (i==4)) {i= 0;continue;}//if_prm ==0 =>check only first three lines
	if(search[i]) continue;
	i = 0;
//	if(if_prm) fprintf(write_f,"%c", *file_head);
	if(if_prm) putc(*data, write_f);
	data++;
    }
}

loop_counter--;
//    printf("Time to ALARM: %d\n", alarm(0));//RAW
  return 0;
}

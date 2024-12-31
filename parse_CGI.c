/* parse_CGI.c:
 *
 * Copyright (C) 2009-2020  Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */



#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>		//used for malloc & free
#include <string.h>
//#include <ctype.h>
#include "include/httpd.h"
#include "parse_CGI.h"
#include "copy_tbl.h"		//for find_tbl()
#include "copy_CGI.h"
#include "copy.h"


#define MIN(a,b) ((a) < (b) ? (a) : (b))
//#define DEBUG

CGI_ENTRY cgi_entries[] = {
    {"_show_CGIs.cgi",show_CGIs},		//used from copy_CGI.c
    {"",NULL}
};

extern char *print200ok_mime;

//#include "include/httpd_config.h"		//for cfg_parse? struct

struct ARGS *args_ptr_local = NULL;
struct ARGS *args_ptr_global = NULL;

/*this is for CGI*/
char *get_arg(char *name, unsigned long long *size, int flag){
//if flag = 0 -> global args are parsed, if 1 -> local args_ptr are parsed.
    struct ARGS *ptr;

    if(size) *size = 0;		//clear the size (non writeable by default !!)
    if(!name) return NULL;
    if(!flag) ptr = args_ptr_global;
    else ptr = args_ptr_local;	//must be preseted befor execution of this function.
    while(ptr && ptr->name && ptr->value){
	if((ptr->marked == 0) && (strcmp(name, ptr->name) == 0)){
	    if(size) *size = ptr->size;
#ifdef DEBUG
printf("string: %s :: %s\n", name, ptr->value);
#endif
	    return ptr->value;
	}
	ptr = ptr->next;
    }
    return NULL;
}

void mark_arg(char *name, int flag){
//if flag = 0 -> global args are parsed, if 1 -> local args_ptr are parsed.
    struct ARGS *ptr;

    if(!name) return;
    if(!flag) ptr = args_ptr_global;
    else ptr = args_ptr_local;	//must be preseted befor execution of this function.
    while(ptr && ptr->name && ptr->value){
	if((ptr->marked == 0) && (strcmp(name, ptr->name) == 0)){
#ifdef DEBUG
printf("mark: %s :: %s\n", name, ptr->value);
#endif
	    ptr->marked = 1;//mark this argument
	    return;
	}
	ptr = ptr->next;
    }
    return;
}

int handle_arg(int flag, char *input)
{
    struct ARGS **ptr;
    char *name, *tmp;
    long long size;

    if(!input) return 0;	//nothing to parse
    if(!flag) ptr = &args_ptr_global;
    else ptr = &args_ptr_local;	//must be preseted befor execution of this function.
    while(*ptr) ptr=&((*ptr)->next);

    while (1){	//parse only booth '=' and '&' chars.
	if(((name = w_strtok(&input, '=')) == NULL) || ((tmp = w_strtok(&input, '&')) == NULL )){
	    break;
	}
	if(! *ptr) *ptr = (struct ARGS *)malloc(sizeof(struct ARGS));
	if(*ptr == NULL){
	    printf("ERR: Allocate memory\n");
	    return 0;
	}
	(*ptr)->next = NULL;
	(*ptr)->value = tmp;
	(*ptr)->marked = 0;	//default value for mark_arg
	(*ptr)->name = name;
//	(*ptr)->size = input - tmp; //size is included \0 char!!
//	httpd_decode(tmp);
	(*ptr)->size = httpd_decode(tmp) - tmp + 1; //size is included \0 char!!
	
//**********
	tmp = parsestr1_(tmp, "??/[/*/]??/");	//if (args+i)->value == "file.inc?par=??_#par1??"
	if(tmp){						//if not find - erease par=??_#par1?? complete
	    tmp = get_var(&size, tmp);				//only writeable (!= 0) parameters will be linked!
	    if(tmp && size) {
		(*ptr)->size = size;
		(*ptr)->value = tmp;
		//(args+i)->flag = 0;
	    } else {
//		(*ptr)->size = 0;//not found
//		free(*ptr);
		continue;
	    }
	}//(args+i)->flag = 1;
//**********
#ifdef DEBUG
//    printf("args[].name: %s args.value(after): %s, length=%d\n",(*ptr)->name,(*ptr)->value,strlen((*ptr)->value)+1);
    printf("args[].name: %s args.value length=%ld\n",(*ptr)->name, (*ptr)->size);
#endif
	ptr = &((*ptr)->next);
    }
    return 1;
}


void free_arg_1(struct ARGS **ptr){
    if(!ptr || !*ptr) return;
    free_arg_1(&((*ptr)->next));
#ifdef DEBUG
    printf("free arg '%s'\n", (*ptr)->name);
#endif
    free(*ptr);	//at hier action
    *ptr = NULL;
}

void free_arg(int flag){
    if(!flag){
    free_arg_1(&args_ptr_global);
    }else{
    free_arg_1(&args_ptr_local);
    }
}


//parm="parm1:parm2:parm3..."
//tbl1:tbl2:tbl3..
//if tbl1==name_of_html_argument && val_of_html_argument==rnd_number_of_table_entry
void fill_tbl(char *parm){

    struct rnd_tbl *ptr, *p;
    struct ARGS *p_args;
    char *tmp, *point, *name;
    unsigned int part=0;

    while(tmp = w_strtok(&parm, ':')){
	if(*tmp){
	    name = w_strtok(&tmp, '#');
	    if(name && *name){
		if(!*tmp) part = 0;
		else part = atoi(tmp);

		if(ptr = find_tbl(name)){
		    point = malloc(strlen(name) + 10);
		    if(point == NULL) continue;
		    sprintf(point, "%s_%d", name, part);

		    p = ptr;
		    while(p){
			p->flag = 1; //hide all elements!
			p = p->next;
		    }

		    while(ptr){
		    p_args = args_ptr_global;
			while(p_args && p_args->name && p_args->value){
			    if((strcmp(point, p_args->name) == 0) && (strcmp(ptr->rnd_entry, p_args->value) == 0)){
				ptr->flag = 0;
				break;//have found, break
			    }
			    p_args = p_args->next;
			}
		    ptr = ptr->next;
		    }

		    free(point);
		}
	    }
	}
    }
}

/* Given a www-form encoded string, restore the original: */
char *httpd_decode(char *string)		/*had name unescape*/
{
    char *src = string, *dest = string;
    int digit1, digit2;

    while (*src!='\0'){
	switch (*src){
	    case '+':
		*dest = ' ';
		break;
	    case '%':
		src ++;
		if (*src >= '0' && *src <= '9') digit1 = *src - '0';
		else if (*src >= 'A' && *src <= 'F') digit1 = (*src - 'A') + 0xA;
		else if (*src >= 'a' && *src <= 'f') digit1 = (*src - 'a') + 0xA;
		else digit1 = 0;
		src ++;
		if (*src >= '0' && *src <= '9') digit2 = *src - '0';
		else if (*src >= 'A' && *src <= 'F') digit2 = (*src - 'A') + 0xA;
		else if (*src >= 'a' && *src <= 'f') digit2 = (*src - 'a') + 0xA;
		else digit2 = 0;
		*dest = (digit1 * 0x10) + digit2;
		break;
	    default:
		*dest = *src;
	}
	src ++;
	dest ++;
    }
    *dest = '\0';
    return dest;	//needed for getting the end of string
}


/*find in *str character mark and replace them throw /0, set *str to next charakter and return pointer to begining of *str
 if charakter in string not found return NULL*/
char *w_strtok(char **str, char mark)
{

    char *begin = *str;

    if(*str)
    if(**str){
	while (**str != '\0'){
	    if (**str == mark){
		**str='\0';
		(*str)++;
		return begin;
	    }
	    (*str)++;
	}
    return begin;
    }
    return NULL;
}
/*
int check_digit( char *string )	//NEED TO BE USED!!!
{
    int i;

    if(!string) return 0;
    for (i=0; i<strlen(string); i++){
	if (isdigit( string[i] ))  return 0;
    }
    return 1;
}
*/
void delete_crlf( char *plag )	//find \n or \r and make end of string at this place!
{
    while ( *plag != '\0'){
	if ((*plag == '\n') || (*plag == '\r')){
	    *plag = '\0';
	    return;			
	}
	plag++;
    }
}

int DoCGI(FILE *out, char *filename, int flag){//if flag == 0 -with mime, == 1 - without mime

    CGI_ENTRY *p = cgi_entries;

#ifdef DEBUG
    printf("filename: %s\n",filename);
#endif

    while((p->name[0] != ' ') && (p->name[0] != '\0')){
	if(strcmp(p->name, filename) == 0){
printf("%s %p    %d\n",p->name, p->handler, flag);
	    if(*filename != '_' && flag != 1) fprintf(out, print200ok_mime, "text/html");
	    p->handler(out);
	    return 0;
	}
	p++;
    }
    if(get_cgi(out, filename, flag)) return 0; //from copy_cgi.c
/*    if(mime){ 
	    fprintf(out, "HTTP/1.0 404 Not Found\n"
		 "Content-type: text/html\n\n"
		 "<html>\n"
		 "<head><title>404 Not Found</title></head>\n"
		 "<body><h1>404 Not Found</h1>\n"
		 "<p>The resource you have requested is not available."
		 "</p></body>\n"
		 "</html>\n");
	}else 
	if(!mime) fprintf(out, "Not Found: %s\n", filename);*/
    return 1;//not found
}
/*
<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>
<BODY><H2>404 Not Found</H2>
The requested object does not exist on this server.
The link you followed is either outdated, inaccurate,
or the server has been instructed not to let you have it.
</BODY></HTML>
*/

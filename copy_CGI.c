/* copy_CGI.c:
 *
 * Copyright (C) 2010-2014  Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
//#include <errno.h>//errno
#include <string.h>
#include <sys/stat.h>	//chmod
#include "copy_CGI.h"
#include "copy_tbl.h"
#include "parse_CGI.h"	//w_strtok, cfg_arg_strcmp, cfg_arg_changed
#include "copy.h"
#include "parser.h"
#include "include/httpd.h"
#include "include/httpd_config.h"	//config is hier


#define MIN(a,b) ((a) < (b) ? (a) : (b))
extern config CONFIG;

struct cgi *cgi_name = NULL;
struct cgi *cgi_used = NULL;	//connect to cgi that is now in use. Used by print()

/*used in get_cgi as print "text to \n be ??_%var?? "*/
void print(FILE *out, char *tmp){
    char *tmp1, *tmp2;//tmp2 can be replaced via tmp1!!
    struct cgi *ptr;
    struct parsestr strct;
    ptr = cgi_used;
    static int loop_print = 0;	// used if loops exissted
    if(loop_print > 10){
	fprintf(out, "max loop counter reached\n");
	return;
    }
    loop_print++;

	    while(*tmp){
		tmp1 = tmp;
		if(*tmp == '\\'){
		    if(*(tmp+1) == '\"'){ tmp++; tmp1 = tmp;}
		    else if(*(tmp+1) == 'n'){tmp++; tmp1 = "\n";}
		    else if(*(tmp+1) == 't'){tmp++; tmp1 = "\t";}
		    else if(*(tmp+1) == '\\'){tmp++; tmp1 = "\\";}
		    else if(*(tmp+1) == '?' || *(tmp+1) == '{' || *(tmp+1) == '['){tmp++; tmp1 = tmp;}
		} else
		if(tmp2 = parsestr2(&strct, tmp, "??/[/N?N/*/]??")){		//??variable??
			tmp2 = get_var(NULL, tmp2);		//get_var and get_variable
			if(tmp2) print(out, tmp2); /*fprintf(out, "%s", tmp3);*/
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, tmp, "?@/[/N@N/*/]@?")){		//?@variable@?
			if(*tmp2 == '-')		//?@-_Table@?	- list
							//?@-Table@?	- <select>
				show_tbl(tmp2+1, out);
			else if(*tmp == '0'){		//?@0variable@?	-	fill with \0 the rest of variable
				long long size, s;
				tmp2 = get_var(&size, tmp2+1);
				if(tmp2 && (s = strlen(tmp2)) < size){
				    fprintf(out, "%s", tmp2);
				    while(s < size-1){
					putc('\0', out);
					s++;
				    }
				}
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, tmp, "{?/[/N?N/*/]?}")){	//{?condition?}	-switched on if running from cgi-script
//needed more thinking about
			if(ptr && ptr->data_ptr){
				tmp2 = parsestr1(ptr->data_ptr, tmp2);		//parse tmp in data-buffer
				if(tmp2){
				    fprintf(out, "%s", tmp2);
				    ptr->data_ptr = point[1];
				}//else ptr->data_ptr = NULL;		//be carefull- this mean that no more parses 
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, tmp, "[?/[/N?N/*/]?]")){	//[?condition?]	-switched on if running from cgi-script
//needed more thinking about
			if(ptr && ptr->data_ptr){
				tmp2 = parsestr1(ptr->data_ptr, tmp2);		//parse tmp in data-buffer
				if(!tmp2){
				    restore_str(&strct);
				    loop_print--;
				    return;		//if tmp is not matched with data_ptr - break printing
				}
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		}
		putc(*tmp1, out);
	    tmp++;
	    }
	loop_print--;
}

unsigned long long strncpy_(char *tmp, char *tmp1, long long size){
    //tmp -is out, tmp1 -is in. and return the size of string!
    //if tmp == NULL -> works as counter of string tmp1
    char ch, *tmp2, *tmp3;
    unsigned long long s = 0;
    int i;

    static int loop_strncpy = 0;	// used if loops exissted
    static char *ptr = NULL;//used for breaking loops like: "area:text to loop ??area??"

    if(loop_strncpy == 0){
	ptr = tmp;
    } else if(loop_strncpy > 10){
	printf("max loop counter in strncpy_ reached\n");
	goto end;
    }
    
    if(tmp1 == ptr){
	goto end;
    }
    loop_strncpy++;

    while ((!tmp || s < size) && *tmp1){
	ch = *tmp1;
	if(ch == '\\'){
	    tmp1++;
	    switch (*tmp1){
		case '\"':	ch = '\"';break;
		case '\\':	ch = '\\';break;
		case 'n':	ch = '\n';break;
		case 't':	ch = '\t';break;
		case '?':	ch = '?';break;
		default :	tmp1--;
	    }
	}else if(*tmp1 == '?' && *(tmp1+1) == '?' && *(tmp1+2) != '?'){		//??variable??
		    i = 0;
		    tmp1 = tmp1 + 2;
		    tmp2 = tmp1;
		    while(*tmp2){
			if(*tmp2 == '?' && *(tmp2+1) == '?'){
			    *tmp2 = '\0';
			    i = 1;
			    tmp3 = get_var(NULL, tmp1);		//get_var and get_variable
			    if(tmp3) s += strncpy_(tmp ? (tmp+s) : NULL, tmp3, size-s);
			    *tmp2 = '?';
			    tmp1=tmp2+2;
			    break;
			}
			tmp2++;
		    }
		    if(i == 0) printf("[ERR]: must be ??variable??\n");
		    continue;		//it's or tmp1=tmp1+2 or tmp1=tmp2+2
	}
	if(tmp) tmp[s] = ch;
	s++; tmp1++;
	if(s == 0) break;//is overload!
    }

    loop_strncpy--;
end:
    if(tmp) tmp[s] = '\0';
    return s;
}

//copy tmp1 to tmp and break if strlen of tmp1 is smoller as size
//size is including end of str.
void strmycpy(char *tmp, char *tmp1, long long size){
	if(size) size--;
	while(size){
		*tmp = *tmp1;
		if(!(*tmp1)) return;
		tmp++; tmp1++; size--;
	}
	*tmp = '\0';
}

//line="par:from_par:parsestr"
void write_ppar(char *line){
	char *par[2], *tmp, *tmp1;
	int i = 1;
	unsigned long long k, size;

	while(i >= 0){
		tmp = w_strtok(&line, ':');
		if(!tmp || !(*tmp) || !(*line)) {printf("[ERR]: write_ppar step: %d\n", i); return;}
//printf("%s , %d\n", tmp, i);
		tmp = get_var((i==1)? &size : NULL, tmp);
		if(!tmp || !size) {printf("[ERR]: parameter not found\n"); return;}
		par[i] = tmp;
	i--;
	}
// par[1]=to_par, par[0]=from_par, line=parsestr
	k = strlen(par[0])+1;
	tmp = (char *)malloc(k);
	if(tmp){
		strncpy(tmp, par[0], k);
		if(tmp1 = parsestr1(tmp, line)){
			strmycpy(par[1], tmp1, size);
		}
		free(tmp);
	}
}

void my_shell(FILE *out, char *tmp){		//tmp- is max. 2048byte  if runned from CGI-script
    FILE *fp;
    char *file_name = "/var/temp1234";
	if((fp = fopen(file_name,"w+")) != NULL){
	    print(fp, tmp);
	    fclose(fp);
	    chmod(file_name, S_IXUSR);	//make file Exec.by.User
	    my_system(out, file_name);
	    remove(file_name);
	}
    
}

void shell(char *tmp){		//tmp- is max. 2048byte  if runned from CGI-script
    FILE *fp;
    char *file_name = "/var/temp1234";
	if((fp = fopen(file_name,"w+")) != NULL){
	    print(fp, tmp);
	    fclose(fp);
	    chmod(file_name, S_IXUSR);
	    system(file_name);
	    remove(file_name);
	}
    
}
void rename_(char *old_name, char *new_name){
//    char LineBuf[256];
    FILE *fp, *fop;
	if((fp = fopen(old_name,"r")) != NULL && (fop = fopen(new_name, "w")) != NULL){
/*	    while(fgets(LineBuf,255,fp) != NULL){
	        fputs(LineBuf,fop);
	    }
*/	    copy(fp, fop);
	    fclose(fp);
	    fclose(fop);
	    remove(old_name);
	}
}

void change_line(char *line){ //line="/etc/file:/ nameserver: nameserver ??var??" or line="/etc/file:/*/?var?/: nameserver ??_%var??"
    char *file_name, *cmpstr, LineBuf[256];//, *tmp;
    char *file ="/var/temp1234";
    FILE *fp, *fop;
    int flag = 0;
        file_name = w_strtok(&line, ':');
        if(!file_name || !(*file_name)){ printf("change_line: file name is empty\n");return;}
        cmpstr = w_strtok(&line, ':');
        if(!cmpstr || !(*cmpstr)){ printf("change_line: compare_string is empty\n");return;}
	if(!(*line)){ printf("change_line: change_string is empty\n");return;}

/*	tmp = cmpstr;
	if(*tmp == '%'){
		tmp++;
		tmp = get_var(NULL, tmp);
		if(!tmp){ printf("change_line: get_var is empty\n");return;}
		
	}
*/	
	if((fp = fopen(file_name,"r")) != NULL && (fop = fopen(file, "w")) != NULL){
	    while(fgets(LineBuf,255,fp) != NULL){
	
	//if(strstr(LineBuf, tmp)){
		if(parsestr1(LineBuf, cmpstr)){
//printf("%s   :: %sline: %s\n", cmpstr, LineBuf, line);
		    flag = 1;
		    print(fop, line);
		}else{
		    fputs(LineBuf,fop);
		}
	    }
	    fclose(fp);
	    if(flag == 0) print(fop, line);
	    fclose(fop);
	    rename_(file, file_name);
	}
	*(cmpstr-1) = ':';
	*(line-1) = ':';

}

//chars = ab\"fr\\;<>
//if sw=1->show, sw=0->remove chars
void httpd_filter(char *src, char *chars, int sw){
    char tmp, *ptr, *dest = src;

    if(src && chars){
	while(*src){
	    ptr = chars;
	    while(*ptr){
		tmp = *ptr;
		if(*ptr == '\\'){
		    ptr++;
		    switch(*ptr){
			case '\"': tmp = '\"'; break;//chars have \"
			case 'n': tmp = '\n'; break;//	\n
			case 't': tmp = '\t'; break;
			case 'r': tmp = '\r'; break;
			case '\\': tmp = '\\'; break;//	\\
			default:
		    }
		}
		if(*src == tmp){
		    break;
		}
		ptr++;
	    }
	    if(((sw == 1) && *ptr)||((sw == 0) && !*ptr)) {*dest = *src; dest++;}
	    src++;
	}
	*dest = '\0';
    }
}

//parm="chars::parm1:parm2:parm3..."
void remove_show_chars(char *parm, int sw){

    char *tmp, *tmp2 = NULL, *chars = parm;
    long long size;

    while(*parm){
	if(parm[0] == ':' && parm[1] == ':' && parm[2] != ':'){
	    tmp2 = parm;
	    parm[0] = '\0';
	    parm+=2;
	    break;
	}
    parm++;
    }

    if(tmp2){
     while(tmp = w_strtok(&parm, ':')){
	if(*tmp){
	    tmp = get_var(&size, tmp);
	    if(tmp && size){
		httpd_filter(tmp, chars, sw);
	    }

	if(tmp != parm) *(parm-1) = ':'; //not last parm
	}
     }
    tmp2[0] = ':';
    }
}

//name:size_limit:format:file_name
int save_bfile_(FILE *out, char *parm){
    int i = 0, k = 0, ret_val = 1;
    char *tmp[4];

    while(i < 4){
	tmp[i] = w_strtok(&parm, ':');
	if(tmp[i] == NULL || (i != 2 && *tmp[i] == '\0')) k = 1;
	i++;
    }
    if(k == 1){
	fprintf(out, "not all args are given\n");
	return ret_val;
    }
    ret_val = save_bfile(out, tmp[0], atoll(tmp[1]), tmp[2], tmp[3]);

    *(tmp[1]-1) = ':';
    *(tmp[2]-1) = ':';
    *(tmp[3]-1) = ':';
    return ret_val;
}

#define skipspaces(p) while(isspace(*p)) p++;

int set_2(char *arg){
    int result=0;
    skipspaces(arg);
    if(isdigit(*arg)){
	while(isdigit(*arg))
	    result=result*10 + *arg++ - '0';
	return result;
    }
    else return 0;
}

int set_1(int *var, char *arg){
    skipspaces(arg);
    if(*arg == '\0') return 0;
    else if(*arg == '=' && *(arg+1) == '=') return *var == set_2(arg+2);
    else if(*arg == '!' && *(arg+1) == '=') return *var != set_2(arg+2);
    else if(*arg == '<' && *(arg+1) == '=') return *var <= set_2(arg+2);
    else if(*arg == '>' && *(arg+1) == '=') return *var >= set_2(arg+2);
    else if(*arg == '<') return *var < set_2(arg+1);
    else if(*arg == '>') return *var > set_2(arg+1);
    else if(*arg == '-' && *(arg+1) == '=') return *var -= set_2(arg+2);
    else if(*arg == '+' && *(arg+1) == '=') return *var += set_2(arg+2);
    else if(*arg == '='){ *var = set_2(++arg); return 1;}
    else if(*arg == ':' && *(arg+1) == '='){
	    char *tmp;		// set "a := _#val";
	    arg=arg+2;
	    skipspaces(arg); tmp=get_var(NULL, arg);
	    if(tmp){*var = set_2(tmp); return 1;}
	    else {*var = 0; return 0;}
	    }
    else return(*var);
}

int set(struct cgi *ptr, char *arg){
    skipspaces(arg);
    if(*arg == '\0') return 0;
    else if(*arg == 'a') return set_1(&(ptr->a), ++arg);
    else if(*arg == 'b') return set_1(&(ptr->b), ++arg);
    else if(*arg == 'c') return set_1(&(ptr->c), ++arg);
    else if(*arg == 'd') return set_1(&(ptr->d), ++arg);
    else return 0;
}

char *search[] = {"print",		//1
			"system",
			"my_system",	//"expl1" befor "expl"
			"get_file",
			"save_file",	//5
			"savecfg",
			"init",
			"fill_all_cfg",
			"fill_cfg",	//9
			"get_var",	//10
			"boot_page",
			"save_bfile",	//12
			"set",		//13
			"if_set",		//14 - the same as set!!
			"change_line",		//15
			"if_changed",		//16
			"not_changed",		//17
			"if",		//18
			"not",		//19
			"shell",	//20
			"my_shell",	//21
			"clean_ip",	//22
			"buf_if_eof",	//23
			"buf_parse_area",	//24
			"write_file",	//25
			"write_par",	//26
			"write_system",	//27
			"cat_system",	//28
			"bind_par",	//29
			"remove_chars",	//30
			"show_chars",	//31
			"load_file",	//32
			"exit_cgi",	//33
			"chroot",	//34
			"exist_file",	//35
			"nnot",		//36
			"1234test",	//37
			"chtbl_stat",	//38
			"write_ppar",	//39
			NULL
			};

char *cgi_loop(char *data, int *i, struct cgi *ptr){
    int j, len;
    char *tmp;


    while(data && *data){
	if(*data == '/'){
	    data++;
	    if(*data == '/') {do{ data++; if(*data == '\n'){ data+=1; break;}} while(*data); continue;}	//comment //...
	    if(*data == '*') {do{ data++; if(*data == '*'&& *(data+1) == '/'){ data+=2; break;}} while(*data); continue;}	//comment /*...*/
	    data--;
	}
	if(*data == ' ' || *data == '\t' || *data == '\n' || *data == '\r') {data++;continue;}

	j = 0;
	while(search[j]){
	    len = strlen(search[j]);
	    if(!strncmp(data, search[j], len)){
		data = data + len;
		ptr->cmd[*i] = j + 1;
//printf("cmd: %s:%d, len %d\n", search[j], *i, len);
		strncpy(ptr->arg[*i], "", 1);
		break;//see data++;
	    }
	    j++;
	}

	if(*data == '\"') {data++;
			    tmp = data;
			    while(*tmp){
				if(*tmp == '\"' && *(tmp-1) != '\\'){// && *(tmp-1) != '/'){	//neither \" nor /"
				    *tmp = '\0';
				    tmp++;
				    strncpy(ptr->arg[*i], data, MIN(tmp-data, GET_CGI_LEN));
//printf("arg: %s", ptr->arg[*i]);
				    break;
				}
			    tmp++;
			    }
	    data = tmp;
	    continue;
	}	//string
	if(*data == ';') {data++; (*i)++;if(*i >= GET_CGI_MAX) return NULL; ptr->cmd[*i] = 0; continue;}

	if(*data == 'd' && *(data+1) == 'o'){		// do{ }
	    j = 2; while(*(data+j) == ' ' || *(data+j) == '\t' || *(data+j) == '\n'){ j++; }
	    if(*(data+j) == '{') {data=data+j+1; j = *i; tmp = cgi_loop(data, i, ptr);
		if(tmp) ptr->bb[*i] = j; //tmp replace via data
//printf("bb: %d, to %d\n",*i, ptr->bb[*i]);
		data = tmp;
		continue;
	    }
	}
	
	if(*data == '{') {data++; j = *i; (*i)++; if(*i >= GET_CGI_MAX) return NULL; tmp = cgi_loop(data, i, ptr); //return NULL- is a problem!!
	    if(tmp) ptr->bb[j] = *i; //tmp replace via data
//printf("bb: %d, to %d\n",j, ptr->bb[j]);
	    data = tmp;
	    continue;
	}
	if(*data == '}'){data++; return data;
	}
#ifdef DEBUG
putchar(*data);
#endif
	data++;
    }
return data;
}

void parse_cgi_script(char *data){


    int i = 0;
    char *tmp;
    struct cgi **ptr;


    tmp = w_strtok(&data, '\n');
    if(!tmp){ printf("Unable to def. length\n"); return;}
    if(data - tmp > 30){ printf("Length of cgi name > 30\n"); return;}
    ptr = &cgi_name;
    while(*ptr){
	if((*ptr)->name && !strcmp((*ptr)->name, tmp)) return;	//if name exist - skip parsing!
	ptr = &((*ptr)->next);		//add cgi struct to the end
    }
    *ptr = (struct cgi *)malloc(sizeof(struct cgi));
    if(*ptr == NULL){
	printf("ERR: allocate memory\n");
	return;
    }
    strcpy((*ptr)->name, tmp);
    (*ptr)->next = NULL;
printf("script %s\n", (*ptr)->name);
//    printf("%s", data);

	while(i < GET_CGI_MAX){		//clear all cgi_lines
	    (*ptr)->cmd[i] = 0;
	    strncpy((*ptr)->arg[i], "", 1);
	    (*ptr)->bb[i] = i;
	    i++;
	}
	(*ptr)->data_ptr = NULL;
	i = 0;
	if(!cgi_loop(data, &i, *ptr)){	//if something wrong - erase entry of this cgi-script
	    free(*ptr);
	    *ptr = NULL;
	}
}

struct cgi *find_cgi(char *filename){
    struct cgi *ptr;
    ptr = cgi_name;
    while(ptr){
	if(!strcmp(ptr->name, filename)){
	    return ptr;
	}
	ptr = ptr->next;
    }
    return NULL;
}
/*
struct cgi *reverse_cgi(char *str){
    struct cgi *ptr;
    int i;
    ptr = cgi_name;
    while(ptr){
	i = 0;
	while(i < GET_CGI_MAX){
	    if(ptr->arg[i] == str){
		return ptr;
	    }
	i++;
	}
	ptr = ptr->next;
    }
    return NULL;
}
*/

int load_file(char *parm, FILE *out){
    
    parm = get_var(NULL, parm);
    if(!parm || !(*parm)){
	fprintf(out, "[ERR] Variable not found\n");
	return -1;
    }
    return (copy_file_include(parm, out));
}

extern char *print200ok_mime;

int get_cgi(FILE *out, char *filename){

    int i = 0, j = 0, jump = 0, allocated;
    long long size;
    struct cgi *ptr;
    char *tmp, *tmp1, *tmp2, *arg = NULL;
    FILE *f;

    ptr = cgi_name;
    while(ptr){
//printf("cgi: %s\n", ptr->name );
	if(!strcmp(ptr->name, filename)){
	    cgi_used = ptr;
	    if(*(ptr->name) != '_') fprintf(out, print200ok_mime, "text/html"); //script.cgi - with mime, _script.cgi - without
	    while(i < GET_CGI_MAX && ptr->cmd[i]){
//printf("[%d]: %s\n", i, ptr->arg[i]);
		arg = "";
		allocated = 0;
		if(ptr->arg[i]){	//allocate memory & copy ptr->arg[i] to arg
		    j = strlen(ptr->arg[i]) + 1;
		    arg = (char *)malloc(j);
		    if(arg){
			strncpy(arg, ptr->arg[i], j);
			allocated = 1;
		    }else{
			printf("[ERR] Unable allocate memory\n");
			cgi_used = NULL;
			return 0;
		    }
		} else {
		    printf("ptr->arg[i] is empty\n");
		    cgi_used = NULL;
		    return 0;
		}
		switch (ptr->cmd[i]){
		    case 1: //print
			    print(out, arg);
			     break;
		    case 2: system_(arg);break;
		    case 3: my_system(out, arg);break;
		    case 4: //get_file
			    if(copy_file_include(arg, out)) { jump = 1;}
			    break;
		    case 5: tmp1 = arg;			//save_file arg:file_name
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp = get_var(NULL, tmp2);//was get_arg
			    if(tmp)
			    if((f = fopen(tmp1,"w+")) == NULL)
				fprintf(out, "Unable to open file: %s", ptr->arg[i]);
			    else {
				fprintf(f, "%s", tmp);
/*				while(*tmp){
				    if(*tmp != 0x0d) putc(*tmp, f);	// '\r'
				    tmp++;
				}
*/
				fclose(f);
			    }
//			    if(tmp2 != tmp1) *(tmp1-1) = ':';
			    } break; //save_file
		    case 6: SaveConfiguration();break;		//savecfg (all changed flags will be zeroed)
		    case 7: ReadConfiguration1();break;		//init
		    case 8: fill_all_cfg();break;
		    case 9: fill_cfg(arg);break;
		    case 10: tmp = get_var(NULL, arg);
			    if(tmp) fprintf(out, "%s", tmp);/*maybe here print(out, tmp);*/ break;
		    case 11:	boot_page(out,arg);break;	//the same as: 
		/*	"<script language=\"JavaScript\">\n"
			"location.replace('http://??_srv_ip??:??_srv_port??/filename.htm');\n</script>*/
		    case 12: 
			    //save_bfile	name:size_limit:format:file_name
			    //name, file_name
			    if(save_bfile_(out, arg)) { jump = 1;}
			    break;
		    case 13: //set(ptr, ptr->arg[i]);		//set
			    //break;
		    case 14:					//if_set
			    if(!set(ptr, arg)){ jump = 1;}
			    break;
		    case 15:				//change_line
			    change_line(arg);
			    break;
		    case 16:				//if_changed
//printf("if_changed: %s; i=%d; bb=%d\n", ptr->arg[i], i, ptr->bb[i]);
			    if(!cfg_arg_changed(arg)){ jump = 1;}
			    break;
		    case 17:				//not_changed or not found
			    if(cfg_arg_changed(arg)) { jump = 1;}
			    break;
		    case 18:				//if
//printf("if: %s; i=%d; bb=%d\n", ptr->arg[i], i, ptr->bb[i]);
			    if(cfg_arg_strcmp(arg, 0)) { jump = 1;}
			    break;
		    case 19:				//not
			    if(cfg_arg_strcmp(arg, 1)) { jump = 1;}
			    break;
		    case 20:shell(arg);		//shell
			    break;
		    case 21:my_shell(out, arg);		//my_shell
			    break;
/* write to global variables */
		    case 22:     check_ip[0] = '\0';	//clean_ip
			    break;
/* buffer functions */
		    case 23:	//buf_if_eof - if not found in buffer
			if(ptr->data_ptr != NULL) { jump = 1;}
			    break;
		    case 24:	//buf_parse_area
			if(ptr->parse == NULL){
			    printf("buf_parse_area: Pointed to NULL\n");
			    if(allocated) free(arg);
			    cgi_used = NULL;
			    return 0;
			}
			tmp = parsestr1(ptr->parse, arg);
			if(tmp){
			    ptr->data_ptr = tmp;
			    ptr->parse = point[1];
			    break;
			} else {
			    ptr->data_ptr = ptr->parse;	//this is "if not matched" -  return for "print" begin of parse
			    if(i!=ptr->bb[i]) { jump = 1;}//jump
			    else {
				printf ("buf_parse_area: no jump braces!\n");
				if(allocated) free(arg);
				cgi_used = NULL;
				return 0;
			    }
			}
		    case 25:	// write_file  par:file_name
			    tmp1 = arg;
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp = get_var(&size, tmp2);
				if(tmp && size)
//printf("SIZE: %lld\n", size);
				    copy_file_buf(tmp1, tmp, size);
				if(tmp2 != tmp1) *(tmp1-1) = ':';
			    }
			    break;
		    case 26:	// write_par  par:value
			    tmp1 = arg;
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp = get_var(&size, tmp2);
				if(tmp && size){
				    strncpy_(tmp, tmp1, size);
				    tmp[size-1] = '\0';
				}
				if(tmp2 != tmp1) *(tmp1-1) = ':';
			    }
			    break;
		    case 27:	// write_system  par:cmd
		    case 28:	// cat_system  par:cmd
			    tmp1 = arg;
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp = get_var(&size, tmp2);
				if(tmp && size){
				if(ptr->cmd[i] == 27) write_system(tmp, size, 0, tmp1);
				else write_system(tmp, size, 1, tmp1);
				}
				if(tmp2 != tmp1) *(tmp1-1) = ':';
			    }
			    break;
		    case 29:	// bind_par  par
			    tmp = get_var(&size, arg);
			    if(tmp && size){
				ptr->data_ptr = tmp;
				ptr->parse = tmp;//used in 24 - buf_parse_area
			    }else{
				ptr->data_ptr = NULL;
				ptr->parse = NULL;
			    }
			    break;
		    case 30:		//remove_chars
			    remove_show_chars(arg, 0);
			    break;
		    case 31:		//show_chars
			    remove_show_chars(arg, 1);
			    break;
		    case 32: //load_file		danger!!
			    if(load_file(arg, out)) { jump = 1;}
			    else break;
		    case 33: //exit_cgi
			    if(allocated) free(arg);
			    return 1;
		    case 34: //chroot
			    if(*arg == '\0') tmp = CONFIG.WEB_ROOT; //if empty str -> use default value
			    else tmp = arg;

			    if(chdir(tmp)) { jump = 1;}
			    break;
		    case 35: //exist_file
			    tmp = get_var(NULL, arg);
			    if(tmp){
				if((f = fopen(tmp, "r"))==NULL){
				    jump = 1;	//jump
				}else fclose(f);
			    }else{
				fprintf(out, "Parameter: %s not found\n", arg);
				jump = 1;	//jump
			    }
			    break;
		    case 36:			//nnot		-not exist or not matches
			    if(!cfg_arg_strcmp(arg, 0)) { jump = 1;}
			    break;
		    case 37:
			    printf("%s\n", arg);		//1234test
/*			    if((f = fopen("/dev/random", "r"))==NULL){
				fprintf(out, "Unable open /dev/random\n");
			    }else {
			    j = 0;
			    while(j <= 6){
				size = fgetc(f);
//				if(size >= '0' && size <= '9' ) {
				    j++;
				    putc(size, out);
//				}
			    }
			    fclose(f);
			    }
			    size = random();
			    fprintf(out, "%ld", size);
*/			    break;
		    case 38:
			    change_tbl_stat(arg);	//arg="flag:name:frase(forParsing)"
			    break;
		    case 39://arg="par:from_par:parsestr"
			    write_ppar(arg);
			    break;
		}//switch end
	    if(allocated) free(arg);
//printf("check: [%d]: %s a=%d b=%d\n", i, ptr->arg[i], ptr->a, ptr->b);
	    if(jump == 1){
		jump = 0;
		if(i!=ptr->bb[i]) i = ptr->bb[i];
		else i++;
	    }else i++;
	    }//while
	    cgi_used = NULL;
	    return 1;	//ok cgi is found
	}
	ptr = ptr->next;
    }//while(ptr)
    return 0;
}

void free_cgi(struct cgi *ptr){
    if(!ptr) return;
    free_cgi(ptr->next);
printf("free %s\n", ptr->name);
    free(ptr);	//at hier action
}

void show_CGIs(FILE *out){ //used for analysis of cgi-scripts
    int i = 0;
    struct cgi *ptr;

    ptr = cgi_name;
    while(ptr){
	fprintf(out, "CGI: %s\n", ptr->name);
	while(i < GET_CGI_MAX && ptr->cmd[i]){
	    if(i == ptr->bb[i]) fprintf(out, "%d:\t\t%s\t\t%s\n", i, *(search + ptr->cmd[i] - 1), ptr->arg[i]);
	    else fprintf(out, "%d: -> %d\t\t%s\t\t%s\n", i, ptr->bb[i], *(search + ptr->cmd[i] - 1), ptr->arg[i]);
	    i++;
	}//while(i)
	i = 0;
	ptr = ptr->next;
    }//while(ptr)

}


/* copy_CGI.c:
 *
 * Copyright (C) 2010-2021  Alexander Reimer <alex_raw@rambler.ru>
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
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>	//chmod
#include <sys/wait.h>
#include <sys/types.h>
#include "copy_CGI.h"
#include "copy_tbl.h"
#include "parse_CGI.h"	//w_strtok, cfg_arg_strcmp, cfg_arg_changed
#include "copy.h"
#include "parser.h"
#include "include/httpd.h"
#include "include/httpd_config.h"	//config is hier


#define MIN(a,b) ((a) < (b) ? (a) : (b))
extern config CONFIG;
//for my_system
extern int fd;
extern int sockfd;

struct cgi *cgi_name = NULL;
struct cgi *cgi_used = NULL;	//connect to cgi that is now in use. Used by print()

//return 0 -exec in braces, 1 - jump
/*used in get_cgi as print "text to \n be ??_%var?? "*/
int print(FILE *out, char *tmp){
    char ch, *tmp1, *tmp2, *tmp3, *tmp4, pattern[48];
    struct cgi *ptr;
    struct parsestr strct;
    ptr = cgi_used;
    static int loop_print = 0;	// used if loops exissted
    if(loop_print > 10){
	fprintf(out, "max loop counter reached\n");
	return 1;
    }
    loop_print++;

	    while(*tmp){
		ch = *tmp;
		if(ch == '\\'){
		    tmp++;
		    switch (*tmp){
			case '\"':
			case '?':
			case '{':
			case '[':
			case '\\':	ch = *tmp;break;
			case 'n':	ch = '\n';break;
			case 't':	ch = '\t';break;
			default :	tmp--;
		    }
//		} else if(ch == '/' && *(tmp+1) == '/'){ tmp++; //new here!!!!!!!!!!!!!!!!!!!
		} else
		if(tmp1 = parsestr2(&strct, NULL, tmp, "??/[/N?N/*/]??")){		//??variable??
			tmp2 = w_strtok(&tmp1, ':');//??parameter:compstring:digits?? == ??test_par:/[hello/]something:10??
			if(tmp2){
			    tmp2 = get_var(NULL, tmp2);		//get_var and get_variable
			    if(tmp2){
				if(*tmp1){
				tmp3 = tmp1;
				while(*tmp3){
				    if(*tmp3 == '\\' && *(tmp3+1) == '\\') tmp3++; //tmp3=+2
				    else if(*tmp3 == '\\' && *(tmp3+1) == ':') tmp3++; //tmp3=+2
				    else if(*tmp3 == '/' && *(tmp3+1) == '/') tmp3++; //tmp3=+2
				    else if(*tmp3 == '/' && *(tmp3+1) == '\\') tmp3++; //tmp3=+2
				    else if(*tmp3 == '/' && *(tmp3+1) == ':') tmp3++; //tmp3=+2
				    else if(*tmp3 == ':') { *tmp3 = '\0'; tmp3++; break;} //tmp3=+2
				    tmp3++;
				}
				tmp4 = malloc(strlen(tmp2) + 2);//in tmp2 is a value of variable
				    if(tmp4){
					sprintf(tmp4, "%s", tmp2);//copy tmp2 to tmp4
					tmp2 = parsestr(tmp4, tmp1);
					if(tmp2){
						if(*tmp3 == '\0') tmp3 = "%s";
						else{
						    sprintf(pattern, "%%.%ds", atoi(tmp3));//pattern is max 48 chars
						    tmp3 = pattern;
printf("pattern:%s---", tmp3);
						}
					    fprintf(out, tmp3, tmp2);
					}
					free(tmp4);
				    } else printf("ERR: allocate memory\n");
				} else print(out, tmp2); /*fprintf(out, "%s", tmp3);*/
			    }
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "?@/[/N@N/*/]@?")){		//?@variable@?
			if(*tmp2 == '-')		//?@-_Table@?	- list
							//?@-Table@?	- <select>
				show_tbl(tmp2+1, out);
			else if(*tmp2 == '_')		//?@__Table@?	- <input type=checkbox>	- show all
							//?@_Table@?	- <input type=checkbox>	- show only showable
				show_tbl_chck(tmp2+1, out);
			else if(*tmp2 == '0'){		//?@0variable@?	-	fill with \0 the rest of variable
				long long size, s;
				tmp2 = get_var(&size, tmp2+1);
				if(tmp2){
				    s = strlen(tmp2);
				    if(s < size)	fprintf(out, "%s", tmp2);
				    else{		fwrite(tmp2, size-1, 1, out); s = size-1;}//make zero at end
				    while(s < size){
					putc('\0', out);
					s++;
				    }
				}
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "{?/[/N?N/*/]?}")){	//{?condition?}	-switched on if running from cgi-script
//needed more thinking about
			if(ptr && ptr->data_ptr){
				tmp2 = parsestr(ptr->data_ptr, tmp2);		//parse tmp in data-buffer
				if(tmp2){
				    fprintf(out, "%s", tmp2);
				    ptr->data_ptr = point[1];
				}//else ptr->data_ptr = NULL;		//be carefull- this mean that no more parses 
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "[?/[/N?N/*/]?]")){	//[?condition?]	-switched on if running from cgi-script
//needed more thinking about
			if(ptr && ptr->data_ptr){
				tmp2 = parsestr(ptr->data_ptr, tmp2);		//parse tmp in data-buffer
				if(!tmp2){
				    restore_str(&strct);
				    loop_print--;
				    return 0;		//if tmp is not matched with data_ptr - break printing
				}
			}
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "?TABLE:/ /[/*\n/]END_TABLE?\n")){	// ?TABLE:.....\nEND_TABLE?
			parse_tbl(tmp2, 1);
			tmp = restore_str(&strct);
			continue;
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "?table?:/B/sV:/\\/n0V/E/[/*/]/<1\n\\0/>")){	// ?table?:.....\n
			tabs(tmp2, value_, out);		//run table-ordered show of TABLE entries
			tmp = restore_str(&strct);
			continue;
		} else
		if(tmp2 = ticket_find(&tmp)){	//in tmp2 - ticket
			fprintf(out, "%s", tmp2);
			continue;
		}

		putc(ch, out);
	    tmp++;
	    }
	loop_print--;
	return 1;
}

int print_page(FILE *out, char *tmp){
    char ch, arg_s = 0, *tmp2;//tmp2 can be replaced via tmp1!!
    struct parsestr strct;

	    while(*tmp){
		ch = *tmp;
		if(ch == '\\'){
		    tmp++;
		    switch (*tmp){
			case '\"':
			case '?':
			case '{':
			case '[':
			case '\\':	ch = *tmp;break;
			case 'n':	ch = '\n';break;
			case 't':	ch = '\t';break;
			default :	tmp--;
		    }
		} else
		if(tmp2 = parsestr2(&strct, NULL, tmp, "??/[/N?N/*/]??")){		//??variable??
			tmp2 = get_var(NULL, tmp2);		//get_var and get_variable
			if(tmp2) print(out, tmp2); /*fprintf(out, "%s", tmp3);*/
			tmp = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		} else
		if(ch == '?') arg_s = 1;
		else
		if(arg_s == 0 && (tmp2 = ticket_find(&tmp)) != NULL){	
				//in tmp2 - ticket. make it before '?'. after don't get a ticket.
				// file?arg1=12&arg2=tffddf
			fprintf(out, "%s", tmp2);
			continue;
		}

		putc(ch, out);
	    tmp++;
	    }
	return 1;
}

unsigned long long strncpy_(char *tmp, char *tmp1, long long size){
    //tmp -is out, tmp1 -is in. and return the size of string!
    //if tmp == NULL -> works as counter of string tmp1
    //'\0' -sign is not counted!! -> strncpy_() + 1 
    char ch, *tmp2;
    struct parsestr strct;
    unsigned long long s = 0;

    static int loop_strncpy = 0;	// used if loops exissted
    static char *ptr = NULL;//used for breaking loops like: "area:text to loop ??area??"

    if(loop_strncpy == 0){
	ptr = tmp;
    } else if(loop_strncpy > 10){
	printf("max loop counter in strncpy_ reached\n");
	goto end;
    }

    if(tmp1 == ptr || tmp1 == NULL){
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
//	} else if(ch == '/' && *(tmp1+1) == '/'){ tmp1++; //new here!!!!!!!!!!!!!!!!!!!
	} else if(tmp2 = parsestr2_s(&strct, NULL, tmp1, "?@/[/N@N/*/]@?")){		//?@variable@?
		if(*tmp2 == '-')		//?@-_Table@?	- list
						//?@-Table@?	- <select> (not exist and not used)
			s += show_tbl_str(tmp2+1, tmp ? (tmp+s) : NULL, size-s);
/*		else if(*tmp2 == '0'){		//?@0variable@?	-	fill with \0 the rest of variable
				long long size1, s;
				tmp2 = get_var(&size1, tmp2+1);
				if(tmp2 && (s = strlen(tmp2)) < size1){
				    fprintf(out, "%s", tmp2);
				    while(s < size1-1){
					putc('\0', out);
					s++;
				    }
				}
		}
*/		tmp1 = restore_str(&strct);
		continue;
	} else if(tmp2 = parsestr2_s(&strct, NULL, tmp1, "??/[/N?N/*/]??")){		//??variable??
		tmp2 = get_var(NULL, tmp2);		//get_var and get_variable
		if(tmp2) s += strncpy_(tmp ? (tmp+s) : NULL, tmp2, size-s);
		tmp1 = restore_str(&strct);
		continue;
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
unsigned long long strmycpy(char *tmp, char *tmp1, unsigned long long size){
	if(size) size--;
	int s = size;
	while(size){
		*tmp = *tmp1;
		if(!(*tmp1)) return (s-size);
		tmp++; tmp1++; size--;
	}
	*tmp = '\0';
	return s;
}

//line="from_par:to_par:parsestr"
int write_ppar(char *line){
	char *par[2], *tmp, *tmp1;
	int i = 0, jump = 0;
	unsigned long long k, size;

	while(i <= 1){
		tmp = w_strtok(&line, ':');
		if(!tmp || !(*tmp) || !(*line)) {printf("[ERR]: write_ppar step: %d\n", i); return 0;}
//printf("%s , %d\n", tmp, i);
		par[i] = get_var((i==1)? &size : NULL, tmp);
	i++;
	}
	if(!par[0]) {printf("[ERR]: parameter not found\n"); return 0;}

// par[1]=to_par, par[0]=from_par, line=parsestr, size=to_par_size
	if(par[1]){
	    if(size){
		k = strlen(par[0])+1;
		//size == 1
		tmp = (char *)malloc(k);
		if(tmp){
		    strncpy(tmp, par[0], k);
		    if(tmp1 = parsestr(tmp, line)){
			strmycpy(par[1], tmp1, size);
			jump = 1;//everything is OK - so jump
		    }
		free(tmp);
		}
	    }
	}else{
	    //tmp[1] == NULL
	    if(tmp1 = parsestr(par[0], line)){	//par[0] - can be zeroed!! by /[/*/]
		jump = reg_par(tmp, tmp1, strlen(tmp1)+1);	//tmp = to_par, on success = 1
	    }
	}
	return jump;
}

//b_in(buffer) and s_in(size) are /dev/stdin. If (b_in == NULL) ->/dev/stdin is OFF!
//buf and size are std(out, err) buffers.
void write_system(char *b_in, long long s_in, char *buf, long long size, int mode, char *cmd)
{
	int pid, status;
	int pipe_fd[2], pipe_[2]; //pipe_fd - is for read(stdout stderr) and pipe_ - is for write(stdin)
	int wrote;
	int err = 0;
	long long i = 0;

if(!size) return;

if(mode){	//mode==1 -> cat mode
	while(buf[i] && (size - 1 - i)){
	    i++;
	}
}

    if (pipe(pipe_fd) == 0) {
      if (pipe(pipe_) == 0) {
	if (!(pid=fork())){	//child
	    close(pipe_fd[0]);	//read end

	    close(fd);	    	//for inetd
	    close(sockfd);

	    close(1);
	    close(2);
	    dup2(pipe_fd[1], 1);	//1 -stdout
	    dup2(pipe_fd[1], 2);	//2 -stderr
	    close(pipe_fd[1]);

	    close(pipe_[1]);	//write end
	    close(0);
	    dup2(pipe_[0], 0);	//1 -stdin
	    close(pipe_[0]);

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);

	    exit(1);
	}else{			//parent
	    close(pipe_[0]);	//read end
	    close(pipe_fd[1]);	//write end
	    if (pid > 0) {	// fork -- parent

		if(b_in != NULL){
		    if(s_in == 0) s_in = strlen(b_in);
		    if(s_in != 0) write(pipe_[1], b_in, s_in);
		}
		close(pipe_[1]);

		while(i<(size-1) && read(pipe_fd[0], (buf+i), 1) > 0){
//printf("n=%d w=%d",n,wrote);
//		    if(i >= (size-1)) break;
		    i++;
		}
		buf[i] = '\0';
		wait(&status);
	    } else {	//fork failed
		err = 2;
		close(pipe_[1]);
	    }

	    close(pipe_fd[0]);	//close rest
	}
      }else{
        err = 1;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
      }

    } else err = 1;	//make a pipe failed
    if(err)	fprintf(stderr, "my_system: Unable to run child process %s, errno %d\n", cmd, err);
}


void my_system(FILE *out, char *cmd)
{
	int pid, status;
	int pipe_fd[2];
	int wrote;
	int err = 0;
	int buf;
	
    if (pipe(pipe_fd) == 0) {

	if (!(pid=fork())){	//child
	    close(pipe_fd[0]);	//read end

	    close(fd);	    	//}for inetd
	    close(sockfd);

	    close(1);
	    close(2);
	    dup2(pipe_fd[1], 1);	//1 -stdout
	    dup2(pipe_fd[1], 2);	//2 -stderr
	    close(pipe_fd[1]);

	    close(0);	//	/dev/stdin is off

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);
	    exit(1);
	}else{			//parent
	    close(pipe_fd[1]);	//write end
	    if (pid > 0) {	// fork -- parent
		while(read(pipe_fd[0], &buf, 1) > 0){
		    wrote = fwrite(&buf, 1, 1, out);
//printf("n=%d w=%d",n,wrote);
		    if(wrote < 1){
			err = 3;	//write failed
			break;
		    }
		}
//		waitpid(pid, &status, 0);
		wait(&status);
	    } else	//fork failed
		err = 2;

	    close(pipe_fd[0]);	//close rest
	
	}

    } else err = 1;	//make a pipe failed
    if(err)	fprintf(out, "my_system: Unable to run child process %s, errno %d\n", cmd, err);
}

void system_(char *cmd)
{
	int pid, status;
	int err = 0;
	
	if (!(pid=fork())){	//child

	    close(fd);	    	//for inetd
	    close(sockfd);

//	    close(1);
//	    close(2);

	    close(0);	//	/dev/stdin is off

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);
	    exit(1);
	}else{			//parent
	    if (pid > 0) {	// fork -- parent
		wait(&status);
	    } else	//fork failed
		err = 1;

	}

    if(err)	fprintf(stderr, "system: Unable to run child process %s, errno %d\n", cmd, err);
}


void write_shell(char *buf_in, long long size_in, char *buf, long long size_1, int mode, char *cmd){ //tmp- is max. 2048byte  if runned from CGI-script
	unsigned long long size = strncpy_(NULL, cmd, 0);	//this is max. size of cmd-string
	char *arg;

	if(arg = malloc(size+1)){
	    strncpy_(arg, cmd, size);
	    write_system(buf_in, size_in, buf, size_1, mode, arg);
	    free(arg);
	}
/*
    FILE *fp;
    char *file_name = "/var/temp1234";
	if((fp = fopen(file_name,"w+")) != NULL){
	    print(fp, cmd);
	    fclose(fp);
	    chmod(file_name, S_IXUSR);
	    system(file_name);
	    write_system(buf, size, mode, file_name);
	    remove(file_name);
	}*/
}

void my_shell(FILE *out, char *tmp){		//tmp- is max. 2048byte  if runned from CGI-script
	unsigned long long size = strncpy_(NULL, tmp, 0);	//this is max. size of arg-string
	char *arg;

	if(arg = malloc(size+1)){
	    strncpy_(arg, tmp, size);
	    my_system(out, arg);
	    free(arg);
	}
}

void shell(char *tmp){		//tmp- is max. 2048byte  if runned from CGI-script
	unsigned long long size = strncpy_(NULL, tmp, 0);	//this is max. size of arg-string
	char *arg;

	if(arg = malloc(size+1)){
	    strncpy_(arg, tmp, size);
	    system(arg);
	    free(arg);
	}

}

void rename_(char *old_name, char *new_name){
//    char LineBuf[256];
    FILE *fp, *fop;
    char i = 0;
	if((fp = fopen(old_name,"r")) != NULL){
	    if((fop = fopen(new_name, "w")) != NULL){
/*	    while(fgets(LineBuf,255,fp) != NULL){
	        fputs(LineBuf,fop);
	    }
*/	    i = 1;
	    copy(fp, fop);
	    fclose(fop);
	    }
	    fclose(fp);
	    if(i) remove(old_name);
	}
}

//return 0 -exec in braces, 1 - jump
//if flg == 0 - match one time (change_line), == 1 - match all times (change_all_lines)
int change_line(char *line, char flg){ //line="/etc/file??name??:/ nameserver: nameserver ??var??" or line="/etc/file:/*/?var?/: nameserver ??_%var??"
    char *file_name, *cmpstr, LineBuf[256], *tmp;
    unsigned long long size;
    char *file ="/var/temp1234";
    char *ptr = cgi_used->data_ptr; //!!!!use only in cgi-scripts!!!!

    FILE *fp, *fop;
    int flag = 0;//exec in braces
	file_name = w_strtok(&line, ':');
	if(!file_name || !(*file_name)){ printf("change_line: file name is empty\n");return 1;}//jump the braces
	cmpstr = line;
	while(1){
	    if(!(*line)) { printf("change_line: no cmpstr:print\n");return 1;}
	    else if(*line == '\\' && *(line+1) == '\\') line++; //line=+2
	    else if(*line == '\\' && *(line+1) == ':') line++; //line=+2
	    else if(*line == '/' && *(line+1) == '/') line++; //line=+2		//this is for /(/:/)
	    else if(*line == '/' && *(line+1) == '\\') line++; //line=+2
	    else if(*line == '/' && *(line+1) == ':') line++; //line=+2
	    else if(*line == ':') {*line = '\0'; line++; break;}
	    line++;
	}
	if(!(*cmpstr)){ printf("change_line: compare_string is empty (double file::print)\n");return 1;}

    size = strncpy_(NULL, file_name, 0);	//this is max. size of arg-string
    if(size && (tmp = malloc(size+1))){
	strncpy_(tmp, file_name, size);

	if((fp = fopen(tmp,"r")) != NULL){
	    if((fop = fopen(file, "w")) != NULL){
	    while(fgets(LineBuf,255,fp) != NULL){

		cgi_used->data_ptr = LineBuf;
		if((flg || !flag) && parsestr(LineBuf, cmpstr)){
		    if(print(fop, line)){
//printf("%s   :: %sline: %s\n", cmpstr, LineBuf, line);
		    flag = 1;//jump braces
		    }else{
			fputs(LineBuf,fop);
		    }
		}else{
		    fputs(LineBuf,fop);
		}
	    }
	    fclose(fop);
		if(flag == 1){
		    rename_(file, tmp);//found and changed - rename file
		}else{
		    remove(file);//not found - remove file
		}
	    }
	fclose(fp);
	}
	cgi_used->data_ptr = ptr;
	free(tmp);
    }
	return flag;
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

extern int method1;
extern char *postdata;
extern unsigned long content_length;

/*search needle in haystack and return the end of needle
    i - is offset from haystack to needle pointer
*/
void *memmem_in(char *haystack, long haystack_len, char *needle, unsigned long long *i)	/*other places witch use this function must be changed to end of needle*/
{
    char *last_possible;
    int needle_len;
    
    needle_len = strlen(needle);// only one string is possible

    *i = 0;

    if (needle_len == 0)
	/* The first occurrence of the empty string is deemed to occur at
	   the beginning of the string.	 */
	return (void *) haystack;

    last_possible = (char *) haystack + haystack_len - needle_len;

    for ( ;haystack <= last_possible; ++haystack)
    {
	if (*haystack == *needle &&
	    !memcmp((void *) (haystack + 1),(void *) (needle + 1), needle_len - 1))
	{
		return (void *) (haystack + needle_len);
	}
	*i += 1;
    }

    *i = 0;
    return NULL;
}

//name:size_limit:firmware:file_name
int save_bfile_1(FILE *out, char *form_name, unsigned long long size_limit, char *firmware, char *file_name){

    int jump = 0;
    unsigned long long i = 0, b_len, t_len, size;
    char bound[1060], *var_name = NULL, *file, *tmps;

    if(method1 != POST_BOUNDARY || !postdata) return 0;

printf("content_length = %ld\n",content_length);

    var_name = (char *) malloc(strlen(form_name) + 60);
    if(!var_name) return 0;

    sprintf(var_name, "Content-Disposition: form-data; name=\"%s\"; filename=\"", form_name);
    file = (char *) memmem_in(postdata, content_length, var_name, &i);
    free(var_name);

    if(file && (*file != '\"')){
	    tmps = strchr(file, '\"');
	    if(tmps == NULL){
		fprintf(out, "\" - not found\n");
		return 0;
	    }
	    *tmps = '\0';

	    size = tmps - file;	//size of file_name
	    b_len = content_length - (tmps - postdata);	   // from tmps length, i is the offset

	    tmps = (char *) memmem_in(tmps, b_len, "\r\n\r\n", &i);	// find data starting point

	    if(i  && (b_len - i)){
		b_len = b_len - i - 4;

		sprintf(bound, "\r\n--%s", boundary);//0x0d,0x0a
		memmem_in(tmps, b_len, bound, &t_len);
		if(t_len > 0){// found end boundary, t_len is the offset to boundary from tmps
		    if(size_limit && t_len > size_limit){
			fprintf(out, "File size:%lld > Size limit: %lld\n", t_len, size_limit);
			return 0;
		    }
		    jump = reg_par(file_name, file, size);
		    if(jump){
			jump = reg_par(firmware, tmps, t_len);
		    }
		}
	    }
	}
return jump;
}

//name:size_limit:firmware:file_name
int save_bfile_(FILE *out, char *parm){
    int i = 0, ret_val = 0;
    char *tmp[4];

    while(i < 4){
	tmp[i] = w_strtok(&parm, ':');
	if(tmp[i] == NULL){
	    fprintf(out, "not all args are given\n");
	    return ret_val;
	}
	i++;
    }
    ret_val = save_bfile_1(out, tmp[0], atoll(tmp[1]), tmp[2], tmp[3]);

//    *(tmp[1]-1) = ':';
//    *(tmp[2]-1) = ':';
//    *(tmp[3]-1) = ':';
    return ret_val;
}

// tmp1 = "from_par:to_par:cmd"
//or ":to_par:cmd"
void wr_shell(char *tmp1, char flag){
    char *tmp, *tmp2, *tmp3;
    unsigned long long size, size1;
	tmp = w_strtok(&tmp1, ':');
	if(tmp){
		tmp3 = tmp1;
		if(*tmp)  tmp = get_var(&size1, tmp);
		else tmp = NULL;
		tmp2 = w_strtok(&tmp1, ':');
		if(tmp2 && *tmp2 && *tmp1){
		    tmp2 = get_var(&size, tmp2);
		    if(tmp2 && size){
		    write_shell(tmp, size1, tmp2, size, flag, tmp1);
//			else write_shell(tmp, size, 1, tmp1);
		    }
		if(tmp2 != tmp1) *(tmp1-1) = ':';
		}
		*(tmp3-1) = ':';
	}
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


//these functions used in set_tbl()
unsigned long long set_2_lng(char *arg){
    unsigned long long result=0;
    skipspaces(arg);
    if(isdigit(*arg)){
	while(isdigit(*arg))
	    result=result*10 + *arg++ - '0';
	return result;
    }
    else return 0;
}

int set_1_lng(unsigned long long *var, char *arg){
    skipspaces(arg);
    if(*arg == '\0') return 0;
    else if(*arg == '=' && *(arg+1) == '=') return *var == set_2_lng(arg+2);
    else if(*arg == '!' && *(arg+1) == '=') return *var != set_2_lng(arg+2);
    else if(*arg == '<' && *(arg+1) == '=') return *var <= set_2_lng(arg+2);
    else if(*arg == '>' && *(arg+1) == '=') return *var >= set_2_lng(arg+2);
    else if(*arg == '<') return *var < set_2_lng(arg+1);
    else if(*arg == '>') return *var > set_2_lng(arg+1);
    else if(*arg == '-' && *(arg+1) == '=') return *var -= set_2_lng(arg+2);
    else if(*arg == '+' && *(arg+1) == '=') return *var += set_2_lng(arg+2);
    else if(*arg == '='){ *var = set_2_lng(++arg); return 1;}
    else if(*arg == ':' && *(arg+1) == '='){
	    char *tmp;		// set "a := _#val";
	    arg=arg+2;
	    skipspaces(arg); tmp=get_var(NULL, arg);
	    if(tmp){*var = set_2_lng(tmp); return 1;}
	    else {*var = 0; return 0;}
	    }
    else return(*var);
}

int set_tbl(char *arg){
    char *tmp, ch = '\0', *st, *string = " =!<>-+:";
    unsigned long long *in = NULL;

    skipspaces(arg);
    if(*arg == '\0') return 0;
    tmp = arg;
    while(*tmp){
	st = string;
	while(*st){
	    if(*tmp == *st){
		ch = *st;
		*tmp = '\0';
		in = get_tbl_begin(arg);
		*tmp = ch;
		if(in != NULL) return set_1_lng(in, ++tmp);
		else return 0;
	    }
	    st++;
	}
	tmp++;
    }
    return 0;
}


char *search[] = {"print",		//1
			"system",
			"my_system",	//"expl1" befor "expl"
			"get_file",
			"save_file_full",
			"save_file",	//5
			"savecfg",
			"init",
			"fill_all_cfg",
			"fill_cfg",	//10
			"get_var",	//11
			"boot_page",
			"save_bfile",	//13
			"bla",		//14  not used!!
			"if_set",		//15 - the same as set!!
			"change_line",		//16
			"if_changed",		//17
			"not_changed",		//18
			"if",		//19
			"not",		//20
			"shell",	//21
			"my_shell",	//22
			"clean_ip",	//23
			"buf_if_eof",	//24
			"buf_parse_area",	//25
			"write_file",	//26
			"write_par",	//27
			"write_system",	//28
			"cat_system",	//29
			"bind_par",	//30
			"remove_chars",	//31
			"show_chars",	//32
			"dub_ppar",	//33--not used!
			"exit_cgi",	//34
			"chroot",	//35
			"exist_file",	//36
			"nnot",		//37
			"test",	//38
			"chtbl_stat",	//39
			"write_ppar",	//40
			"fill_tbl",	//41
			"copy_ppar",	//42
			"clean_par",	//43
			"get_ffile",	//44
			"cat_par",	//45
			"else",		//46  see jump code!!
			"boot_cgi",
			"mark_arg",	//48
			"change_alines",	//49
			"set_tbl",		//50
			"set",		//51
			"free_tbl",	//52
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

	if(*data == '}'){data++; return data;}

	j = 0;
	while(search[j]){
	    len = strlen(search[j]);
	    if(!strncmp(data, search[j], len)){
		data = data + len;
		ptr->cmd[*i] = j + 1;
//printf("cmd: %s:%d, len %d\n", search[j], *i, len);
//		strncpy(ptr->arg[*i], "", 1);
		break;//see data++;
	    }
	    j++;
	}
	if(search[j] == NULL){

	if(*data == 'd' && *(data+1) == 'o'){		// do{ }
	    j = 2; while(*(data+j) == ' ' || *(data+j) == '\t' || *(data+j) == '\n'){ j++; }
	    if(*(data+j) == '{') {data=data+j+1; j = *i; tmp = cgi_loop(data, i, ptr);
		if(tmp) ptr->bb[*i] = j; //tmp replace via data
//printf("bb: %d, to %d\n",*i, ptr->bb[*i]);
		data = tmp;
		continue;
	    }
	}
	    fprintf(stderr, "Undef. command\n");
	    return NULL;
	}
	while(*data == ' ' || *data == '\t' || *data == '\n' || *data == '\r') {data++;}

	if(*data == '\"') {data++;
//			    tmp = parsestr(data, "/{*/B\\\\\\\\/\\\\\\\"/\\\\\\?/\\??/{*/B\\\\\\\\/\\\\\\?/\\/|/E/}??/\\/|/E/}\"");
//			    tmp = parsestr(data, "/{*/B\\\\\\\\/\\/////\\//\\\\/\\\\\\\"/\\//?/\\\\\\?/\\??/{*/B\\\\\\\\/\\/////\\//\\\\/\\\\\\?/\\/|/E/}??/\\/|/E/}\"");
			    tmp = parsestr(data, "/{*/B\\\\\\\\/\\/////\\//\\\\/\\\\\\\"/\\/|/E/}\"");
			    if(tmp){
//			    printf("char: %.4s---\n", tmp);
				    len = tmp-data-1;//-1 is because of " at the end
				    if(ptr->arg[*i] == NULL){
				    ptr->arg[*i] = (char *)malloc(len+2);
				    if(ptr->arg[*i] != NULL){
					strncpy(ptr->arg[*i], data, len);
					*(ptr->arg[*i] + len) = '\0';
				    }
				    }else printf("ERR: Overwrite to \"\" line: %d\n", *i + 1);
//printf("arg: %s", ptr->arg[*i]);
			    data = tmp;
			    } else data = "";
//	    continue;
	}	//string
	while(*data == ' ' || *data == '\t' || *data == '\n' || *data == '\r') {data++;}
	if(*data == ';') {data++; (*i)++;if(*i >= GET_CGI_MAX) return NULL; ptr->cmd[*i] = 0; continue;}

	if(*data == '{') {data++; j = *i; (*i)++; if(*i >= GET_CGI_MAX) return NULL; tmp = cgi_loop(data, i, ptr); //return NULL- is a problem!!
	    if(tmp) ptr->bb[j] = *i; //tmp replace via data
//printf("bb: %d, to %d\n",j, ptr->bb[j]);
	    data = tmp;
	    continue;
	}

#ifdef DEBUG
putchar(*data);
#endif
//	data++;
	return NULL;
    }
return data;
}

void parse_cgi_script(char *data){


    int i = 0;
    char *tmp;
    struct cgi **ptr;


    tmp = w_strtok(&data, '\n');
    if(!tmp){ printf("Unable to def. length\n"); return;}
    if(data - tmp > 320){ printf("Length of cgi name > 30\n"); return;}
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
	    (*ptr)->arg[i] = NULL;
	    //strncpy((*ptr)->arg[i], "", 1);
	    (*ptr)->bb[i] = i;
	    i++;
	}
	(*ptr)->data_ptr = NULL;
	i = 0;
	if(!cgi_loop(data, &i, *ptr)){	//if something wrong - erase entry of this cgi-script
	    fprintf(stderr, "ERR: script %s\n", (*ptr)->name);
	    i = 0;
	    while(i < GET_CGI_MAX){		//clear all cgi_lines
		if((*ptr)->arg[i] != NULL){ free((*ptr)->arg[i]); /*(*ptr)->arg[i] = NULL;*/}
		i++;
	    }
	    free(*ptr);
	    *ptr = NULL;
	}
}
/*
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
*/
/*
inline int load_file(char *parm, FILE *out){

    parm = get_var(NULL, parm);
    if(!parm || !(*parm)){
	fprintf(out, "[ERR] Variable not found\n");
	return -1;
    }
    return (copy_file_include(parm, out));
}
*/
extern char *print200ok_mime;

int get_cgi(FILE *out, char *filename, int flag){

    int i = 0, j = 0, jump = 0, allocated, before = 0;
    unsigned long long size, size1, size2;//size2 - for copy_ppar
    struct cgi *ptr;
    struct parsestr strct;
    char *tmp, *tmp1, *tmp2, *arg = NULL;
    FILE *f;
    static int cloop = 0;  //used in boot_cgi


    ptr = cgi_name;
    while(ptr){
//printf("cgi: %s\n", ptr->name );
	tmp = parsestr2(&strct, NULL, filename, ptr->name);
//	if(tmp) strcmy(cgi_file, tmp);
	restore_str(&strct);
	if(tmp){
//	if(!strcmp(ptr->name, filename)){
	    cgi_used = ptr;
	    if(*(ptr->name) != '_' && flag != 1) fprintf(out, print200ok_mime, "text/html"); //script.cgi - with mime, _script.cgi - without
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
		} /*else {
		    printf("ptr->arg[i] is empty\n");
		    cgi_used = NULL;
		    return 0;
		}*/
		switch (ptr->cmd[i]){
		    case 1: //print
			    jump = print(out, arg); //run in braces if print returns 0;
			    break;
		    case 2: system_(arg);break;
		    case 3: my_system(out, arg);break;
		    case 4: //get_file
		    case 44: //get_ffile  -> means get file without pasrsing of .html or .inc files
			    size = strncpy_(NULL, arg, 0);	//this is max. size of arg-string
			    if(size && (tmp = malloc(size+1))){
//			    if(tmp = malloc(size)){
				strncpy_(tmp, arg, size);
//printf("get_file:%s %ld\n", tmp, size);
				if(ptr->cmd[i] == 4) {if(!copy_file_include(tmp, out)) jump = 1;} //exec in braces if file not found 
									//jump if error memory allocate
				else {if(!copy_file(tmp, out)) jump = 1;}
				free(tmp);
			    }else jump = 1;//error allocate memory
			    break;
		    case 5://save_file_full
		    case 6: tmp1 = arg;			//save_file arg:file_name
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp2 = get_var(&size1, tmp2);
				if(tmp2){
				    size = strncpy_(NULL, tmp1, 0);
				    if(size && (tmp = malloc(size+1))){
//				    if(tmp = malloc(size)){
					strncpy_(tmp, tmp1, size);
					if((f = fopen(tmp,"w+")) == NULL)
					    fprintf(out, "Unable to open file: %s", tmp);
					else {
					    if(ptr->cmd[i] == 5 && size1) fwrite(tmp2, size1, 1, f);//if size1 == 0->make fprintf
					    else fprintf(f, "%s", tmp2);
					    fclose(f);
					}
				    free(tmp);
				    }
				}
			    } break; //save_file
		    case 7: SaveConfiguration();break;		//savecfg (all changed flags will be zeroed)
		    case 8: ReadConfiguration1();break;		//init
		    case 9: fill_all_cfg();break;
		    case 10: fill_cfg(arg);break;
		    case 11: tmp = get_var(NULL, arg);
			    if(tmp) fprintf(out, "%s", tmp);/*maybe here print(out, tmp);*/ break;
		    case 12:		//boot_page
/* -replace- to the page 'filename'
    for work need IP and PORT from config file
*/
				fprintf(out,"<script language=\"JavaScript\">\n"
					    "location.replace('http://%s:%s/"
					    , CONFIG.IP, CONFIG.ADMIN_PORT);
				print_page(out, arg);

				fprintf(out,"');\n</script>\n<noscript>Follow <a href=\"http://%s:%s/", CONFIG.IP, CONFIG.ADMIN_PORT );
				print_page(out, arg);
				fprintf(out, "\">link</a></noscript>");
				//fprintf(out,"window.location.href=\"%s\";\n",filename);
			    if(allocated) free(arg);
			    cgi_used = NULL;
			    return 1;
//			    break;	//the same as: 
		/*	"<script language=\"JavaScript\">\n"
			"location.replace('http://??_srv_ip??:??_srv_port??/filename.htm');\n</script>"
			"\n<noscript><a href=\"http://??_srv_ip??:??_srv_port??/filename.htm\">filename.htm</a></noscript>"
			*/
		    case 13: 
			    //save_bfile	name:size_limit:firmware:file_name
			    //name, file_name
			    jump = save_bfile_(out, arg);
			    break;
		    case 14://get_cgi --not used
			    break;
		    case 51: //set(ptr, ptr->arg[i]);		//set
			    //break;
		    case 15:					//if_set
			    if(!set(ptr, arg)){ jump = 1;}
			    break;
		    case 16:				//change_line
			    jump = change_line(arg, 0);
			    break;
		    case 17:				//if_changed
//printf("if_changed: %s; i=%d; bb=%d\n", ptr->arg[i], i, ptr->bb[i]);
			    if(!cfg_arg_changed(arg)){ jump = 2;}
			    break;
		    case 18:				//not_changed or not found
			    if(cfg_arg_changed(arg)) { jump = 2;}
			    break;
		    case 19:				//if
//printf("if: %s; i=%d; bb=%d\n", ptr->arg[i], i, ptr->bb[i]);
			    if(cfg_arg_strcmp(arg, 0)) { jump = 2;}
			    break;
		    case 20:				//not
			    if(cfg_arg_strcmp(arg, 1)) { jump = 2;}
			    break;
		    case 21:shell(arg);		//shell
			    break;
		    case 22:my_shell(out, arg);		//my_shell
			    break;
/* write to global variables */
		    case 23:     check_ip[0] = '\0';	//clean_ip
			    free_ticket_all();
			    break;
/* buffer functions */
		    case 24:	//buf_if_eof - if not found in buffer
			if(ptr->data_ptr != NULL) { jump = 1;}
			    break;
		    case 25:	//buf_parse_area
			if(ptr->parse == NULL){
			    printf("buf_parse_area: Pointed to NULL\n");
			    if(allocated) free(arg);
			    cgi_used = NULL;
			    return 0;
			}
			tmp = parsestr(ptr->parse, arg);
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
		    case 26:	// write_file  par:file_name
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
		    case 27:	// write_par  par:value
		    case 45:	// cat_par par:value
			    tmp1 = arg;
			    tmp2 = w_strtok(&tmp1, ':');
			    if(tmp2 && *tmp2 && *tmp1){
			    	tmp = get_var(&size, tmp2);
				if(tmp && size){
				if(ptr->cmd[i] == 45){
				    size1 = strlen(tmp);
				    if(size > (size1 + 1)){
					strncpy_(tmp + size1, tmp1, size-1-size1);
//					tmp[size-1] = '\0';
				    }
				}else {
				    strncpy_(tmp, tmp1, size-1);
//					tmp[size-1] = '\0';
				}
				}
//				if(tmp2 != tmp1) *(tmp1-1) = ':';
			    }
			    break;
		    case 28:	// write_system  from_par:to_par:cmd	or :to_par:cmd
		    case 29:	// cat_system  from_par:to_par:cmd	or :par:cmd
			    wr_shell(arg, (ptr->cmd[i] == 28) ? 0 : 1);
			    break;
		    case 30:	// bind_par  par
			    tmp = get_var(&size, arg);
			    if(tmp && size){
				ptr->data_ptr = tmp;
				ptr->parse = tmp;//used in 24 - buf_parse_area
			    }else{
				ptr->data_ptr = NULL;
				ptr->parse = NULL;
			    }
			    break;
		    case 31:		//remove_chars
			    remove_show_chars(arg, 0);
			    break;
		    case 32:		//show_chars
			    remove_show_chars(arg, 1);
			    break;
		    case 33: //dub_ppar
			    break;
		    case 34: //exit_cgi
			    if(allocated) free(arg);
			    cgi_used = NULL;
			    return 1;
		    case 35: //chroot
			    if(*arg == '\0') tmp = CONFIG.WEB_ROOT; //if empty str -> use default value
			    else tmp = arg;

			    if(chdir(tmp)) { jump = 1;}
			    break;
		    case 36: //exist_file
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
		    case 37:			//nnot		-not exist or not matches
			    if(!cfg_arg_strcmp(arg, 0)) { jump = 2;}
			    break;
		    case 38:			//test
			    jump = print(stdout, arg); //run in braces if print returns 0;
fflush(stdout);
//			    printf("%s\n", arg);

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
		    case 39:
			    change_tbl_stat(arg);	//arg="flag:name:frase(forParsing)"
			    break;
		    case 40://arg="from_par:to_par:parsestr"
			    jump = write_ppar(arg);
			    break;
		    case 41://arg="par:par1:par2.."
			    fill_tbl(arg);
			    break;
		    case 42: //copy_ppar
			    //arg="from_par:to_par:offset"
			    tmp = arg;//offset
			    tmp1 = w_strtok(&tmp, ':');//from_par
			    tmp2 = w_strtok(&tmp, ':');//to_par
			    if(tmp2 && *tmp2 && tmp1 && *tmp1 && *tmp){
				size2 = atoll(tmp);
				tmp1 = get_var(&size, tmp1);//from_par
				if(tmp1){
				    if(size == 0) size = strlen(tmp1) + 1;//read only from_par
				    tmp = get_var(&size1, tmp2);//to_par
				    if(tmp){
					if(size1){
						if(size2 < size){//offset < from_par
						    size = size-size2;
						    strncpy(tmp, tmp1+size2, (size < size1) ? size : (size1-1));
						    if(size >= size1) tmp[size1-1] = '\0';
						    jump = 1;
						}
					}
				    }else{
					if(size2 < size) jump = reg_par(tmp2 ,tmp1+size2 , size-size2);//to_par
				    }
				}
			    }
			    break;
		    case 43://clean_par arg="par:par1:par2.."
			    tmp1 = arg;
			    while(tmp2 = w_strtok(&tmp1, ':')){
				tmp2 = get_var(&size, tmp2);
				if(tmp2 && size){
				    memset(tmp2, '\0', size);
				}
			    }
			    break;
		    case 46:	//else {} or else "if" {}
			    if(i){	//not first command in cgi struct
				if(*arg == '\0' || !strcmp(search[ptr->cmd[before] - 1], arg)){
				    if(jump) jump = 0;//if arg not exist or matched
				    else jump = 1;
				}
			    }
			    break;
		    case 47://boot_cgi "dir.cgi";
			    if(cloop <= 10){ //if > as 10 then don't jump
				cloop++;
				if(DoCGI(out, arg, 1) == 0) jump = 1;//jump if cgi is found
				cloop--;
			    } else printf("get_cgi: loop is %d in \"%s\" running \"%s\"\n", cloop, ptr->name, arg);
			    break;
		    case 48://mark_arg, global argumets, like user_3
			    mark_arg(arg, 0);
			    break;
		    case 49:	//change_alines
			    change_line(arg, 1);
			    break;
		    case 50:					//if set_tbl
			    if(!set_tbl(arg)){ jump = 1;}
			    break;
		    case 52:					//if set_tbl
			    free_table(arg);
			    break;
		}//switch end
	    before = i;		//which step was before
	    fflush(out);
	    if(allocated) free(arg);
//printf("check: [%d]: %s a=%d b=%d\n", i, ptr->arg[i], ptr->a, ptr->b);
	    if(jump){
		if(i!=ptr->bb[i]) i = ptr->bb[i];
		else i++;
		if((ptr->cmd[i] != 46) /*else function*/ ) jump = 0;
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
    int i = 0;
    while(i < GET_CGI_MAX){		//clear all cgi_lines
	if(ptr->arg[i] != NULL){ free(ptr->arg[i]);}
	i++;
    }
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


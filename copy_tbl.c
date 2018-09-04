/* copy_tbl.c:
 *
 * Copyright (C) 2013-2018  Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>//errno
#include <string.h>
#include <sys/types.h>
#include <dirent.h>		//opendir, readdir
#include <sys/stat.h>
#include "copy_CGI.h"
#include "parse_CGI.h"	//w_strtok, cfg_arg_strcmp, cfg_arg_changed
#include "copy.h"
#include "parser.h"
#include "copy_tbl.h"
//#include "include/httpd.h"

#define DEBUG

struct tbl *tbl_name = NULL;
//void get_folder(struct rnd_tbl **rnd_ptr, char *folder_name){
//
//}


//if i = 0 or Return NULL -> not found, if i!=0 and return not NULL ->found
char *parsestr_mass(char *str, char **massive, int *i){
	int k = 0;
	char *ptr;
	while(massive[k]){
		ptr = parsestr1(str, massive[k]);
		if(ptr){
			*i = (k + 1);
			return ptr;
		}
		k++;
	}
	*i = 0;
	return NULL;
}

void parse_tbl(char *data){

    char *tmp, *tmp2, *err = "ERR: allocate memory %d\n";
    struct tbl **ptr;
    struct rnd_tbl **rnd_ptr, *rnd_p;
    struct parsestr prsstr;
//    long int rnd;
    int flag = 1;//used 
    int k;

		    struct stat stbuf;
		    FILE *f;
		    DIR *dir;

    tmp = w_strtok(&data, '\n');
    if(!tmp){ printf("Unable to def. length\n"); return;}
//    if(data - tmp > 30){ printf("Length of cgi name > 30\n"); return;}
    ptr = &tbl_name;
    while(*ptr){
	if(!strcmp((*ptr)->name, tmp)){
	    flag = 0;
	    break;
	}
	ptr = &((*ptr)->next);		//add tbl struct to the end
    }
    if(flag){
	*ptr = (struct tbl *)malloc(sizeof(struct tbl));
	if(*ptr == NULL){
	    printf(err, 0);
	    return;
	}
	tmp2 = malloc(data - tmp + 1);
	if(tmp2 == NULL){
	    free(*ptr);
	    *ptr = NULL;
	    printf(err, 1);
	    return;

	}

	strcpy(tmp2, tmp);

	(*ptr)->name = tmp2;
	(*ptr)->ptr = NULL;
	(*ptr)->next = NULL;
    }
#ifdef DEBUG
printf("script %s\n", (*ptr)->name);
//    printf("%s", data);
#endif
    rnd_ptr = &((*ptr)->ptr);
    while(*rnd_ptr) rnd_ptr = &((*rnd_ptr)->next);

    while(tmp = w_strtok(&data, '\n')){
	if(*tmp){
	    if(*tmp == '#' && *(tmp+1) == '-' && *(tmp+2) == '>'){
//check if fold_name exist, if yes - then make link
//#-> check_folders
//fold_name
//#-> end
//check if fold_name NOT exist, if yes - then make link
//#-> check_Nfolders
//fold_name
//#-> end
		//tmp2 = parsestr1(tmp+3, "/ check_folders/ /");
		char *massive[]={	"/ check_folders/ /", //flag = 1
					"/ check_Nfolders/ /", //flag = 2
					"/ check_files/ /", //flag = 3
					NULL
				};
		if(parsestr_mass(tmp+3, massive, &flag)){

		    while(tmp = w_strtok(&data, '\n')){
			tmp2 = parsestr1(tmp, "#->/ end/ /");//thing about it!
			if(tmp2){
			    //data = tmp2;
			    break;
			}
			if(flag == 1 || flag == 2 || flag == 3){
				if((dir = opendir(tmp)) == NULL){
				    if(flag == 1) continue;	//jump if not exist
				}else{
				    closedir(dir);
				    if(flag == 2 || flag == 3) continue;	//jump if exist.
				    //flag=3 -is here because of no recognition between of file and folder in fopen function.
				}
			    if(flag == 3){
				if((f = fopen(tmp, "r"))==NULL){
				    continue;	//jump
				}else fclose(f);
			    }
			} else continue;//skip if not 1 or 2 or 3

			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 2);
			return;
			}

			rnd_p = *rnd_ptr;
			sprintf(rnd_p->rnd_entry, "%ld", random());
			rnd_p->entry = malloc(data - tmp + 1);
			if(rnd_p->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    printf(err, 3);
			    return;
			}

			sprintf(rnd_p->entry, "%s", tmp);
//			(*rnd_ptr)->entry = tmp2;
			k = 0;
			while(k < TAB_LEN){
			rnd_p->entries[k] = NULL;
			k ++;
			}
			rnd_p->p_flag = 0;
			rnd_p->flag = 0;
			rnd_p->next = NULL;
#ifdef DEBUG
printf("Exist folder: %s %s\n", rnd_p->rnd_entry, rnd_p->entry);
#endif

			rnd_ptr = &(rnd_p->next);


		    }	//end of while
		    continue;
		}
//*****************
		char *massive1[]={	"/ get_folder:/ ", //flag = 1
					"/ get_files:/ ", //flag = 2
					NULL
				};
		tmp2 = parsestr_mass(tmp+3, massive1, &flag);	//#-> get_folder:  /mnt
		if(tmp2 && flag){
//here can be added strncpy_ for /path/??par??
		    char *tmp5;
		    unsigned long long size;
		    size = strncpy_(NULL, tmp2, 0)+1;	//this is max. size of tmp2-string
		    if(size > 1 && (tmp5 = malloc(size))){
			strncpy_(tmp5, tmp2, size);
			if((dir = opendir(tmp5))== 0){
				printf("Unable to open directory %s, %d\n", tmp5, errno);
				free(tmp5);
				continue;
			}
		    } else continue;
//printf("file:%d, dir:%d\n", DT_REG, DT_DIR);

		    struct dirent *dirent;
		    //get_folder(rnd_ptr, tmp2);
		    while((dirent = readdir(dir)) != NULL){
			tmp = dirent->d_name;
printf("file:%s, %d\n", tmp, dirent->d_type);
			if(flag == 1){
			    if(dirent->d_type == DT_REG || dirent->d_type == 0 || !strcmp(tmp, "lost+found") 
				|| (tmp[0] == '.' && (tmp[1] == '\0' || tmp[1] == '.')))
				//d_type==8 is file(DT_REG), 10-link, 4-dir(DT_DIR).
			    continue;
			}else{
			    if(dirent->d_type != DT_REG && dirent->d_type != 0) continue;
			    //i dont know, but file_type on NAS is 0
			}
			
			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 4);
			    closedir(dir);
			    free(tmp5);
			return;
			}

			rnd_p = *rnd_ptr;
			sprintf(rnd_p->rnd_entry, "%ld", random());
			rnd_p->entry = malloc(strlen(tmp) + strlen(tmp2)+ 4);
			if(rnd_p->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    printf(err, 5);
			    closedir(dir);
			    free(tmp5);
			    return;
			}

			sprintf(rnd_p->entry, "%s/%s", tmp2, tmp);
			k = 0;
			while(k < TAB_LEN){
			rnd_p->entries[k] = NULL;
			k ++;
			}
			rnd_p->p_flag = 0;
			rnd_p->flag = 0;
			rnd_p->next = NULL;
#ifdef DEBUG
printf("Special: %s %s %d\n", rnd_p->rnd_entry, rnd_p->entry, dirent->d_type);
#endif

			rnd_ptr = &(rnd_p->next);

		    }
		    closedir(dir);
		    free(tmp5);
		    continue;
		//    system("/bin/ls -F -w 1 /mnt | grep -v spool> /var/run/mnt_dir");
		}
//********************
		char *massive2[]={	"/ parse_file:/ ",	//flag = 1	#-> parse_file:  /etc/fstab
					"/ parse_area:/ ",	//flag = 2	#-> parse_area:  AREA-1
					NULL
				};
		tmp2 = parsestr_mass(tmp+3, massive2, &flag);	//#-> get_folder:  /mnt
		if(tmp2 && flag){
		    char *dat, *tmp3, *tmp4[TAB_LEN], *tmp5, *tmp6, *check = NULL, *mixed = NULL;
		    int i = 0;
		    unsigned long long size;
			tmp3 = parsestr1(data, "/ while:/[/*/]\n");
			if(tmp3){ 
			    mixed = parsestr1(point[1], "/ mixed\n");
			    //data = point[1];
			    check = parsestr1(point[1], "/ check:/[/*/]\n");
			    data = point[1];
			    while(tmp5 = parsestr1(data, "/ if:/[/*/]\n")){
				data = point[1];
				if(i < TAB_LEN){		//upto 10 "if:" entries will be recorded
				    tmp4[i] = tmp5;
				    i++;
				}
				if(data == NULL) break;
			    }
			}
			if(i == 0) continue; //i - is in use
		    //file open from tmp2

		    if(flag == 1){
			size = strncpy_(NULL, tmp2, 0)+1;	//this is max. size of tmp2-string
			if(size > 1 && (tmp5 = malloc(size))){
			    strncpy_(tmp5, tmp2, size);

			    if(stat(tmp5, &stbuf) || /*(stbuf.st_size>1024*64) ||*/ !(f = fopen(tmp5,"r"))) {
				printf("Unable to open file %s, %d\n", tmp5, errno);
				free(tmp5);
				continue;
			    }

			    dat = (char *)malloc(stbuf.st_size+1);
			    if(dat == NULL){ fclose(f); free(tmp5); continue;}
			    fread(dat, stbuf.st_size, 1, f);
			    dat[stbuf.st_size] = '\0';
			    fclose(f);

			    free(tmp5);
			} else continue;


		    }else{//flag ==2 or higher
			tmp5 = get_var(&size, tmp2);
			if(tmp5 == NULL || size == 0) continue;//non writeable not parse
			dat = (char *)malloc(size);
			if(dat == NULL){continue;}
			strmycpy(dat, tmp5, size);
		    }

		    tmp5 = dat;
		    while(tmp = parsestr1(tmp5, tmp3)){
			if(tmp5 == point[1]) break;
			tmp5 = point[1];
			if(! *tmp) continue;//empty string
			if(check && (parsestr1(tmp, check) == NULL)) continue;//if check not found -> skip parseing  //new
			if((tmp6 = parsestr2(&prsstr, tmp, tmp4[0])) == NULL)//think about!!	//was parsestr1
			    continue;

			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 6);
			    free(dat);
			return;
			}

			rnd_p = *rnd_ptr;
			sprintf(rnd_p->rnd_entry, "%ld", random());
	flag = 0;
	while(1){
	    if(flag >= TAB_LEN) break;
	    if(tmp6){
		if(i == 1){
			rnd_p->entry = malloc(strlen(tmp6) + 4);
			if(rnd_p->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    free(dat);
			    printf(err, 7);
			    return;
			}

			sprintf(rnd_p->entry, "%s", tmp6);
			rnd_p->entries[0] = NULL;
		}else{
			if(flag == 0) rnd_p->entry = NULL;//for begin
			rnd_p->entries[flag] = malloc(strlen(tmp6) + 4);
			if(rnd_p->entries[flag] == NULL){
			    flag--;
			    while(flag >= 0){
				if(rnd_p->entries[flag] != NULL) free(rnd_p->entries[flag]);
				flag--;
			    }
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    free(dat);
			    printf(err, 7);
			    return;
			}

			sprintf(rnd_p->entries[flag], "%s", tmp6);
		}
		if(flag+1 < i){
//		    tmp = point[1];
		    if(mixed == NULL) tmp = restore_str(&prsstr);
		    else restore_str(&prsstr);
		    tmp6 = parsestr2(&prsstr, tmp, tmp4[flag+1]);//think about!//was parsestr1
		} else tmp6 = NULL;
	    } else {
		rnd_p->entries[flag] = NULL;
		if(flag+1 < i){
		    tmp6 = parsestr2(&prsstr, tmp, tmp4[flag+1]);
		}
	    }
	flag ++;
	}
			rnd_p->p_flag = 0;
			rnd_p->flag = 0;
			rnd_p->next = NULL;
#ifdef DEBUG
printf("Special: %s %s\n", rnd_p->rnd_entry, rnd_p->entry);
#endif

			rnd_ptr = &(rnd_p->next);

		    }
		    free(dat);
		    continue;
		    
		}
	    }
//end of if(#->)
	    *rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
	    if(*rnd_ptr == NULL){
		printf(err, 8);
		return;
	    }

	    rnd_p = *rnd_ptr;
	    sprintf(rnd_p->rnd_entry, "%ld", random());
	    tmp2 = malloc(data - tmp + 1);
	    if(tmp2 == NULL){
		free(*rnd_ptr);
		*rnd_ptr = NULL;
		printf(err, 9);
		return;
	    }

	    strcpy(tmp2, tmp);
	    rnd_p->entry = tmp2;
	    k = 0;
	    while(k < TAB_LEN){
		rnd_p->entries[k] = NULL;
		k ++;
	    }
	    rnd_p->p_flag = 1;
	    rnd_p->flag = 0;
	    rnd_p->next = NULL;
#ifdef DEBUG
printf("%s %s\n", rnd_p->rnd_entry, rnd_p->entry);
#endif
	    rnd_ptr = &(rnd_p->next);
	}
    }
}

char *get_tbl(char *par){
    char *tmp, *var;
    unsigned int part = 0;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;

    var = w_strtok(&par, '#');//par="table_name#1"
    if(var == NULL) return NULL;
    if(!*par) part = 0;
    else{
	part = atoi(par);
	if(part >= TAB_LEN){return NULL;}//out of range
    }

    while(ptr){
	if(!strcmp(ptr->name, var)){
	    if((par = malloc(strlen(var) + 16)) == NULL) return NULL;
	    sprintf(par, "%s_%d", var, part);//arg is "tabname_0"
	    tmp = get_arg(par, NULL, 0);//NULL - only readable!!
	    free(par);
	    if(tmp){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(!strcmp(rnd_ptr->rnd_entry, tmp)){
			if(rnd_ptr->entry) return rnd_ptr->entry;
			return rnd_ptr->entries[part];
		    }
		rnd_ptr = rnd_ptr->next;
		}
	    }
	return NULL;
	}
    ptr = ptr->next;
    }
    return NULL;
}

//used in strncpy_()
//var = tab_name#3	-row=4
unsigned long long show_tbl_str(char *var, char *buf, unsigned long long size){
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    unsigned long long s = 0; //in s is '\0' not included!!!
    char *entry, *name;
    unsigned int part = 0;

    name = w_strtok(&var, '#');
    if (!name && !(*name)){printf("tbl_str: Undef. name\n"); return s;}

    if(!*var) part = 0;
    else{
	part = atoi(var);
	if(part >= TAB_LEN){printf("tbl_str: ROW out of range %d\n", part); return s;}//out of range
    }

  if(*name == '_'){	//if var="_table" - show list of all elements, if exist and flag == 0.
    name++;
    while(ptr){
	if(!strcmp(ptr->name, name)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){

		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		    if(entry && rnd_ptr->flag == 0){ 
			//p_flag==0 -> only print, ==1 ->print with conclusions
			if(rnd_ptr->p_flag == 0){
			    if(buf) s += strmycpy(buf + s, entry, size-s+1);
			    else s += strlen(entry);
			
			}else s += strncpy_(buf ? (buf + s) : NULL, entry, size-s);	//here must manual made "\n" in rnd->entry
			if(buf){	//make at the end of string \n-enter
			    if(s<size){
				buf[s] = '\n';
				s++;
			    }
			    buf[s]='\0';
			}else s++;// \n counts!
		    }

		rnd_ptr = rnd_ptr->next;
		}
	return s;
	}
    ptr = ptr->next;
    }
    return s;
  }
  return s;
}

void show_tbl(char *name, FILE *out){
    char *var, *tmp;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    struct parsestr strct;
    char *entry, *tmp1;
    unsigned int part = 0;


    var = w_strtok(&name, ':');
    if(var == NULL || *var == '\0') return;

    tmp1 = w_strtok(&var, '#');
    if (!tmp1 && !(*tmp1)){printf("show_tbl: Undef. name\n"); return;}

    if(!*var) part = 0;
    else{
	part = atoi(var);
	if(part >= TAB_LEN){printf("show_tbl: ROW out of range %d\n", part); return;}//out of range
    }


  if(*tmp1 == '_'){	//if var="_table" - show list of all elements, if exist and flag == 0.
    tmp1++;
    while(ptr){
	if(!strcmp(ptr->name, tmp1)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		    if(entry){
		      if(rnd_ptr->flag == 0){ 
			//p_flag==0 -> only print, ==1 ->print with conclusions
			if(rnd_ptr->p_flag == 0) fprintf(out, "%s\n", entry);
			else print(out, entry);	//here must manual made "\n" in rnd->entry
		      }
		    }
		rnd_ptr = rnd_ptr->next;
		}
	return;
	}
    ptr = ptr->next;
    }
    return;
  }
//default: show <select> object
    while(ptr){
	if(!strcmp(ptr->name, tmp1)){
	    fprintf(out, "<select name=\"%s_%d\" >\n", tmp1, part);
	    
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		    if(entry){
		      if(rnd_ptr->flag == 0){ 
			if(tmp = parsestr2(&strct, entry, name)){//bind at 'name'
			    fprintf(out, "<option value=\"%s\" >", rnd_ptr->rnd_entry);
			    //p_flag==0 -> only print, ==1 ->print with conclusions
			    if(rnd_ptr->p_flag == 0) fprintf(out, "%s", tmp);
			    else print(out, tmp);
			    fprintf(out, "</option>\n");
			    restore_str(&strct);
			}
		      }
		    }
		rnd_ptr = rnd_ptr->next;
		}

	    fprintf(out, "</select>\n");
	return;
	}
    ptr = ptr->next;
    }
}

//show checkbox val
void show_tbl_chck(char *name, FILE *out){
    char *var, *tmp;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    struct parsestr strct;
    char *entry, *tmp1;
    unsigned int part = 0;


    var = w_strtok(&name, ':');
    if(var == NULL || *var == '\0') return;

    tmp1 = w_strtok(&var, '#');
    if (!tmp1 && !(*tmp1)){printf("show_tbl: Undef. name\n"); return;}

    if(!*var) part = 0;
    else{
	part = atoi(var);
	if(part >= TAB_LEN){printf("show_tbl: ROW out of range %d\n", part); return;}//out of range
    }

  if(*tmp1 == '_'){	//if var="_table" - show all elements, ignoring if exist and flag == 0.
    tmp1++;
    while(ptr){
	if(!strcmp(ptr->name, tmp1)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		  if(entry){
		    fprintf(out, "<input type=\"checkbox\" name=\"%s_%d\" value=\"%s\" %s>",
			    tmp1, part, rnd_ptr->rnd_entry, rnd_ptr->flag == 0 ? "checked" : "");
		    //p_flag==0 -> only print, ==1 ->print with conclusions
		    if(rnd_ptr->p_flag == 0) fprintf(out, "%s\n", entry);
		    else print(out, entry);	//here must manual made "\n" in rnd->entry
		    fprintf(out, "<br>\n");
		  }
		rnd_ptr = rnd_ptr->next;
		}
	return;
	}
    ptr = ptr->next;
    }
    return;
  }
//default: show <input=checkbox ... > object
    while(ptr){
	if(!strcmp(ptr->name, tmp1)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		  if(entry){
		    if(rnd_ptr->flag == 0){ 
			if(tmp = parsestr2(&strct, entry, name)){//bind at 'name'
			    fprintf(out, "<input type=\"checkbox\" name=\"%s_%d\" value=\"%s\">",
				tmp1, part, rnd_ptr->rnd_entry);
			    //p_flag==0 -> only print, ==1 ->print with conclusions
			    if(rnd_ptr->p_flag == 0) fprintf(out, "%s", tmp);
			    else print(out, tmp);
			    fprintf(out, "<br>\n");
			    restore_str(&strct);
			}
		    }
		  }
		rnd_ptr = rnd_ptr->next;
		}
	return;
	}
    ptr = ptr->next;
    }
}

void change_tbl_stat(char *data){
    char *name, *flag, tmp, *var, *entry;
    unsigned int part = 0;
    unsigned long long size = 0;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    FILE *f;
    DIR *dir;

    flag = w_strtok(&data, ':');	//data="flag:name:frase(forParsing)" or data="flag:name#4:frase(forParsing)"
    if (!flag || !(*flag)){printf("Undef. flag\n"); return;}
    var = w_strtok(&data, ':');
    if (!var){printf("Undef. name 1\n"); return;}
    name = w_strtok(&var, '#');
    if (!name && !(*name)){printf("Undef. name\n"); return;}
    if(!*var) part = 0;
    else{
	part = atoi(var);
	if(part >= TAB_LEN){printf("tbl: ROW out of range %d\n", part); return;}//out of range
    }

    tmp = *(flag+1);//used for second flag (if exist)
    var = NULL;
    while(ptr){
	if(!strcmp(ptr->name, name)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){//d: dir exist, e: dir not exist, f: file exist
//
		    if(rnd_ptr->entry) entry = rnd_ptr->entry;
		    else entry = rnd_ptr->entries[part];//check row by row

		    if(entry){
			    if(rnd_ptr->p_flag == 1){
				size = strncpy_(NULL, entry, 0)+1;	//this is max. size of cmd-string
				if(var = malloc(size)){
					strncpy_(var, entry, size);
					entry = var;
				}else{
					goto next;
				}
			    }

		      if(entry && parsestr1_(entry, data)){
			if(tmp == 'd' || tmp == 'f' || tmp == 'e'){

				if((dir = opendir(entry)) == NULL){
				    if(tmp == 'd') goto next;	//jump if not exist
				}else{
				    closedir(dir);
				    if(tmp == 'f' || tmp == 'e') goto next;	//jump if exist.
				    //flag='f' -is here because of no recognition between of file and folder in fopen function.
				}
				if(tmp == 'f'){
				    if((f = fopen(entry, "r"))==NULL){
					goto next;	//jump
				    }else fclose(f);
				}

			}

			if(*flag == 's') rnd_ptr->flag = 0;		//show
			else if(*flag == 'n') rnd_ptr->flag = 1;	//not show
			else rnd_ptr->flag = !(rnd_ptr->flag);		//toggle
		      }
		    }
//
next:
		if(var){ free(var); var = NULL;}
		rnd_ptr = rnd_ptr->next;
		}
	return;
	}
    ptr = ptr->next;
    }
}

//find table by name
struct rnd_tbl *find_tbl(char *name){
	struct tbl *ptr = tbl_name;

	while(ptr){
		if(!strcmp(ptr->name, name))
			return (ptr->ptr);
		ptr = ptr->next;
	}
	return NULL;
}

void free_rnd_tbl(struct rnd_tbl **ptr){
    if(!ptr || !*ptr) return;
    free_rnd_tbl(&((*ptr)->next));
    if((*ptr)->entry){
#ifdef DEBUG
    printf("free rnd_tbl '%s'\n", (*ptr)->entry);
#endif
    free((*ptr)->entry);
    }else{
	int i = 0;
	while(i < TAB_LEN){
	if((*ptr)->entries[i]){
#ifdef DEBUG
		printf("free rnd_tbl entry %d '%s'\n", i, (*ptr)->entries[i]);
#endif
		free((*ptr)->entries[i]);
	}
	i++;
	}
    }
    free(*ptr);	//at hier action
    *ptr = NULL;
}

void free_tbl_1(struct tbl **ptr){
    if(!ptr || !*ptr) return;
    free_tbl_1(&((*ptr)->next));
#ifdef DEBUG
    printf("free tbl '%s'\n", (*ptr)->name);
#endif
    free_rnd_tbl(&((*ptr)->ptr));
    free((*ptr)->name);
    free(*ptr);	//at hier action
    *ptr = NULL;
}

void free_tbl(void){
    free_tbl_1(&tbl_name);
    tbl_name = NULL;
}




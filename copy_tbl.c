/* copy_tbl.c:
 *
 * Copyright (C) 2013-2014  Alexander Reimer <alex_raw@rambler.ru>
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
    struct rnd_tbl **rnd_ptr;
//    long int rnd;
    int flag = 1;//used 

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
			
			sprintf((*rnd_ptr)->rnd_entry, "%ld", random());
			(*rnd_ptr)->entry = malloc(data - tmp + 1);
			if((*rnd_ptr)->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    printf(err, 3);
			    return;
			}

			sprintf((*rnd_ptr)->entry, "%s", tmp);
//			(*rnd_ptr)->entry = tmp2;
			(*rnd_ptr)->p_flag = 0;
			(*rnd_ptr)->flag = 0;
			(*rnd_ptr)->next = NULL;
#ifdef DEBUG
printf("Exist folder: %s %s\n", (*rnd_ptr)->rnd_entry, (*rnd_ptr)->entry);
#endif

			rnd_ptr = &((*rnd_ptr)->next);


		    }	//end of while
		    continue;
		}
//*****************
		tmp2 = parsestr1(tmp+3, "/ get_folder:/ ");	//#-> get_folder:  /mnt
		if(tmp2){
		    struct dirent *dirent;
		    if((dir = opendir(tmp2))== 0){
    			printf("Unable to open directory %s, %d\n", tmp2, errno);
			continue;
		    }

		    //get_folder(rnd_ptr, tmp2);
		    while((dirent = readdir(dir)) != NULL){
			tmp = dirent->d_name;
			if(dirent->d_type == 8 || !strcmp(tmp, "lost+found") 
				|| (tmp[0] == '.' && (tmp[1] == '\0' || tmp[1] == '.')))	//d_type==8 is file, 10-link, 4-dir.
			    continue;
			
			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 4);
			    closedir(dir);
			return;
			}
			
			sprintf((*rnd_ptr)->rnd_entry, "%ld", random());
			(*rnd_ptr)->entry = malloc(strlen(tmp) + strlen(tmp2)+ 4);
			if((*rnd_ptr)->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    printf(err, 5);
			    closedir(dir);
			    return;
			}

			sprintf((*rnd_ptr)->entry, "%s/%s", tmp2, tmp);
//			(*rnd_ptr)->entry = tmp2;
			(*rnd_ptr)->p_flag = 0;
			(*rnd_ptr)->flag = 0;
			(*rnd_ptr)->next = NULL;
#ifdef DEBUG
printf("Special: %s %s %d\n", (*rnd_ptr)->rnd_entry, (*rnd_ptr)->entry, dirent->d_type);
#endif

			rnd_ptr = &((*rnd_ptr)->next);

		    }
		    closedir(dir);
		    continue;
		//    system("/bin/ls -F -w 1 /mnt | grep -v spool> /var/run/mnt_dir");
		}
//********************
		tmp2 = parsestr1(tmp+3, "/ parse_file:/ ");	//#-> parse_file:  /etc/fstab
		if(tmp2){
		    char *dat, *tmp3, *tmp4, *tmp5, *tmp6;
			tmp3 = parsestr1(data, "/ while:/[/*/]\n");
			if(tmp3){ data = point[1];
			    tmp4 = parsestr1(data, "/ if:/[/*/]\n");
			    if(tmp4){ data = point[1];
				goto parse;
			    }
			}
			continue;
		    //file open from tmp2
parse:

		    if(stat(tmp2, &stbuf) || /*(stbuf.st_size>1024*64) ||*/ !(f = fopen(tmp2,"r"))) {
			printf("Unable to open file %s, %d\n", tmp2, errno);
			continue;
		    }

		    dat = (char *)malloc(stbuf.st_size+1);
		    if(dat){
			fread(dat, stbuf.st_size, 1, f);
			dat[stbuf.st_size] = '\0';

		    tmp5 = dat;
		    while(tmp = parsestr1(tmp5, tmp3)){
			if(tmp5 == point[1]) break;
			tmp5 = point[1];
			if(! *tmp) continue;//new
			if((tmp6 = parsestr1(tmp, tmp4)) == NULL)
			    continue;

			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 6);
			    fclose(f);
			    free(dat);
			return;
			}

			sprintf((*rnd_ptr)->rnd_entry, "%ld", random());
			(*rnd_ptr)->entry = malloc(strlen(tmp6) + 4);
			if((*rnd_ptr)->entry == NULL){
			    free(*rnd_ptr);
			    free(dat);
			    fclose(f);
			    *rnd_ptr = NULL;
			    printf(err, 7);
			    return;
			}

			sprintf((*rnd_ptr)->entry, "%s", tmp6);
//			(*rnd_ptr)->entry = tmp2;
			(*rnd_ptr)->p_flag = 0;
			(*rnd_ptr)->flag = 0;
			(*rnd_ptr)->next = NULL;
#ifdef DEBUG
printf("Special: %s %s\n", (*rnd_ptr)->rnd_entry, (*rnd_ptr)->entry);
#endif

			rnd_ptr = &((*rnd_ptr)->next);

		    }
		    free(dat);
		    } else printf("ERR: Unable allocate memory.\n");
		    fclose(f);
		    continue;
		    
		}
	    }
//end of if(#->)
	    *rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
	    if(*rnd_ptr == NULL){
		printf(err, 8);
		return;
	    }

	    sprintf((*rnd_ptr)->rnd_entry, "%ld", random());
	    tmp2 = malloc(data - tmp + 1);
	    if(tmp2 == NULL){
		free(*rnd_ptr);
		*rnd_ptr = NULL;
		printf(err, 9);
		return;
	    }

	    strcpy(tmp2, tmp);
	    (*rnd_ptr)->entry = tmp2;
	    (*rnd_ptr)->p_flag = 1;
	    (*rnd_ptr)->flag = 0;
	    (*rnd_ptr)->next = NULL;
#ifdef DEBUG
printf("%s %s\n", (*rnd_ptr)->rnd_entry, (*rnd_ptr)->entry);
#endif
	    rnd_ptr = &((*rnd_ptr)->next);
	}
    }
}

char *get_tbl(char *var){
    char *tmp;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    
    while(ptr){
	if(!strcmp(ptr->name, var)){
	    tmp = get_arg(var, 0);
	    if(tmp){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(!strcmp(rnd_ptr->rnd_entry, tmp))
			return rnd_ptr->entry;
		rnd_ptr = rnd_ptr->next;
		}
	    }
	return NULL;
	}
    ptr = ptr->next;
    }
    return NULL;
}

void show_tbl(char *var, FILE *out){
//    char *tmp;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;

  if(*var == '_'){	//if var="_table" - show list of all elements, if exist and flag == 0.
    var++;
    while(ptr){
	if(!strcmp(ptr->name, var)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->flag == 0){ 
			//p_flag==0 -> only print, ==1 ->print with conclusions
			if(rnd_ptr->p_flag == 0) fprintf(out, "%s\n", rnd_ptr->entry);
			else print(out, rnd_ptr->entry);	//here must manual made "\n" in rnd->entry
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
	if(!strcmp(ptr->name, var)){
	    fprintf(out, "<select name=\"%s\" >\n", var);
	    
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->flag == 0){ 
			fprintf(out, "<option value=\"%s\" >", rnd_ptr->rnd_entry);
			//p_flag==0 -> only print, ==1 ->print with conclusions
			if(rnd_ptr->p_flag == 0) fprintf(out, "%s", rnd_ptr->entry);
			else print(out, rnd_ptr->entry);
			fprintf(out, "</option>\n");
		    }
		rnd_ptr = rnd_ptr->next;
		}

	    fprintf(out, "</select>\n");
	return;
	}
    ptr = ptr->next;
    }
}

void change_tbl_stat(char *data){
    char *name, *flag, tmp;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    FILE *f;
    DIR *dir;

    flag = w_strtok(&data, ':');	//data="flag:name:frase(forParsing)"
    if (!flag || !(*flag)){printf("Undef. flag\n"); return;}
    name = w_strtok(&data, ':');
    if (!name && !(*name)){printf("Undef. name\n"); return;}
    tmp = *(flag+1);//used for second flag (if exist)
    while(ptr){
	if(!strcmp(ptr->name, name)){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){//d: dir exist, e: dir not exist, f: file exist
		    if(parsestr1_(rnd_ptr->entry, data)){ 
			if(tmp == 'd' || tmp == 'f' || tmp == 'e'){
				if((dir = opendir(rnd_ptr->entry)) == NULL){
				    if(tmp == 'd') goto next;	//jump if not exist
				}else{
				    closedir(dir);
				    if(tmp == 'f' || tmp == 'e') goto next;	//jump if exist.
				    //flag='f' -is here because of no recognition between of file and folder in fopen function.
				}
			    if(tmp == 'f'){
				if((f = fopen(rnd_ptr->entry, "r"))==NULL){
				    goto next;	//jump
				}else fclose(f);
			    }
			}

			if(*flag == 's') rnd_ptr->flag = 0;		//show
			else if(*flag == 'n') rnd_ptr->flag = 1;	//not show
			else rnd_ptr->flag = !(rnd_ptr->flag);		//toggle
		    }
next:
		rnd_ptr = rnd_ptr->next;
		}
	return;
	}
    ptr = ptr->next;
    }
}

void free_rnd_tbl(struct rnd_tbl **ptr){
    if(!ptr || !*ptr) return;
    free_rnd_tbl(&((*ptr)->next));
#ifdef DEBUG
    printf("free rnd_tbl '%s'\n", (*ptr)->entry);
#endif
    free((*ptr)->entry);
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




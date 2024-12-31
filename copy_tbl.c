/* copy_tbl.c:
 *
 * Copyright (C) 2013-2021  Alexander Reimer <alex_raw@rambler.ru>
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

//#define DEBUG

struct tbl *tbl_name = NULL;

void parse_tbl(char *data, char clean){

    char *tmp, *tmp2, *err = "ERR: allocate memory %d\n";
    struct tbl **ptr;
    struct rnd_tbl **rnd_ptr, *rnd_p, *p;
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
	(*ptr)->begin = 0;
	(*ptr)->ptr = NULL;
	(*ptr)->next = NULL;
    } else if(clean) free_rnd_tbl(&((*ptr)->ptr));
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
		if(parsestr(tmp+3,"/n0N/ check_/Bfolders/ /n1n/\\" //flag = 1
					"Nfolders/ /n2n/\\" //flag = 2
					"files/ /n3n/E/" //flag = 3
					)){
		    flag = number;

		    while(tmp = w_strtok(&data, '\n')){
			tmp2 = parsestr(tmp, "#->/ end/ /");//thing about it!
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
		tmp2 = parsestr(tmp+3, "/n0N/ get_/Bfolder:/n1n/t/\\" //flag = 1
					"files:/n2n/t/\\" //flag = 2
					"ofiles:/n3n/t/\\" //flag = 3
					"ofolder:/n4n/t" //flag = 4
					);	//#-> get_folder:  /mnt
		flag = number;
		if(tmp2 && flag){
//here can be added strncpy_ for /path/??par??
		    char *tmp5, *check_ = NULL;
		    unsigned long long size;
		    check_ = parsestr(data, "/ check:/[/!/B/!/*/]\n/\\/*/{*/////}//]/r/m+TBL_folder_files:check has //]\\n\\0");
		    if(point[1]){
			data = point[1];
			if(check_ == NULL) check_ = "/!";//nothing show at all!!
		    }
//		    check_ = parsestr(data, "/ check:/[/*/]\n");
//		    if(check_){
//			data = point[1];
//			if(parsestr(check_, "/*/{*/////}//]")){
//			    check_ = "/!";//nothing show at all!!
//			    printf("Warning: check is with /]\n");
//			}
//		    }

		    size = strncpy_(NULL, tmp2, 0);	//this is max. size of tmp2-string
		    if(size && (tmp5 = malloc(size+1))){
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
#ifdef DEBUG
printf("file:%s, %d\n", tmp, dirent->d_type);
#endif
			if(flag == 1 || flag == 4){
			    if(dirent->d_type == DT_REG || dirent->d_type == 0 || !strcmp(tmp, "lost+found") 
				|| (tmp[0] == '.' && (tmp[1] == '\0' || tmp[1] == '.')))
				//d_type==8 is file(DT_REG), 10-link, 4-dir(DT_DIR).
			    continue;
			}else{
			    if(dirent->d_type != DT_REG && dirent->d_type != 0) continue;
			    //i dont know, but file_type on NAS is 0
			}
			if(check_ && (parsestr(tmp, check_) == NULL)) continue;//if check not found -> skip parseing  //new

			*rnd_ptr = (struct rnd_tbl *)malloc(sizeof(struct rnd_tbl));
			if(*rnd_ptr == NULL){
			    printf(err, 4);
			    closedir(dir);
			    free(tmp5);
			return;
			}

			rnd_p = *rnd_ptr;
			sprintf(rnd_p->rnd_entry, "%ld", random());
			if(flag == 3 || flag == 4) rnd_p->entry = malloc(strlen(tmp) + 4);//!!!only filename or path!!! if flag = 3 || 4
			else rnd_p->entry = malloc(strlen(tmp) + strlen(tmp5)+ 4);
			if(rnd_p->entry == NULL){
			    free(*rnd_ptr);
			    *rnd_ptr = NULL;
			    printf(err, 5);
			    closedir(dir);
			    free(tmp5);
			    return;
			}

			if(flag == 3 || flag == 4) sprintf(rnd_p->entry, "%s", tmp);//!!!only filename or path!!! if flag = 3 || 4
			else sprintf(rnd_p->entry, "%s/%s", tmp5, tmp);
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
		tmp2 = parsestr(tmp+3, "/n0N/ parse_/Bfile:/n1n/t/\\"	//flag = 1	#-> parse_file:  /etc/fstab
					"area:/n2n/t"	//flag = 2	#-> parse_area:  AREA-1
					);	//#-> get_folder:  /mnt
		flag = number;
		if(tmp2 && flag){
		    char *dat, *tmp3, *tmp4[TAB_LEN], *tmp5, *tmp6, *check = NULL, *mixed = NULL;
		    int i = 0;
		    unsigned long long size;
			tmp3 = parsestr(data, "/ while:/[/*/]\n");
			if(tmp3){ 
			    data = point[1];
			    mixed = parsestr(data, "/ mixed\n");
			    if(point[1]) data = point[1];
			    check = parsestr(data, "/ check:/[/!/B/!/*/]\n/\\/*/{*/////}//]/r/m+TBL:check has //]\\n\\0");
//			    check = parsestr(data, "/ check:/[/*/]\n");
			    if(point[1]) data = point[1];
			    while(tmp5 = parsestr(data, "/ if:/[/*/]\n")){
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
			size = strncpy_(NULL, tmp2, 0);	//this is max. size of tmp2-string
			if(size && (tmp5 = malloc(size+1))){
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
		    while(tmp = parsestr(tmp5, tmp3)){
			if((unsigned char *)tmp5 == point[1]) break;	//changed 1.05.2019
			tmp5 = point[1];
			if(! *tmp) continue;//empty string
			if(check && (parsestr(tmp, check) == NULL)) continue;//if check not found -> skip parseing  //new
			if((tmp6 = parsestr2(&prsstr, NULL, tmp, tmp4[0])) == NULL)//think about!!	//was parsestr1
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
		    tmp6 = parsestr2(&prsstr, NULL, tmp, tmp4[flag+1]);//think about!//was parsestr1
		} else tmp6 = NULL;
	    } else {
		rnd_p->entries[flag] = NULL;
		if(flag+1 < i){
		    if(prsstr.end) tmp = prsstr.end;
		    tmp6 = parsestr2(&prsstr, NULL, tmp, tmp4[flag+1]);
		}
	    }
	flag ++;
	}//while(1)
			rnd_p->p_flag = 0;
			rnd_p->flag = 0;
			rnd_p->next = NULL;

#ifdef DEBUG
printf("Special: %s %s\n", rnd_p->rnd_entry, rnd_p->entry);
#endif

//no double entries
			p = (*ptr)->ptr;
			while(p){
			    if(i == 1){
				if(rnd_p->entry && p->entry && (rnd_p->entry != p->entry)
						 && !strcmp(rnd_p->entry, p->entry)){
printf("free double: %s\n", rnd_p->entry);
				free(rnd_p->entry);
				free(*rnd_ptr);
				*rnd_ptr = NULL;
				break;
				}
			    } else {
				flag = 0;
				while(flag < TAB_LEN && flag < i){
					if(rnd_p->entries[flag] == NULL && p->entries[flag] == NULL){flag++; continue;}
					if(rnd_p->entries[flag] == NULL || p->entries[flag] == NULL ||
					(rnd_p->entries[flag] == p->entries[flag]) ||
					strcmp(rnd_p->entries[flag], p->entries[flag])) break;
				flag++;
				}
				if(flag == i){//matched compliete
printf("free double i=%d entries[0]: %s\n", i, rnd_p->entries[0]);
					flag--;
					while(flag >= 0){
					    if(rnd_p->entries[flag] != NULL) free(rnd_p->entries[flag]);
					    flag--;
					}
					free(*rnd_ptr);
					*rnd_ptr = NULL;
					break;
				}
			    }
			    p = p->next;
			}
//end of double entries
			if(p == NULL) rnd_ptr = &(rnd_p->next);

		    }
		    free(dat);
		    continue;
		    
		}
//***********************
		if(tmp2 = parsestr(tmp+3, "/tbegin:/t")){//#->begin: _par
		    if(tmp2 = get_var(NULL, tmp2)){
			(*ptr)->begin = atoll(tmp2);				//in term_tbl.c it is atoi !!!!!
		    }
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

unsigned long long *get_tbl_begin(char *name){
    struct tbl *ptr = tbl_name;
    char *tmp1;

    while(ptr){
	if(!strcmp(ptr->name, name)){
	    return &(ptr->begin);
	}
    ptr = ptr->next;
    }
    return NULL;
}


char *get_tbl(char *parm){
    char *tmp, *tmp1, *par, *par1 = NULL, *par2 = NULL, *var, *rvar, ch = '\0', ch_dctn = '\0';
    unsigned int part = 0, rpart = 0;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
    struct parsestr p_ptr;
/*
    var = w_strtok(&par, '_');//par="table_name_0#1"  -> get table_name_0 and use 1 entry of that
    if(var == NULL) return NULL;//var = table_name
    rvar = w_strtok(&par, '#');//rvar="_0" and par = "#1"

    if(!rvar || !*rvar) rpart = 0;
    else{
	rpart = atoi(rvar);
	if(rpart >= TAB_LEN){return NULL;}//out of range
    }
    if(!*par) part = rpart;
    else{
	part = atoi(par);
	if(part >= TAB_LEN){return NULL;}//out of range
    }
*/
    switch(*parm){
	case '#':
	case '$': ch = *parm; parm++; break;
    }

    tmp1 = parsestr2_s(&p_ptr, NULL, parm, "/[/*/]/t|/t");
    if(tmp1) {par = tmp1; par1 = p_ptr.end;//par1 = point[1];
	if(par1 && *par1){ 
	    par2 = parsestr2_s(&p_ptr, NULL, par1, "/<1<>/>?if?/[/!/B/!/*/]?fi?/t|/t/\\/*/{*/////}//]/r");
//	    par2 = parsestr2(&p_ptr, NULL, par1, "/<1<>/>?if?/[/*/]?fi?/t|/t");
	    if(par2){
		char *par3 = p_ptr.end;//point[1];
//		if(parsestr(par2, "/*/{*/////}//]")){//par2 has /]
//		    par2 = 0;
//		    restore_str(&p_ptr);
//		}else{
		    ch_dctn = *par1; //Can be < or >
		    par1 = par3;
//		}
	    }//else restore_str(&p_ptr);//think about!!
	}
    } else par = parm;

    rvar = w_strtok(&par, '#');//par="table_name#1" - the same as table_name+0#1
    if(rvar == NULL) return NULL;//par="table_name+0#1"  -> get table_name+0 and use 1 entry of that
    var = w_strtok(&rvar, '+');	//par="table_name+5#1" var=table_name rvar=5 par=1
    if(var == NULL) return NULL;

    if(!*rvar) rpart = 0;
    else{
	rpart = atoi(rvar);
	if(rpart >= TAB_LEN){return NULL;}//out of range
    }
    if(!*par) part = rpart;
    else{
	part = atoi(par);
	if(part >= TAB_LEN){return NULL;}//out of range
    }


    while(ptr){
	if(!strcmp(ptr->name, var)){
	    if(tmp1 == NULL){
		if((par = malloc(strlen(var) + 16)) == NULL) return NULL;
		sprintf(par, "%s_%d", var, rpart);//arg is "tabname_0"
		tmp = get_arg(par, NULL, 0);//NULL - only readable!!
		free(par);// ??_@tab+0#4?? the same as: ??_@tab#4 | _#tab_0??
	    } else {
		if(par1 && *par1) tmp = get_var(NULL, par1);
		else return NULL;
	    }

	    if(tmp){
		rnd_ptr = ptr->ptr;
		while(rnd_ptr){
		    if(rnd_ptr->entry){
			var = rnd_ptr->entry;	//tab#3
			rvar = var;		//tab+2
		    } else {
			var = rnd_ptr->entries[part];	//tab#3
			rvar = rnd_ptr->entries[rpart];	//tab+2
		    }

		    if(ch == '\0'){
			if(!strcmp(rnd_ptr->rnd_entry, tmp)) return var;
		    } else {

		if(var){//not zero!, rvar can be everything (as NULL as)
		    if(par2){
			if(ch_dctn == '<'){//  var | <?if? compare string == par2 ?fi? | tmp
			    bucks = tmp;
			    if(parsestr2_s(NULL, NULL, var, par2)){
				if(ch != '#') return rvar;
				else return rnd_ptr->rnd_entry;
			    }
			} else if(ch_dctn == '>'){//  var | >?if? compare string == par2 ?fi? | tmp
			    bucks = var;
			    if(parsestr2_s(NULL, NULL, tmp, par2)){
				if(ch != '#') return rvar;
				else return rnd_ptr->rnd_entry;
			    }
			}
			bucks = NULL;
		    } else {	//if not exist:  <> ?if? ... ?fi? |
				//simple compare
			if(!strcmp(var, tmp)){
			    if(ch != '#') return rvar;
			    else return rnd_ptr->rnd_entry;
			}
		    }
		}
		    
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

// ??_**$tabl+10?? - show rnd_entry of begin+10 element or ??_*$tab+10#2?? - show entry[2] of begin+10 element
char *get_table(char *par, int flag){
    char *var, *rvar, ch = 0;
    unsigned int part = 0, rpart = 0;
    unsigned long long b = 0;
    struct tbl *ptr = tbl_name;
    struct rnd_tbl *rnd_ptr;
/*
    var = w_strtok(&par, '_');//par="table_name_0#1"  -> get table_name_0 and use 1 entry of that
    if(var == NULL) return NULL;//var = table_name
    rvar = w_strtok(&par, '#');//rvar="_0" and par = "#1"

    if(!rvar || !*rvar) rpart = 0;
    else{
	rpart = atoi(rvar);
	if(rpart >= TAB_LEN){return NULL;}//out of range
    }
    if(!*par) part = rpart;
    else{
	part = atoi(par);
	if(part >= TAB_LEN){return NULL;}//out of range
    }
*/
    if(*par == '$'){ ch = '$'; par++;}


    rvar = w_strtok(&par, '#');//par="_**table_name+0" or par="_*table_name+0#1"
    if(rvar == NULL) return NULL;//par="table_name+0#1"  -> get table_name+0 and use 1 entry of that
    var = w_strtok(&rvar, '+');	//par="table_name+5#1" var=table_name rvar=5 par=1
    if(var == NULL) return NULL;

    if(!*rvar) rpart = 0;//must be 1 for 0
    else{
	rpart = atoi(rvar);
//	if(rpart >= TAB_LEN){return NULL;}//out of range
    }
    if(!*par) part = 0;
    else{
	part = atoi(par);
	if(part >= TAB_LEN){return NULL;}//out of range
    }

    while(ptr){
	if(!strcmp(ptr->name, var)){
	    rnd_ptr = ptr->ptr;
	    if(ch == '$'){
		b = ptr->begin;
		while(rnd_ptr){
		    if (b == 0) break;
		    b--;
		    rnd_ptr = rnd_ptr->next;
		}
	    }
	    while(rnd_ptr){
		if(rpart == 0){
		    if(flag == 0){
			if(rnd_ptr->entry) return rnd_ptr->entry;	//flag == 0	(_*tab+0#4)
			return rnd_ptr->entries[part];
		    }else{
			return rnd_ptr->rnd_entry;			//flag == 1	(_**tab+0)
		    }
		}else rpart--;
		
	    rnd_ptr = rnd_ptr->next;
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
			if(tmp = parsestr2(&strct, NULL, entry, name)){//bind at 'name'
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
			if(tmp = parsestr2(&strct, NULL, entry, name)){//bind at 'name'
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
				size = strncpy_(NULL, entry, 0);	//this is max. size of cmd-string
				if(var = malloc(size+1)){
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

//t cannot be NULL!!!
//**p is main row of table (it is or first or with "!" marked)
void reg_tabs(struct tabs **t, struct rnd_tbl **p, char **name, unsigned long value){
	char *tmp, *tmp1, *n = *name;
	unsigned long long counter;
	struct parsestr strct;
	struct tabs *T;
	struct tbl *ptr;
	struct rnd_tbl *p_rnd;

	*p = NULL;
	while(tmp = w_strtok(&n, ':')){
	    if(*tmp == '\0') {*tmp = ':'; *name = n; return;}	//it means '::n'
	    tmp1 = parsestr2(&strct, NULL, tmp, "/n0N/,!,/[/*/]/B\\01/\\_/sV/n2n");
	    //number = 1 if "table\0" or number = 2 if "table_(value number)"
	    ptr = tbl_name;
	    while(ptr){
		if(!strcmp(ptr->name, tmp1)){
		    p_rnd = ptr->ptr;
		    if(value){		//if value is exist then skip line to counter of begin_value
			counter = ptr->begin;
			while(p_rnd){
			    if(!counter) break;
			    else counter--;
			    p_rnd = p_rnd->next;
			}
		    }
		    if((tmp == *name) || (*tmp == '!')) *p = p_rnd;
		    //if == ! then begin is this entry (tab_1:tab_2:!tab_3)->tab_3
		    // or if not exist begin is begin of list (tab_1:tab_2:tab_3)->tab_1
		    T = (struct tabs *)malloc(sizeof(struct tabs));
		    if(T){
			while(*t) t = &((*t)->next);
			*t = T; T->n = p_rnd; T->flag = strct.num;
			if(strct.num == 2) T->num = value_;
			else T->num = 0;
			T->next = NULL;
		    } else printf("error allocate mem!\n");
		    break;
		}
		ptr = ptr->next;
	    }
	    restore_str(&strct);
	    if(*n) *(n-1) = ':';
	}
	*name = n;
}
void free_tabs(struct tabs **ptr){
	if(!ptr || !*ptr) return;
	free_tabs(&((*ptr)->next));
	free(*ptr);
	*ptr = NULL;
}

//used in tabs(): ?@@ls -l ?$ @@?
unsigned long long tabs_strncpy_(char *tmp, char *tmp1, long long size, struct tabs **tabs){
    //tmp -is out, tmp1 -is in. and return the size of string!
    //if tmp == NULL -> works as counter of string tmp1
    //'\0' -sign is not counted!! -> strncpy_() + 1 
    char ch, *tmp2, *c;
    struct tabs *t = *tabs;
    struct parsestr strct;
    unsigned long long s = 0;

    static int loop_strncpy = 0;	// used if loops exissted
    static char *ptr = NULL;//used for breaking loops like: "area:text to loop ??area??"

    if(loop_strncpy == 0){
	ptr = tmp;
    } else if(loop_strncpy > 10){
	printf("max loop counter in tabs reached\n");
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
		case '?':
		case '\"':
		case '\\':	ch = *tmp1;break;
		case 'n':	ch = '\n';break;
		case 't':	ch = '\t';break;
		default :	tmp1--;
	    }
	} else if(tmp2 = parsestr2(&strct, NULL, tmp1, "?@/[/N@N/*/]@?")){		//?@variable@?
		if(*tmp2 == '-')		//?@-_Table@?	- list
						//?@-Table@?	- <select>
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
	} else if(tmp2 = parsestr2(&strct, NULL, tmp1, "??/[/N?N/*/]??")){		//??variable??
		tmp2 = get_var(NULL, tmp2);		//get_var and get_variable
		if(tmp2) s += tabs_strncpy_(tmp ? (tmp+s) : NULL, tmp2, size-s, &t);
		tmp1 = restore_str(&strct);
		continue;
	} else if(*tmp1 == '?'){
		    if(*(tmp1+1) == '$'){//if ?$
		    tmp1 = tmp1 + 2;
		    if(t){
			c = NULL;
			if(t->n) {
			    c = t->n->entry;
			    if(c == NULL && t->num < TAB_LEN) c = t->n->entries[t->num];
			}
			if(c == NULL) c = "";
			if(t->n->p_flag == 0){ if(tmp == NULL) s += strlen(c); else s += sprintf(tmp+s, "%s", c);}
			else s += tabs_strncpy_(tmp ? (tmp+s) : NULL, c, size-s, &t);
			if(tmp && t->n) t->n = t->n->next;//next by column
			t = t->next;//next by line
		    }
		    continue;
		    }else if(*(tmp1+1) == '#'){//if ?# or ?#+
		    
		    tmp1 = tmp1 + 2;
		    if(*tmp1 == '+'){
			    ch = *tmp1;
			    tmp1++;
		    }
		    if(t){
			if(t->n) {
			    c = t->n->rnd_entry;
			} else c = "";
			if(tmp == NULL) s += strlen(c); else s += sprintf(tmp+s, "%s", c);
			if(ch == '+'){ if(tmp && t->n) t->n = t->n->next;//if ?#+ -> jump to next column
			    t = t->next;
			}
		    }
		    continue;
		    }
	}else if(c = ticket_find(&tmp1)){	//in c - ticket //not checked, please check it!!  - maybe not usefull here
		if(tmp == NULL) s += strlen(c); else s += sprintf(tmp+s, "%s", c);//fprintf()
		continue;
	}

	if(tmp) tmp[s] = ch;
	s++; tmp1++;
	if(s == 0) break;//is overload!
    }

    loop_strncpy--;
end:
    *tabs = t;
    if(tmp) tmp[s] = '\0';
    return s;
}


//str = 100:tab_1:tab_4::string
//or
//str = 0:tab_1:!tab_2:tab_4::string
//:the same as:
//str = tab_1:!tab_2:tab_4::string ?? <td>??</td>
void tabs(char *str, unsigned long value, FILE *out){
	struct tabs *tabs = NULL, *t, *t_tmp;
	struct rnd_tbl *p;
	struct parsestr strct;
	char ch, *c, *s, *tmp, *tmp1, *tmp2, *var, *var1, *argm;
	unsigned long counter = value;
	unsigned long long size = 0;

	reg_tabs(&tabs, &p, &str, value);
	while(p){
	    if(value && !counter) break;
	    t = tabs;
	    s = str;
	    while(*s){
		if(*s == '?'){
		    if(*(s+1) == '$'){//if ?$
		    s = s + 2;
		    if(t){
			c = NULL;
			if(t->n) {
			    c = t->n->entry;
			    if(c == NULL && t->num < TAB_LEN) c = t->n->entries[t->num];
			}
			if(c == NULL) c = "";
			if(t->n->p_flag == 0) fprintf(out, "%s", c);
			else print(out, c);
			if(t->n) t->n = t->n->next;
			t = t->next;
		    }
		    continue;
		    }else if(*(s+1) == '#'){//if ?# or ?#+
		    s = s + 2;
		    if(*s == '+'){
			    ch = *s;
			    s++;
		    }
		    if(t){
			if(t->n) {
			    c = t->n->rnd_entry;
			} else c = "";
			fprintf(out, "%s", c);
			if(ch == '+'){ if(t->n) t->n = t->n->next;//if ?#+ -> jump to next column
			    t = t->next;
			}
		    }
		    continue;
		    }else if(c = parsestr2(&strct, NULL, s, "?@@/[/*/]@@?")){		//?@@ls -l ?$ @@?

			if(t){
			t_tmp = t;

			size = tabs_strncpy_(NULL, c, 0, &t_tmp);	//this is max. size of arg-string

			    argm = malloc(size+1);
			    if(argm){
				t_tmp = t;
				tabs_strncpy_(argm, c, size, &t_tmp);
				my_system(out, argm);
				free(argm);
			    }
			    argm = NULL;
			    t = t_tmp;
			}

			s = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		    }else if(c = parsestr2(&strct, NULL, s, "??/[/N?N/*/]??")){		//??variable??
			c = get_var(NULL, c);		//get_var and get_variable
			if(c) print(out, c); /*fprintf(out, "%s", tmp3);*/
			s = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		    }else if(c = parsestr2(&strct, NULL, s, "?@/[/N@N/*/]@?")){		//?@variable@?
			if(*c == '-')		//?@-_Table@?	- list
							//?@-Table@?	- <select>
				show_tbl(c+1, out);
			else if(*c == '_')		//?@__Table@?	- <input type=checkbox>	- show all
							//?@_Table@?	- <input type=checkbox>	- show only showable
				show_tbl_chck(c+1, out);
			s = restore_str(&strct);
			continue;		//it's or tmp=tmp+2 or tmp=tmp2+2
		    }else if(tmp = parsestr2(&strct, NULL, s, "?if?/[/*:/{*/B\\\\\\\\/\\/////\\//\\\\/\\\\\\:/\\/////\\//:/\\/|/E/}:"
						"/{*/B\\\\\\\\/\\\\\\?/\\??/{*/B\\\\\\\\/\\/////\\//\\\\/\\\\\\?/\\/|/E/}??/\\/|/E/}/]?fi?")){
			var = NULL;//?if?tmp:tmp2:tmp1?fi?		was only:	?if?:....:....?fi?
			var1 = NULL;
			tmp1 = tmp;//variable
			while(*tmp1){
/*			    if(*tmp1 == '\\' && *(tmp1+1) == '\\') tmp1++;
			    else if(*tmp1 == '\\' && *(tmp1+1) == ':') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == '/') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == '\\') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == ':') tmp1++;
			    else */ if(*tmp1 == ':'){ break;}
			    tmp1++;
			}
			if(*tmp1 == ':'){
			    *tmp1 = '\0';
			    if(*tmp) var = get_var(NULL, tmp);
			    *tmp1 = ':';
			    tmp1++;
			}
			tmp2 = tmp1;//comparestring
			while(*tmp1){
			    if(*tmp1 == '\\' && *(tmp1+1) == '\\') tmp1++;
			    else if(*tmp1 == '\\' && *(tmp1+1) == ':') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == '/') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == '\\') tmp1++;
			    else if(*tmp1 == '/' && *(tmp1+1) == ':') tmp1++;
			    else if(*tmp1 == ':'){ break;}
			    tmp1++;
			}

			if(*tmp1  == ':'){//print string
			    *tmp1 = '\0';
			//in tmp -strcmp and in tmp1+1 -print
			    if(t){
				c = NULL;
				if(t->n) {
				    c = t->n->entry;
				    if(c == NULL && t->num < TAB_LEN) c = t->n->entries[t->num];
				}
				if(c == NULL) c = "";
				if(t->n->p_flag == 1){
				    size = strncpy_(NULL, var ? var : c, 0);	//this is max. size of cmd-string
				    if(var1 = malloc(size+1)){
					strncpy_(var1, var ? var : c, size);
					if(var) var = var1;
					else c = var1;
				    }
				}
				if(t->n) t->n = t->n->next;	//if (?if?) then "++" one time
				t = t->next;

				if(t){
				//if next column exist then run this. depends on next columns
				    t_tmp = t;
				    size = tabs_strncpy_(NULL, tmp1+1, 0, &t_tmp);	//this is max. size of arg-string

				    argm = malloc(size+1);
				    if(argm){
					t_tmp = t;
					tabs_strncpy_(argm, tmp1+1, size, &t_tmp);

					if(var){
					    bucks = c;	//get variable from outside with /$
					    if(parsestr(var, tmp2)) fprintf(out, "%s", argm);
					    bucks = NULL;
					} else{
					    if(parsestr(c, tmp2))
//				if(t->n->p_flag == 0) fprintf(out, "%s", c);
//				else print(out, c);
					    fprintf(out, "%s", argm);
					}

					free(argm);
					
				    }//arg = malloc()
				    t = t_tmp;
				} else {//if(t)
				//there is no next column - just put out string tmp1+1
				    if(var){
					bucks = c;	//get variable from outside with /$
					if(parsestr(var, tmp2)) print(out, tmp1+1);
					bucks = NULL;
				    } else{
					if(parsestr(c, tmp2))
//				if(t->n->p_flag == 0) fprintf(out, "%s", c);
//				else print(out, c);
					    print(out, tmp1+1);
				    }

				}//if(t)
				if(var1) free(var1);
			    }
			    *tmp1 = ':';
			}
			s = restore_str(&strct);
			continue;
		    }
		} //end of if(*s == '?')

		if(c = ticket_find(&s)){	//in c - ticket
			fprintf(out, "%s", c);
			continue;
		}

		ch = *s;
		if(ch == '\\'){
		    s++;
		    switch(*s){
		    case '\"':
		    case '\\':
		    case  '?':	ch = *s; break;
		    case  'n':	ch = '\n'; break;
		    case '\0':	continue;
		    default: 	s--;
		    }
		}
		putc(ch, out);
		s++;
	    }
	    if(value) counter--;
	    p = p->next;
	}
	free_tabs(&tabs);
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
//    tbl_name = NULL;
}

void free_table(char *name){
	struct tbl *p, **ptr;
	char *tmp;

    while(tmp = w_strtok(&name, ':')){
	ptr = &tbl_name;
	while(ptr && *ptr){
		p = *ptr;
		if(!strcmp(p->name, tmp)){
			free_rnd_tbl(&(p->ptr));
			free(p->name);
			p = p->next;
			free(*ptr);
			*ptr = p;
			break;
		}
		ptr = &(p->next);
	}
    }
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include "include/httpd.h"


#define MIN(a,b) ((a) < (b) ? (a) : (b))

void usage(void)
{
  printf("Httpd_config save v1.00\n"
	"	<file name> [parm]\n"
	"\n"
	"	 parm :    HOSTNAME=i-Drive\n"
	"\n");
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

char *strstrcfg(char *d, char *c, int *len)	/*check c string in d string*/
{

    if(*c == '\0') return NULL;
    while (*c)
	{
	    if (*c != *d)  break;
	    if (!(*d))  return NULL;//this line is not needed!!!
	    c++;
	    d++;
	}

	if (!(*c)){
	    if(*d != '=' && *(d+1) != '\'') return NULL;	//it's "=\'"  -- fresch
	    d = d + 2;

	    c = d;
	    while((*c != '\'') && *c) c++;	//can be danger
	    *c = '\0';
	    *len = c - d + 1;	//size of value (incl. termin. NULL) on which is d pointed
	    return d;
	}
    return NULL;
}


int main(int argc, char *argv[])
{
    int i = 0, j, k = 0;
    char arg_[256], *arg, *name;
    FILE *fip, *fop;
    char LineBuf[256];	//maybe buffer is to small

  if (argc != 2) {
    usage();
    return (-1);
  }

  // Duplicate the command line parameters
  j = MIN(strlen(argv[1]), 255);
  strncpy(arg_, argv[1], j);
  arg_[j] = '\0';
//printf("arg: %s\n", arg_);
  arg = arg_;
  name = w_strtok(&arg, '=');
  if(!name || !(*name) || !(*arg)) {usage();return(-1);}

    if((fip = fopen(ETC_PATH "/config","r")) == NULL || (fop = fopen(ETC_PATH "/config1","w+")) == NULL){
#ifdef DEBUG
        fprintf( stderr, "Save: Cannot open one of config files\n" );
#endif
	return (-1);
    }else{
        while(fgets(LineBuf,255,fip) != NULL){
		if((i != 1) && (strstrcfg(LineBuf, name, &k) != NULL)){
		    fprintf(fop,"%s=\'%s\'\n", name, arg);
		    i = 1;
		} else 
		    fputs(LineBuf,fop);
	}
	if(!i) fprintf(fop,"%s=\'%s\'\n", name, arg);


    fclose(fip);
    fclose(fop);
    rename(ETC_PATH "/config1", ETC_PATH "/config");
    chmod(ETC_PATH "/config", S_IRUSR|S_IWUSR);
    }
  return (0);
}


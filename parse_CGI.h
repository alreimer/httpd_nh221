//data: a pointer to the encoded form data
//out: a file pointer used for output
typedef void cgi_handler(FILE *out);
typedef struct{
    const char *name;	   /* The name of the script as given in the URL */
    cgi_handler *handler;  /* The function which should be called */
} CGI_ENTRY;

char *httpd_decode(char *string);	/*had name unescape*/
char *w_strtok(char **s, char d);/*finds d in s and replaced by '\0' s move to next char and returns pointer to beginning of s */

int DoCGI(FILE *out, char *filename, int flag);//if flag == 0 -with mime, == 1 - without mime

#define	ARGS_MAX		40
struct ARGS{
    char *name;	   /* The name of the script as given in the URL */
    char *value;  /* The function which should be called */
    unsigned long long size;	//in size is \0 - included
    struct ARGS *next;
};

//extern struct ARGS args[];
extern struct ARGS *args_ptr_local;
extern struct ARGS *args_ptr_global;
int handle_arg(int flag, char *input);		//flag = 0 - global, 1-local
char *get_arg(char *name, unsigned long long *size, int flag);
void fill_tbl(char *parm);
void free_arg(int flag);

/* not used yet */
int check_digit( char *string ); /*checks if in string all chars are digits*/
void delete_crlf( char *plag );	/*find \n or \r and make end of string at this place!*/


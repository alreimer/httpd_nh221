void parse_cgi_script(char *data);

#define GET_CGI_MAX 78
//#define GET_CGI_LEN 2048
/*for x_open_file */

struct cgi {
    char	name[320];	//name of cgi
    int		cmd[GET_CGI_MAX];
//    char	arg[GET_CGI_MAX][GET_CGI_LEN];
    char	*arg[GET_CGI_MAX];
    int		bb[GET_CGI_MAX];
    int 	a, b, c, d;	//used as variables
    char	*data_ptr;	//pointer for data-field
    char	*parse;		//pointer for data-field buf_parse_area
    struct cgi	*next;
};

extern struct cgi *cgi_name;	//begin of cgi

struct cgi *find_cgi(char *filename);	//find cgi in cgi-tree. if not found return NULL

int get_cgi(FILE *out, char *filename, int flag);
int print(FILE *out, char *text);//returns 1 on success
void system_(char *cmd);
void my_system(FILE *out, char *cmd);
void write_system(char *b_in, long long s_in, char *buf, long long size, int mode, char *cmd);
void shell(char *tmp);
void my_shell(FILE *out, char *tmp);
//if(buf_in == NULL) -> /dev/stdin is off. if(size_in == 0) -> strlen(buf_in)
void write_shell(char *buf_in, long long size_in, char *buf, long long size, int mode, char *cmd);
void wr_shell(char *arg, char flag);
void free_cgi(struct cgi *ptr);
void show_CGIs(FILE *out);

unsigned long long  strmycpy(char *tmp, char *tmp1, unsigned long long size);
unsigned long long strncpy_(char *tmp, char *tmp1, long long size);

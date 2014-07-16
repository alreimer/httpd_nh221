void parse_cgi_script(char *data);

#define GET_CGI_MAX 78
#define GET_CGI_LEN 2048
/*for x_open_file */

struct cgi {
    char	name[30];	//name of cgi
    int		cmd[GET_CGI_MAX];
    char	arg[GET_CGI_MAX][GET_CGI_LEN];
    int		bb[GET_CGI_MAX];
    int 	a, b, c, d;	//used as variables
    char	*data_ptr;	//pointer for data-field
    char	*parse;		//pointer for data-field buf_parse_area
    struct cgi	*next;
};

extern struct cgi *cgi_name;	//begin of cgi

struct cgi *find_cgi(char *filename);	//find cgi in cgi-tree. if not found return NULL

int get_cgi(FILE *out, char *filename);
void print(FILE *out, char *text);
void shell(char *tmp);
void my_shell(FILE *out, char *tmp);
void free_cgi(struct cgi *ptr);
void show_CGIs(FILE *out);

void strmycpy(char *tmp, char *tmp1, long long size);

void parse_tbl(char *data);
void free_tbl(void);
void show_tbl(char *var, FILE *out);
char *get_tbl(char *var);
void change_tbl_stat(char *data);	//data="flag:name:frase(forParsing)"

char *parsestr_mass(char *parsestr, char **massive, int *i);

struct rnd_tbl {
    char		rnd_entry[100];	//name of cgi
    int			flag;		//=1 - not show, =0 - show
    int			p_flag;		//used for =1 print(), and =0 fprintf()
    struct rnd_tbl	*next;
    char		*entry;
};

struct tbl {
    char		*name;	//name of cgi
    struct rnd_tbl	*ptr;
    struct tbl		*next;
};

extern struct tbl *tbl_name;	//begin of cgi


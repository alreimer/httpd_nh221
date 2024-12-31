void parse_tbl(char *data, char clean);
struct rnd_tbl *find_tbl(char *name);
void show_tbl(char *var, FILE *out);
void show_tbl_chck(char *var, FILE *out);
unsigned long long show_tbl_str(char *var, char *buf, unsigned long long size);
char *get_tbl(char *var);
char *get_table(char *var, int flag);
unsigned long long *get_tbl_begin(char *name);//name of table
void change_tbl_stat(char *data);	//data="flag:name:frase(forParsing)"
void free_table(char *name);

#define TAB_LEN 10

struct rnd_tbl {
    char		rnd_entry[100];	//name of cgi
    int			flag;		//=1 - not show, =0 - show
    int			p_flag;		//used for =1 print(), and =0 fprintf()
    struct rnd_tbl	*next;
    char		*entry;
    char		*entries[TAB_LEN];//if entry==NULL so use that entries
};

struct tbl {
    char		*name;	//name of cgi
    unsigned long long	begin;	//begin of window!, scrolling throw table
    struct rnd_tbl	*ptr;
    struct tbl		*next;
};

void free_rnd_tbl(struct rnd_tbl **ptr);
void free_tbl(void);
extern struct tbl *tbl_name;	//begin of cgi

struct tabs {
    struct rnd_tbl	*n;
    int			flag;		//0-not matched, 1-full tab (table), 2-one column of tab (table_3)
    int			num;
    struct tabs		*next;
};
void reg_tabs(struct tabs **t, struct rnd_tbl **p, char **name, unsigned long value);
void free_tabs(struct tabs **ptr);
void tabs(char *str, unsigned long value, FILE *out);

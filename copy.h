int copy_file(char *file, FILE *out);	/*copy file to stream out*/
int copy_file_include(char *file, FILE *out);/*copy file with include elements (used handle_gets())*/
int copy_file_buf(char *file, char *buf, long long size); /* copy file to buffer witn size */
int copy(FILE *read_f, FILE *write_f);
int handle_get(char *data, FILE *write_f);
extern unsigned char *point[2];
extern unsigned long number;
extern unsigned long value_;
extern unsigned char *bucks;
//char *parsestr( char *a, char *b);
unsigned char *parsestr(unsigned char *a, unsigned char *b);
unsigned char *parsestr1(unsigned char *a, unsigned char *b);
char *parsestr1_( char *a, char *b);
struct parsestr{
    char ch;
    unsigned long num;
    unsigned long val;
    char *zero;		//place, were ch was stored (for restoring)
    char *end;		//end of matched string
};

struct strctexec{
    unsigned long exec;
    char *begin;
    char *end;	//zero pointer (for restoring)
    unsigned long value;
    char ch;	//zero char
    struct strctexec *next;
};
typedef void exec_fnctn(struct strctexec *ptr);
void free_strctexec(void);

char *parsestr2(struct parsestr *ptr, exec_fnctn *fn, char *a, char *b);
char *restore_str( struct parsestr *ptr);//return the end of matched string (struct parsestr->end)
char *parsestr2_s( struct parsestr *ptr, exec_fnctn *fn, char *d, char *c);

void free_page_mem(void); //release memory for html page
void free_par_tmp(void);

char *get_var(unsigned long long *size_ptr, char *var_index);		//parse Varialbles

int reg_par(char *name, char *value, long long size);

//begin of ticket
struct ticket{
    int type;
    char *fname;
    char ticket[64];	//1233284374_7986543949
    struct ticket *next;
};
extern struct ticket *ticket_list;

void reg_ticket(char *name, int type);
char *check_ticket(char *name, int type);
char *ticket_find(char **name);
void free_ticket(void);
void free_ticket_all(void);
//end of ticket

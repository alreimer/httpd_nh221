int copy_file(char *file, FILE *out);	/*copy file to stream out*/
int copy_file_include(char *file, FILE *out);/*copy file with include elements (used handle_gets())*/
int copy_file_buf(char *file, char *buf, long long size); /* copy file to buffer witn size */
int copy(FILE *read_f, FILE *write_f);
int handle_get(char *data, FILE *write_f);
//char *stristr(/*const*/ char *a, /*const*/ char *b);
//char *strstrend( char *a, char *b);
extern char *point[2];
//char *parsestr( char *a, char *b);
char *parsestr1( char *a, char *b);
char *parsestr1_( char *a, char *b);
struct parsestr{
    char ch;
    unsigned int num;
    char *zero;		//place, were ch was stored (for restoring)
    char *end;		//end of matched string
};
char *parsestr2(struct parsestr *ptr, char *a, char *b);
char *restore_str( struct parsestr *ptr);//return the end of matched string (struct parsestr->end)

void free_page_mem(void); //release memory for html page
void free_par_tmp(void);

char *get_var(unsigned long long *size_ptr, char *var_index);		//parse Varialbles

int reg_par(char *name, char *value, long long size);

inline void radio_value_insert(char *line, char *var_head, FILE *out);
inline void include_(char *line, char *var_head, FILE *out);

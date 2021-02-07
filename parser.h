char *strstrcfg(char *a, char *b, int *len);
int  ReadConfiguration(void);
int  ReadConfiguration1(void);
int  SaveConfiguration(void);
int cfg_arg_strcmp(char *parm, char flag);
int cfg_arg_changed(char *web_name);
char *get_cfg_value(long long *size, char *field_name, int i);	//i is 0-value, 1-new_value, 2-fresh_value
void fill_all_cfg(void);
void fill_cfg(char *parm);//parm="parm1:parm2:parm3..."


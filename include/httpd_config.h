//used in parser.c and httpd.c RAW
//in httpd.c used for USERNAME AND PASSWORD and security(MAC_SECURITY_MODE&IP_SECURITY_MODE)
/*
struct cfg_parse {//NEED to remove it in httpd_scripts.c
	const char *name;	//used by config file parsing
	const char *web_name;	//used by web_parsing
	char *value;
	long long size;		//max string size
	int changed;		//by init - is 0	in cfg_parseargs will be changed and in SaveConfig will be cleared
};
*/
struct cfg_parse2 {//Used in ReadConfiguration1
	char *name;	//used by config file parsing
	char *value;
	int size;		//max string size
};

struct cfg_parse1 {	//for web config
	char *str;	//is whole string(will be cuted by replacing of ':' to '\0')
	char *name;	//pointer to begin of name
	char *web_name;	//pointer to begin of web_name
	char *value;	//pointer to begin of value in str
	char *new_value;	//pointer to begin of new_value in str
	char *pattern;	//pointer to begin of pattern in str
	long long size;		//max string size(is applied to value string!)
	int changed;		//by init - is 0	in cfg_parseargs will be changed (fill_cfg, fill_cfg_all)
	char saved;		//by init - 0 and in SaveConfig will be set to 1
};


#define cfg_line(a, b, c)	char a[c];
#define cfg_line1(a, b, c, d)

typedef struct {
#include "../config.h"
	char LEGAL_IP[10][20];
	char LEGAL_MAC[10][30];
} config;

#undef cfg_line
#undef cfg_line1

/*
#define cfg_line(a, b, c)	a,
#define cfg_line1(a, b, c, d)	a ## c,

enum cfg_points {
#include "../config.h"
empt1, //in cfg next 1 lines
END_POINT
};

#undef cfg_line
#undef cfg_line1
*/

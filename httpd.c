/* httpd.c:  A very simple http server
 *
 * Copyright (c) 1997-1999 D. Jeff Dionne <jeff@rt-control.com>
 * Copyright (c) 1998	   Kenneth Albanowski <kjahds@kjahds.com>
 * Copyright (c) 1999	   Nick Brok <nick@nbrok.iaehv.nl>
 * Copyright (c) 2009-2020 Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>
#include <arpa/inet.h>		//inet_ntoa()
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <syslog.h>		//for syslog()
#include "include/httpd.h"
#include "include/httpd_config.h"
#include "copy.h"
#include "parse_CGI.h"
#include "copy_CGI.h"


#define SP_OPTIONS
#ifndef SOCK_STREAM
#define SOCK_STREAM	  2
#endif

#define DEBUG
//#define SHOW_CONSOLE

#undef DEBUG

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define FIRMWARE_BUF_SIZE	1024*80
#define FIRMWARE_4M		    1000*950*4	  // 4M + 512 bytes, 4M for image, 512 bytes for version header
#define LOGOUT_TIME	    180

config CONFIG;
#include "include/config.h"

//add by snop to get the method
int method;
int method1;		//for post data
int log_time=1;
int alrm_counter;

char *postdata;
unsigned long content_length;

char *firmware;
//long firmware_len = 0;
int TIMEOUT=30;


/* Global variables */
char referer[128];
char user_agent[128];
char boundary[1024];
char ip[20];		//is it not the same as incoming_ip ?
char port[10];
char check_ip[20];		//if == '\0'- so release ip.
char dns_name[128];
char etc_save[2];
char buf[16384];
//char *arg;
char *file;
//char os[6];

int wauth;
int fd;
int sockfd;
FILE *fdcr;

char *print200ok_mime = "HTTP/1.0 200 OK\n"
			"Content-type: %s\n"
//			"Server: uClinux-httpd 0.2.2\n"
			"Server: thttpd/2.25b\n"
			"Cache-Control: no-cache,no-store\n"	//new here RAW
			"Expires: 0\n\n";

int DoFile(FILE *f, char *name, char *type){

    FILE * infile;

    if(!(infile = fopen(name, "r"))){
//	alarm(TIMEOUT);
	fprintf(stderr, "Unable to open file %s, %d\n", name, errno);
	fflush(f);
//	alarm(0);
	//return -1;
	return 0;
    }

    fprintf(f, print200ok_mime, type);

    copy(infile,f); /* prints the page */

//    alarm(TIMEOUT);
    fclose(infile);
//    alarm(0);

    return 0;
}

int DoHTML(FILE *f, char *name, char *type){

    FILE *infile;
    struct stat stbuf;
    char *data;

#ifdef NOVAC
    if(!strcmp(name,"close.htm")){
	check_ip[0] = '\0';
    }
#endif
#if defined(LOBOS) || defined(NOVAC)
    if(*check_ip && strcmp(check_ip,ip))
	strcpy(name,"limit-warn.htm");
#endif
    if(stat(name, &stbuf) || (stbuf.st_size>1024*64) || !(infile = fopen(name,"r"))) {
//	alarm(TIMEOUT);
	fprintf(stderr, "Unable to open HTML file %s, %d\n", name, errno);
	fflush(f);
//	alarm(0);
	//return -1;
	return 0;
    }

    data = (char *)malloc(stbuf.st_size+1);
    if(data){
    fread(data,stbuf.st_size,1,infile);
    data[stbuf.st_size] = '\0';
    fprintf(f, print200ok_mime, type);
    //copy(infile,f); /* prints the page */
//  handle_get(infile,f);
    free_page_mem();		//free memory of page includes
    handle_get(data,f);
    free(data);
    } else fprintf(f, "ERR: Unable allocate memory.\n");

//    alarm(TIMEOUT);
    fclose(infile);
//    alarm(0);

    return 0;
}

#ifdef DO_DIR
int DoDir(FILE *f, char *name){

    DIR * dir;
    struct dirent * dirent;
    
    if((dir = opendir(name))== 0){
	fprintf(stderr, "Unable to open directory %s, %d\n", name, errno);
	fflush(f);
	//return -1;
	return 0;
    }

    fprintf(f, print200ok_mime, "text/html");

//    alarm(TIMEOUT);
    fprintf(f, "<html>\n"
		"<head><title>Index of %.80s</title></head>\n"
		"<body bgcolor=\"#99cc99\" text=\"#000000\" link=\"#2020ff\" vlink=\"#4040cc\">\n"
		"<h2>Index of %.80s</h2>\n"
		"<pre>\n"
//		"mode  links  bytes  last-changed  name\n"
		"<hr>", name, name);
//*    alarm(0);

    if(name[strlen(name)-1] != '/'){
	    strcat(name, "/");
    }

    while((dirent = readdir(dir)) != NULL){
//	    alarm(TIMEOUT);
	    fprintf(f, "<a href=\"/%s%s\">%s</a>\n", name, dirent->d_name, dirent->d_name);
//*	    alarm(0);
    }
/*   struct stat stbuf;
    while((dirent = readdir(dir)) != NULL && !stat(dirent->d_name, &stbuf)){
	    alarm(TIMEOUT);
	    fprintf(f, "<a href=\"/%s%s\">%s%s</a>\n", name, dirent->d_name, dirent->d_name, S_ISDIR(stbuf.st_mode) ? "/" : "");
	    alarm(0);
    }
*/
//    alarm(TIMEOUT);				//my insert RAW
    fprintf(f, "</pre></body>\n</html>\n" );	//
//    alarm(0);					//
    closedir(dir);
    return 0;
}
#endif


MIME_TYPES mime[] = {
{".gif", "image/gif", DoFile},
{".png", "image/png", DoFile},
{".jpg", "image/jpeg", DoFile},
{".jpeg", "image/jpeg", DoFile},
{".htm", "text/html", DoHTML},
{".html", "text/html", DoHTML},
{".help", "text/html", DoFile},
{".css", "text/c6ss", DoFile},
{".class", "application/java-vm", DoFile},
{".js", "application/x-javascript", DoFile},
{".txt", "text/plain", DoFile},
{".tgz", "text/plain", DoFile},
{"", "", NULL},
};

//snop adds to decode base64-coded username-password
int base64val(char c){

    int n;
    if (c >= 'A' && c <= 'Z') n = c - 'A';
    else if (c >= 'a' && c <= 'z') n = 26 + c - 'a';
    else if (c >= '0' && c <= '9') n = 52 + c - '0';
    else if (c == '+') n = 62;
    else if (c == '/') n = 63;
    else n = 0;
    return n;
}

void decodebase64(char *string){

    char *src = string, *dest = string;
    unsigned long val;
    while (src[0] != '\0' && src[1] != '\0' && src[2] != '\0'&& src[3] != '\0'){
	val = base64val(src[3])
	    + ( base64val(src[2]) << 6)
	    + ( base64val(src[1]) << 12)
	    + ( base64val(src[0]) << 18);
	if (src[3] == '=') val &= 0xffff00;
	if (src[2] == '=') val &= 0xff0000;
	dest[2] = val & 0xff;
	dest[1] = (val >> 8) & 0xff;
	dest[0] = (val >> 16) & 0xff;
	src += 4;
	dest += 3;
    }
    *dest = '\0';
}

int ParseReq(FILE *f, char *c){

    struct stat stbuf;
    char *r, *r1, *tt;
//    char *file = NULL;

    char *arg = NULL;
//    file = NULL;

#ifdef DEBUG
    printf("req is '%s'\n", c);
#endif

//    while(*(++r) != ' ');  /*skip non-white space*/
//    while(isspace(*r)) r++;
//r = "?fdkfdhgod/sdgjsoidfjdfj.html";
//r = "a";
    r1 = c;
    while(*r1){ 
        if(*r1 == '?'){
	    *r1 = '\0';
	    arg = r1 + 1;
	    break;
	}
	r1++;
    }
    httpd_decode(c);
    tt = check_ticket(c, 0);	//exchange ticket to file name
    if(tt != NULL) c = tt;
    tt = check_ticket(c, 1);	//get this filename throw password free
    if(tt != NULL) c = tt;
    while(*c == '/') c++;
    file = r = c;
    while(*r){ 
	if(*r == '.' && *(r+1) == '/'){
	    fprintf(stderr, "MSG: ./ in string!\n");
	    return 0;
	}
	if(*r == '/'){
	    file = r + 1;
	}
	r++;
    }
/*
     while(*r && (*r != ' ')){
	switch(*r){
	    case '?':	*r = '\0';
			arg = r + 1;
			break;
	    case '/'://	if(!file) c = r + 1;	//    if(!strstr(r,"dir.js"))  //snop adds for showing FTP folder    while (*r == '/') r++;
			//file = r + 1;		//    while(((ptr=strchr(line,'/')) != NULL) && !strstr(line,"dir.js"))  //snop adds for showing FTP folder	line = ptr+1;
			if(arg == NULL) file = r + 1;	//non arg - area
			//else	//arg - area
			break;
	}
	r++;
    }
    *r = '\0';
*/
#ifdef DEBUG
    printf("c='%s', r='%s'\narg='%s' file='%s'\n", c, r, arg, file);
#endif

//printf("WEB_LOGIN: %s\n", CONFIG.WEB_LOGIN);
if(wauth && tt == NULL){
    if(wauth > 3){
	fprintf(f, "HTTP/1.0 404 Not Found\n"
		    "Content-type: text/html\n\n"
		    "<html>\n<head><title>404 Authorisation overflow</title></head>\n"
		    "<body bgcolor=\"#cc9999\" text=\"#000000\" link=\"#2020ff\" vlink=\"#4040cc\">\n"
		    "<h2>Authorisation overflow</h2>\n"
		    "<p>Login error. 3 Times. Please <a href=\"http://%s:%s\">try again</a> in a minute.</p></body>\n</html>\n",CONFIG.IP,CONFIG.ADMIN_PORT);
	return 0;
    }
    if(*(CONFIG.WEB_LOGIN) != '\0'){
//	printf("WEB_LOGIN isnot empty!\n");
	if(!strcmp(file, CONFIG.WEB_LOGIN)){
	    wauth = 0;
	    c = "./index.htm";
	} else {
	    wauth++;
	    c = "./login.htm";
	}
	r = c + strlen(c);
    }
}
//printf("LOGIN:%s\n", c);

//    if(arg) r = arg - 1;	//this is comment because of next string
//    r = httpd_decode(c);	//*file can be moved from origin!

//r - point to the end of file.htm
//c - point to the begin of file with path, without '/'
//arg - point to begin of arguments
//file - point to the begin of only file (used by DoCGI())
//  /path/file.htm?arg1=bla&arg2=blabla
//   |    |       ||
//   c    file   r arg

    switch(method){
	case M_GET:
	    handle_arg(0, arg);
	    break;
	case M_POST:
	    if(method1 == POST_WEB_ENCODED){
		handle_arg(0, postdata);
	    }
	    break;
    }

    if (c[0] == '\0'){	strcat(c,"."); r++;}
#ifdef DEBUG
    printf("c %s\n", c);
#endif

    if(!stat(c, &stbuf)){

	if(tt == NULL){
	if(S_ISDIR(stbuf.st_mode)){	/*OK exist is dir*/
		char buff[3000];	//buff size must be bigger as 2048bytes from *c
	    #if defined(LOBOS) || defined(NOVAC)
		if(*check_ip && strcmp(check_ip,ip))	//if not identical
		    r1 = "limit-warn.htm";
		    //must be return
		else
	    #endif
		{
		    r1 = "index.htm";
//		    r = end + 10;
		}
		r = buff + sprintf(buff, "%s/%s", c, r1);//to parse it after(r - points to end of name)
		c = buff;
		if(stat(c, &stbuf)){	// %dir%/index.htm - such file not exist
#ifdef DO_DIR
		    while(*c){if(*c == '/') r1=c; c++;}
		    *r1 = '\0';
		    DoDir(f,buff);
		    //must be return
#else
		    goto NotFound;
#endif
		}
	}
	}//end of tt == NULL
	/**parsing of supported filetypes**/
	MIME_TYPES *p = mime;
	while(p->ext[0] != '\0'){
	    if(strcmp(p->ext, r - strlen(p->ext)) == 0){
		p->handler(f,c,p->type);
		free_arg(0);
		return 0;
	    }
	    p++;
	}

	printf("UNknown type of file - not supported\n");

    }else{				/*file c not exist*/
	//handle CGI function
	if(!DoCGI(f, file, 0)){//with mime
#ifdef DEBUG
	    printf("CGI c:%s\n",c);
#endif
	    free_par_tmp();//clear all "temp"-parameters
	    free_arg(0);
	    return 0;
	}
    }
NotFound:
//	alarm(TIMEOUT);
	fprintf(f,  "HTTP/1.0 404 Not Found\n"
		    "Content-type: text/html\n\n"
		    "<html>\n<head><title>404 Not Found</title></head>\n"
		    "<body bgcolor=\"#cc9999\" text=\"#000000\" link=\"#2020ff\" vlink=\"#4040cc\">\n"
		    "<h2>404 Not Found</h2>\n"
		    "<p>The requested URL was not found on this server.</p></body>\n</html>\n");
//	alarm(0);
    free_arg(0);
    return 0;
}

int HandleConnect(int fd){

    FILE *f;
    //extend the buffer size (1024->2048)
    //this accomodates Wget.cgi (extra long GET request)
    char buf[2048];
    char buf1[2048];
    char *password, *c, *file_name;
    int agent = 0, auth = 0;

    f = fdopen(fd,"r+");    //FILE *out, LATER
    if(!f){
	fprintf(stderr, "httpd: Unable to open httpd input fd, error %d\n", errno);
//	alarm(TIMEOUT);
	close(fd);
//	alarm(0);
	return 0;
    }
    setbuf(f, 0);

    fdcr = f;
//    alarm(TIMEOUT);
/*
FILE *fp;
if((fp=fopen("/text_","w+")) == NULL) return;

while(fgets(buf, 2048, f)){
fprintf(fp, "%s\n", buf);
printf("printfcopy %d  %s\n", strlen(buf), buf);
}
fclose(fp);
*/
/**Get a method and HTTP version line maxlinesize=2048**/
    if(!fgets(buf, 2048, f)){
	fprintf(stderr, "httpd: Error reading connection, error %d\n", errno);
	fclose(f);
//	alarm(0);
	return 0;
    }
    buf[2047] = '\0';

#ifdef DEBUG
  printf("\nbuf = '%s'\n", buf);
#endif


//    alarm(0);
/*****************************hier must be switch for methods!! */
    method = 0;
    method1 = POST_NO;
//    if(!strncmp(buf,"GET",3))		method = M_GET;
//    else if(!strncmp(buf,"POST",4))	method = M_POST;
    if(file_name = parsestr1(buf, "GET ///././[/!/B/!/*/] HTTP///\\/{C/*.///}/*/<1?\\0/>/E")) method = M_GET;
    else if(file_name = parsestr1(buf, "POST ///././[/!/B/!/*/] HTTP///\\/*?/\\/*.///E")) method = M_POST;
#ifdef SP_OPTIONS
    else if(!strncmp(buf,"OPTIONS",7))	method = M_OPTIONS;
#endif
    else fprintf(stderr, "ERR: Unsupported method\n%s", buf);

    switch (method){
	case M_GET:	
#ifdef DEBUG
		printf("Method GET\n");
#endif
	case M_POST: 

	    referer[0] = '\0';
	    user_agent[0] = '\0';
	    content_length = 0;
	    postdata = NULL;
/**Get other strings from browser antil the postdata (if exist) maxlinesize=2048**/
//	    alarm(TIMEOUT);
#ifdef DEBUG
	    printf("Got buf1:\n");
#endif


	    while(fgets(buf1, 2048, f)){

//		alarm(TIMEOUT);

#ifdef DEBUG
		printf(buf1);
#endif

	//adds for authentication
		if((log_time <= 10) && (c = parsestr1(buf1,"Authorization:/ Basic/ /[/*/]/<1\r\n\\0/>"))){
		    decodebase64(c);
		    password = strchr(c,':');
		    *password = '\0';
		    password++;
#ifdef DEBUG
		    printf("auth_hdr = %s, password = %s\n",c ,password);
#endif
		    if(!strcmp(c, CONFIG.USERNAME) && !strcmp(password, CONFIG.PASSWORD))
			auth = 1;
		    else {
			auth = 0;
			syslog(LOG_AUTHPRIV|LOG_INFO, "Time: %d, IP: %s, USER: %s, PASSWD: %s", log_time, ip, c, password);
			log_time++;
		    }

		}else if(c = parsestr1(buf1, "Refer/,r,er:/ ")){
	            strmycpy(referer, c, 128);
		}else if(c = parsestr1(buf1, "Content-/<1Ll/>ength:/ ")){
		    content_length = atol(c);
		}else if (c = parsestr1(buf1, "Content-/<1Tt/>ype:/ ")){
//printf(buf1);
		    if(parsestr1(c, "application//x-www-form-urlencoded")){	//web_encoded
			method1 = POST_WEB_ENCODED;
		    }else if(c = parsestr1(c, "multipart//form-data;/ ")){		//boundary (data-upload) is comming
			if(c = parsestr1(c, "boundary=/[/*/]/<1\r\n\\0/>")){
			    method1 = POST_BOUNDARY;
			    strmycpy(boundary, c, 1024);
//			    printf(boundary);
			}
		    }
		}else if(c = parsestr1(buf1, "User-Agent:/ ")){    //To identify the OS
	            strmycpy(user_agent, c, 128);
		    agent = 1;
/*		    if(strstr(c,"Linux"))	//used in post parsing only!
	    		strcpy(os,"LINUX");
		    else
		        strcpy(os,"WIN");
*/		} else if ( buf1[0] == '\r' || buf1[0] == '\n' ) break;
//for test alex_raw
//else printf("%s\n", buf1);
//end of test
	}//end of while

/**To accept the data delivered by post**/		/**NEED to make this point to main point for Firmware_upgrade and other functions!!!!! and limit post size!**/
	if((method == M_POST) && (content_length > 1)){
	    if(((method1 == POST_BOUNDARY) && (content_length < 4*1024*1024)) || ((method1 == POST_WEB_ENCODED) && (content_length < 200*1024))){
		postdata = (char *)malloc(content_length+1);
		if(postdata){
		    memset(postdata, 0, content_length+1);
		    fread(postdata,content_length,1,f);			//in copy.c 1 and content_ placechanged!
#ifdef DEBUG
		    printf("\n%s: Line%d - content_length = %d postdata = %s\n",__func__,__LINE__,content_length,postdata);
#endif
		}
	    }
	}

//	alarm(0);

	if(ferror(f)){
	    fprintf(stderr, "http: Error continuing reading connection, error %d\n", errno);
	    if(postdata) free (postdata);
	    fclose(f);
	    close(fd);
	    return 0;
	}

#ifdef DEBUG
	printf("auth = %d, log_time = %d\n",auth,log_time);
#endif

	if(log_time <= 5){
	    if(auth == 0){
		if(agent){	//browser identified
			fprintf(f,"HTTP/1.0 401 Unauthorized\n"
				  "WWW-Authenticate: Basic realm=\"%s\" \n\n"
				  "<html>\n"
				  "<head><title>401 Unauthorized</title></head>\n"
				  "<body><h2>401 Unauthorized</h2>\n"
				  "<p>Access to this resource is denied; your client has not supplied"
				  " the correct authentication.</p></body>\n</html>\n", CONFIG.HOSTNAME);
		}
	    } else {	//authenticated

#ifdef DEBUG
	    printf("come in ParseReq\n");
#endif
	    log_time = 1;
//	printf("auth = %d, log_time = %d\n",auth, log_time);
//	if((auth !=0 && log_time !=1) || strcmp(check_ip,ip))
    	    ParseReq(f, file_name);
	    }
	}//else		need some action for limit access of this IP

	if(postdata) free (postdata);
	break;

#ifdef SP_OPTIONS
	case M_OPTIONS:	
//		alarm(TIMEOUT);
		fprintf(f,"HTTP/1.1 501 Not Implemented\n"
			  "Server: thttpd/2.25b 29dec2003\n"
			  "Content-Type: text/html; charset=\n"
			  "Accept-Ranges: bytes\n"
			  "Connection: close\n"
			  "Cache-Control: no-cache,no-store\n"
			  "<HTML>\n"
			  "<HEAD><TITLE>501 Not Implemented</TITLE></HEAD>\n"
			  "<BODY BGCOLOR=\"#cc9999\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\">\n"
			  "<H2>501 Not Implemented</H2>\n"
			  "The requested method 'OPTIONS' is not Implemented be this server\n"
			  "<HR>\n"
			  "<ADDRESS><A HREF=\"http://www.acme.com/software/thttpd/\">thttpd/2.25b 29dec2003</A></ADDRESS>\n"
			  "</BODY>"
			  "</HTML>\n");
#endif//#ifdef SP_OPTIONS

//	default:
    }//end of switch
    
//    alarm(TIMEOUT);

    fflush(f);
    fclose(f);
    close(fd);	  //To solve error-24
//    alarm(0);

    return 1;
}

void sighup(int signo){
    if(fd != -1) close(fd);
//    close(sockfd);
    ReadConfiguration1();
//    main();
}

void sig_handler(int signo){
    switch (signo){
    case SIGINT:
    case SIGABRT:
    case SIGTERM:
        printf("TERMINATE or ABORT\n");
	break;
//#ifdef DEBUG
    case SIGALRM:
	    alarm(60);
	    alrm_counter++;	//1 counter = 60 sec.
//	    printf("time: %d\n", alrm_counter); 
	    if(alrm_counter == 1){if(log_time > 3) log_time = 1; if(wauth > 3) wauth = 1;}
	    else if(alrm_counter == 5)  chdir(CONFIG.WEB_ROOT);
	    else if(alrm_counter == 10){
		    check_ip[0] = '\0';		//ip is released
		    wauth = 1;
		    free_page_mem();
//		    close(fd);
	    }
	    else if(alrm_counter >= 12) alarm(0);
	    return;
    /* got an alarm, exit & recycle */
/*	printf("Alarm!!\n");
	break;
*/    default: printf("SIgnal %d\n", signo);
//#endif
    }
    free_arg(0);
    free_arg(1);
    free_page_mem();	//clear all memory used for page
    exit(0);
}

int main(int argc, char *argv[]){

    int len;
    int true = 1;
    char *tmp;		//used for ip var.
    struct sockaddr_in ec;
    struct sockaddr_in server_sockaddr;
    struct hostent *hp;
    int opt;
    int server_port = SERVER_PORT;
    etc_save[0] = '0';
    etc_save[1] = '\0';
    check_ip[0] = '\0';		//ip is released
    wauth = 1;			//not authenticated
    char no_dns[] = ".";

    printf("Starting main()!!\n");

    signal(SIGCHLD, SIG_IGN);	//ignore
    signal(SIGPIPE, SIG_IGN);	//ignore
    signal(SIGALRM, sig_handler);	//hang functions on signals

    signal(SIGTERM, sig_handler);	//catch all signals of termination for freeing memory
    signal(SIGABRT, sig_handler);
    signal(SIGKILL, sig_handler);//mayby not used
    signal(SIGINT, sig_handler);
//andy
    signal(SIGHUP, sighup);
//end


    ReadConfiguration1();

//    chdir(HTTPD_DOCUMENT_ROOT);
    chdir(CONFIG.WEB_ROOT);
    
    server_port = atoi(CONFIG.ADMIN_PORT);//get value from config

    while((opt = getopt(argc, argv, "c:p:i")) != -1){
	    switch(opt){
/*		    case 'c':	//opt c with value
				printf("%s\n", optarg);
				exit(0);
*/		    case 'p':	//opt p with value
				server_port = atoi(optarg);
				break;
		    case 'i':	/* I'm running from inetd, handle the request on stdin */
//				fclose(stderr);
//				HandleConnect(0);
				exit(0);
		}
	}
    printf("SERVER_PORT = %d\n", server_port);


    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
	perror("Unable to obtain network");
	exit(1);
    }

    if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&true, sizeof(true))) == -1){
	perror("setsockopt failed");
	exit(1);
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(server_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1){
	perror("Unable to bind socket");
	printf("Unable to bind socket\n");
	exit(1);
    }

    if(listen(sockfd, 8*3) == -1){		/* Arbitrary, 8 files/page, 3 clients */
	perror("Unable to listen");
	exit(4);
    }


    while(1){
	len = sizeof(ec);

	if((fd = accept(sockfd, (void *)&ec, &len)) == -1){
	    close(sockfd);
	    exit(5);
	}else{

	alarm(60);
	alrm_counter = 0;

#ifdef DEBUG
printf("%d == %d\n", ec.sin_addr.s_addr,inet_addr("127.0.0.1"));//important!!!
#endif

	sprintf(port, "%d", ntohs(ec.sin_port));
	port[9] = '\0';

	tmp = (char *)inet_ntoa(ec.sin_addr);		//unsecure
	strmycpy(ip, tmp, 20);

//printf("main ip: %s  --  check_ip: %s\n", ip, check_ip);
	
	if(*check_ip){
	    if(strcmp(check_ip,ip)){		//release ip by writing check_ip='\0'
		close(fd);
		continue;
	    }

	} else {	//check_ip='\0'

	    strcpy(check_ip, ip); //copy IP in check_ip

	hp = gethostbyaddr((caddr_t)&(ec.sin_addr), sizeof(ec.sin_addr), AF_INET);
//printf("addr: %s\n", hp->h_name);
	if(hp){
	    tmp = hp->h_name;
	}else tmp = no_dns;
	strmycpy(dns_name, tmp, 128);

	}

//	printf("dns: %s\n", CONFIG.DNS_PARSE);
	    if(!parsestr1(dns_name, CONFIG.DNS_PARSE)){		//if not parse clean
		close(fd);
		continue;
	    }

/*limit access throw IP and MAC list*/
	if(Get_Security(ip)){
	    close(fd);
	    continue;
	}


	HandleConnect(fd);
	}
    }
}

int Get_Security(char *test_ip){		//Not Used!!! RAW

//    FILE *f1, *f2;
    FILE *f2;
    char LineBuf[256], TagName[15], command[80], test_mac[30];	//test_mac is in develop RAW
    char *ptr, *end;
    int i, j;

	j = atoi(CONFIG.IP_SECURITY_MODE);
	if(j != 0){
	//Check the IP
	//printf("IP_SECURITY_MODE = %d\n",atoi(IP_SECURITY_MODE));

	    j = j - 1;	//1,2, all other values disallow access to server
		for(i=0;i<10;i++){
			if(!strncmp(test_ip,CONFIG.LEGAL_IP[i],strlen(test_ip))){
				//printf("mode = 1: test_ip = %s and LEGAL_IP[i] = %s\n",test_ip,LEGAL_IP[i]);
			    return j;	 //0 - PERMITTED!
			}
		}
		return 1 - j;	 //1 - NOT ALLOWED!!
	}

	//Get the access MAC
	j = atoi(CONFIG.MAC_SECURITY_MODE);
	if(j != 0){
//		sprintf(command,"arp %s > /var/run/mac",test_ip);// /proc/net/arp
//		system(command);
		i = 0;
		if((f2 = fopen("/proc/net/arp","r")) == NULL)
		    return 0;
		else{
		    while(fgets(LineBuf,255,f2) != NULL){
			if((ptr = strstr(LineBuf,test_ip)) != NULL){
/*				ptr = strchr(LineBuf,'*');
				ptr++;
				end = strchr(ptr,' ');
				*end = '\0';
				strcpy(test_mac,ptr);
*/			i = 1;
			break;
			}
		    }
		    fclose(f2);
		    if(i==0) return 0;		//test_ip not found in arp file
		}
	
		
	//printf("Line 915: test_mac = %s\n",test_mac);
	//printf("MAC_SECURITY_MODE = %d\n",atoi(MAC_SECURITY_MODE));
	//Check the MAC
	    j = j - 1;
		for(i=0;i<10;i++){
			if(strstr(LineBuf, CONFIG.LEGAL_MAC[i])){
			    return j;	 //PERMITTED!!
			}
		}
		return 1 - j;    //NOT ALLOWED!!
	}
	//printf("Line 938\n");
	return 0;
}

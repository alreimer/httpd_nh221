/*
 *
 * Copyright (C) 2009-2012  Alexander Reimer <alex_raw@rambler.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
//written by snop to handle CGI function
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include "include/httpd_sysconf.h"
#include "include/httpd.h"
#include "include/httpd_config.h"
#include "copy.h"
#include "copy_CGI.h"
#include "parse_CGI.h"

//=========== Define Customer Name ===========
//#define NH230
//#undef NH230	//define NH221 nonbrand model
//============================================
// Set_webserver.cgi
#ifdef DEBUG
#define MSG(string,args...) fprintf(stderr,string,##args)
#else
#define MSG(string,args...)
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))
//===========debug switch===========
#define DEBUG1
#undef DEBUG1
//==================================

#define MAX_TORRENT  10 //Set_webserver.cgi
#define DEBUG
#define SHOW_CONSOLE
#define END_MARK	    "iamsnop0413andy0805"
#define MAX_USER			50
#define MAX_USERNAME	    50
#define MAX_FOLDER			100
#define MAX_GROUP	    50
#define MAX_NAME			17
#define MAX_PASSWD			17
#define MAX_DESCHAR		51
#define MAX_URL				100
#define MAX_BUF				100
#define READ_SHORT			256
#define READ_LONG			1024
#define MAX_COMMAND			150
#define MAX_FULL_URL		150
#define FTP_SHARE			"ftpshare"
#define SMB_SHARE			"Share"
#define BARCOLOR			"3A48FF"
#define KILOBYTE	    1000
#define MEGABYTE	    KILOBYTE*1000
#define GIGABYTE	    MEGABYTE*1000
/*
#define KILOBYTE	    1024
#define MEGABYTE	    KILOBYTE*1024
#define GIGABYTE	    MEGABYTE*1024
*/


#if defined(LOBOS) || defined(CENTURY)
    #define LONG_LEN			 50
#else
    #define LONG_LEN			 30
#endif

#undef DEBUG
#undef SHOW_CONSOLE

char hdused[LONG_LEN+1];
#ifdef NOVAC
    static char HD_SIZE[LONG_LEN+1];
    static char HD_FREE[LONG_LEN+1];
    static char HD_NAME[256];
#endif

extern unsigned long content_length;
//extern int real_length;
extern char *postdata;

static int old_smbd_num;
static int old_ftpd_num;
int new_smbd_num;
int new_ftpd_num;
int num_agent;	//allen adds to save num variable for transfer global variable
char deviceformat[2];  //allen adds to recieve the device 1, device 2 or both
GROUP_VALUE GROUP_set[MAX_GROUP];
GROUP_VALUE NEW_GROUP;
MOUNT_VALUE MOUNT_set[36];
char DEL_GROUP[MAX_GROUP][17];
int linux_size,root_size,ramdisk2_size;

int open_web, open_ntx;	 //allen adds to adjudge the fm_fail & ntx_fail successful or failure
char ntx_firmware[50], web_firmware[50];
int page_global, initial;
char URL_global[256];		//do we not have it allready in CFG? RAW

//link to parse.c
int hdcapacity;

extern config CONFIG;
//extern struct cfg_parse cfg[];

extern char *firmware;
extern unsigned int firmware_len, firmware_size;
//for my_system
extern int fd;
extern int sockfd;

void my_system(FILE *out, char *cmd);
void boot_page(FILE *out, char *filename);
#undef USB
#ifdef USB
#include "httpd_rc_usb.c"
#endif

void get_hdcapacity(void);
void get_hdused( void );

void Set_network(FILE *out);
int  Cmp_user( char *username);
int  Set_addinvaliduser( char *user_name );
int  Cmp_folder( char *foldername);

void Set_hdformat_m(FILE *out);

void Get_allfolder(FILE *out);
void Get_allfolder_m( unsigned char *count, FOLDER_VALUE *folder_name );
int Get_alluser(FILE *out);
int Get_alluserlist( USER_VALUE *folder_user );
void Get_userinfo(FILE *out);
int Get_userinfo_m(char *user_name, char *user_des, char *user_folder);
void Get_folderuser(FILE *out);
void Get_folderuser_m( char *folder_name, FILE *out);
void Get_folder_user_all( char *folder_name, USER_VALUE *folder_user, char *flag);
void print_user_group( char *folder_name, USER_VALUE *user_list, int num, FILE *out);

void Ch_userinfo(FILE *out);
#ifdef NOVAC
    int	 Ch_userinfo_m(char *user_name, char *user_des);
#else
    int	 Ch_userinfo_m(char *user_name, char *user_des, char *ftp_user, char *ftp_folder, char *ftp_admin);
#endif
void Ch_username(FILE *out);
void Ch_username_m(FILE *out);
void find_ch_username(char *ptr, char *username, char *n_username, FILE *n_fd);
void Ch_foldername(FILE *out);
void Ch_foldername_m(FILE *out);
void find_mntfolder(FILE *out);
int  find_mntfolder_m( char *count, FOLDER_VALUE *hd_folder);
void Set_addmntfolder(FILE *out);
void Ch_ftppasswd(FILE *out);
//void Set_FTP(FILE *out);	//snop adds for FTP Configuration

void Error_page( FILE *out, char *go_url);
void SYSTEM(char *cmd);

//void Restart_System(FILE *out);
//void Factory_default(FILE *out);
int parse_post_data(char *postdata, USER_VALUE *post_user);
//void Firmware_upgrade(FILE *out);
void parse_ufile_data( char *ufile );
//int  write_upload_file( char *que, char *fpath_name, FILE *out);
void *memmem_in(char *haystack, long haystack_len, char *needle, long *i);
int  check_firmware_version( char *check_buf );
void parse_deluser_postdate(int *count, USER_VALUE *deluser, char *post_data);
int  deluser_to_passwd( int count, USER_VALUE *deluser);

void Wait_page(FILE *out, int second, char *url);
void Upgrade_page(FILE *out, char *ip);
//======Firmware upgrade======
void Upgrade_page1(FILE *out);
void Reboot_page1(FILE *out, char *filename);

#define FIRMWAREUP_DEBUG
#undef FIRMWAREUP_DEBUG
#if	 defined(FIRMWAREUP_DEBUG)
# define  firmwareup_debug(fmt,arg...)		       printf(fmt,##arg)
#else
# define  firmwareup_debug(format, args...)
#endif
//============================
void upgrade_error( FILE *out );
void Goto_page( FILE *out, char *url, char *type, char *folder_name );
void JavaReturn_page( FILE *out, int second, char *ip , char *page);
void Systembusy_page( FILE *out, char *go_url );
void Warmming_page( FILE *out, char *message, char *go_url);
void warmming_specialname( FILE *out, char *username, char *go_url);
//================ declare the uplaod and download cgi ================
//void Set_Conf(FILE *out);
//=====================================================================
//================ ntx_update and web_update ==========================
void Set_ntx_web_update(FILE *out);
//=====================================================================
//void Set_codeset(FILE *out);
void name_encode(char *decode_string, char *orig_string);
void IPSecurity(FILE *out);
void MACSecurity(FILE *out);

//===========Character set file============
//void Set_codeset1(FILE *out);
//=========================================
//===============Service_Number for option iterm================
//void Set_service(FILE *out);
//==========================================
void Add_Group(FILE *out);
void Get_user_description(char *user_name, char *description);
void Get_allgroup(FILE *out);
void Set_Group(FILE *out);
void Ch_group(FILE *out);
//sp/230/-----------------
char * get_unicode(void);
void name_decode(char *string);
void find_del_group(char *ptr, int count, FILE *fo);


void ftp_setup(FILE *out); //Dean: transform ftp-setup.htm to cgi function for better control
//=============Set Code Page==============
//void Set_codepage(FILE *out);
//==========================================
WAN_VALUE WAN_set;
LAN_VALUE LAN_set;
SUPERUSER_VALUE SUPERUSER_set;

CGI_ENTRY cgi_entries[] = {
    {"_show_CGIs.cgi",show_CGIs},		//used from copy_CGI.c
/*    {"Set_network.cgi",Set_network},
    {"Get_alluser.cgi",Get_alluser},
    {"Get_allfolder.cgi",Get_allfolder},
    {"Get_folderuser.cgi",Get_folderuser},
    {"Get_userinfo.cgi",Get_userinfo},		//not used in web, used from another cgi
    {"Ch_username.cgi",Ch_username},
    {"Ch_username_m.cgi",Ch_username_m},
    {"Ch_foldername.cgi",Ch_foldername},	//not used in web
    {"Ch_foldername_m.cgi",Ch_foldername_m},	//not used in web
    {"find_mntfolder.cgi",find_mntfolder},
    {"Ch_userinfo.cgi",Ch_userinfo},		//not used in web
    {"Set_addmntfolder.cgi",Set_addmntfolder},
//    {"Restart_System.cgi",Restart_System},
//    {"Factory_default.cgi",Factory_default},
//    {"Firmware_upgrade.cgi",Firmware_upgrade},
    {"Ch_ftppasswd.cgi",Ch_ftppasswd},
    {"Set_hdformat_m.cgi",Set_hdformat_m},
//    {"Set_FTP.cgi",Set_FTP},
//    {"Set_codeset.cgi",Set_codeset},
//************************************************* is in rc_usb.c ****************************
#ifdef USB
*/
//======= CGI Function for USB by snop===============
//********    {"_lsusb_table",lsusb_table},
/*    {"Show_USB_Disk.cgi",Show_USB_Disk},
    {"Set_USB_Disk.cgi",Set_USB_Disk},
    //{"Set_usbdisk.cgi",Set_usbdisk},
//============================================
//======= CGI Function for USB by allen=================
    {"Show_USB_Printer.cgi",Show_USB_Printer},
    {"Set_printer.cgi",Set_printer},
    {"Set_usbque.cgi",Set_usbque},
    {"Printer_scrpage.cgi",Printer_scrpage},
//==============================================
//=============== Set usbadvanced ===============
    //{"Set_usbadvanced.cgi",Set_usbadvanced},
    {"Set_usbhdformat.cgi",Set_usbhdformat},
    {"Set_usbhdformat_m.cgi",Set_usbhdformat_m},
//===============================================
//=================CGI function for Clean Print Job===================
    {"Show_advanced_func1.cgi",Show_advanced_func1},
//=============== copy button ===============
    {"Set_usbcopy.cgi",Set_usbcopy},
//==========================================
#endif
//**********************************************************************************************

//    {"Set_codeset1.cgi",Set_codeset1},
//============== ntx_update & web_update =================
    {"Set_ntx_web_update.cgi",Set_ntx_web_update},
//========================================================
//============Firmware upgrade============
    {"Upgrade_page1.cgi",Upgrade_page1},
//========================================
//================ declare the upload and download cgi ================
//    {"Set_Conf.cgi",Set_Conf},
//=====================================================================
    {"Set_ipsecurity.cgi",IPSecurity},
    {"Set_macsecurity.cgi",MACSecurity},
//======= CGI Function for Group =============
    {"Add_Group.cgi",Add_Group},
    {"Get_allgroup.cgi",Get_allgroup},
    {"Set_Group.cgi",Set_Group},
    {"Ch_group.cgi",Ch_group},
/****************** moved to rc_maintain ********************************************
//=============== Syslog ===============
//    {"Show_syslog.cgi",Show_syslog},
//==========================================
//************************************************************************************
    {"ftp_setup.cgi",ftp_setup},
//================ Samba ===============
//    {"samba.cgi",samba},
//============== Dropbear ===============
//    {"Set_dropbear.cgi", Set_dropbear},
*/    {"",NULL}
};

void Init_Http_Var(void)
{
//    char ch_download[10]="disabled";

    ReadConfiguration();

	get_hdcapacity();
}


void get_hdcapacity(void){	//checks if hd is mounted to /mnt	/*total bullsheet. RAW*/

}

void get_hdused( void ){

	char Line_buf[255];
	char *ptr,*end;
	long hd_g=0;
	float hd_u=0;
#ifdef NOVAC
    long hd_free=0;
#endif
	int i;
	char n = 0;
	struct statfs stbuf;

	switch(hdcapacity){
	    case 0: sprintf( hdused, "No HD Found" );
		    return;
	    case 1: sprintf( hdused, "HD Unable to use" );
		    return;
	    case 3: sprintf(hdused,"Fail to mount HD");
		    return;
	}


    if (statfs(MOUNT_POINT, &stbuf) == -1) {
		printf("Unable to get disk space of /mnt: %s\n", strerror(errno));
		exit(0);
	}

	sprintf(Line_buf,"%9ld %9ld \n",(long) (stbuf.f_blocks * (stbuf.f_bsize / (double)KILOBYTE)),
			    (long) (stbuf.f_bavail * (stbuf.f_bsize / (double)KILOBYTE)));
	if(Line_buf != NULL){
		for(i=0; i<255; i++){
			if ( Line_buf[i] != ' ' ){
				if (!n){
					ptr = &Line_buf[i];
					end = strchr(ptr, ' ');
					if ( end != NULL )
						*end = '\0';
					hd_g = atoi(ptr) ;
					i = i + strlen(ptr);
					n = 1;
				}else{
					ptr = &Line_buf[i];
					end = strchr(ptr, ' ');
					if ( end != NULL )
						*end = '\0';
					hd_u = atoi(ptr) ;
			#ifdef NOVAC
			    hd_free = hd_u;
			#endif
					i = 255;
				}
			}
	    }
	}

    //snop modifies to show the HD usage no matter how it is
	/*if ( (float)(((float)hd_g-(float)hd_u)/(float)hd_g) < 0.01 ){
		hd_u = 0.00;
	}else{
		hd_u = (float)(((float)hd_g-(float)hd_u)/(float)hd_g);
	}*/

    hd_u = (float)(((float)hd_g-(float)hd_u)/(float)hd_g);
	memset( hdused, 0, sizeof(hdused) );
	sprintf( hdused, " %.2f %% ", hd_u*(float)100 );    //snop modifies by multiplying 100

	i = strlen(hdused);
	if ((hd_g / KILOBYTE ) < 1000 ){
		sprintf( &hdused[i], " %ld MB", hd_g/KILOBYTE );
	}else{
		sprintf( &hdused[i], " %ld GB", hd_g/KILOBYTE/KILOBYTE );
	}
	hdused[30]='\0';
#ifdef NOVAC
    memset(HD_SIZE, 0, sizeof(HD_SIZE));
    if ((hd_g / KILOBYTE ) < 1000 ){
		sprintf( HD_SIZE, "%ld MB", hd_g/KILOBYTE );
	}else{
		sprintf( HD_SIZE, "%ld GB", hd_g/KILOBYTE/KILOBYTE );
	}
	HD_SIZE[31]='\0';
	memset(HD_FREE, 0, sizeof(HD_FREE));
	if ((hd_free / KILOBYTE ) < 1000 ){
		sprintf( HD_FREE, "%ld MB free", hd_free/KILOBYTE );
	}else{
		sprintf( HD_FREE, "%ld GB free", hd_free/KILOBYTE/KILOBYTE );
	}
	HD_FREE[31]='\0';
#endif
	//printf("hdused in get_hdused= %s\n",hdused);
	return;
}

int Cmp_user( char *username){

	USER_VALUE cmp_user[MAX_USER];
	int i, num = 0;

	memset( cmp_user, 0, sizeof(USER_VALUE)*MAX_USER );

	num = Get_alluserlist( cmp_user );

	for(i=0; i<MAX_USER; i++){
	    if(!strlen(&(cmp_user[i].username[0])))
		break;
		if(!strcmp(&(cmp_user[i].username[0]),username))
		    return 0;
  }
	return 1;
}

int  Set_addinvaliduser( char *user_name )
{
	FILE *fp;
	FILE *n_fd;
	char Line_b[READ_LONG];
	char *ptr,*end;

	if((fp = fopen("/etc/smb.conf","r")) == NULL){
		printf("Couldn't open smb.conf to read\n");
	return 0;
    }
	if((n_fd = fopen("/etc/smb1.conf","w+")) == NULL){
		printf("Couldn't open smb1.conf to write\n");
	    return 0;
	}

    while(fgets(Line_b, READ_LONG-1, fp) != NULL)
    {
		if ( strstr(Line_b, "Invalid users =") != NULL)
		{
			fprintf(n_fd, "	  Invalid users = %s", user_name);
			end = strchr( Line_b, '=');
			end += 2;
			ptr = end;
			if ( *ptr == '\r' || *ptr == '\n')
				fprintf(n_fd, "\n");
			else{
			    if(strchr(ptr,'\n'))
				    fprintf(n_fd, ",%s", ptr);	  //snop modifies to make SP acceptable
				else
				    fprintf(n_fd, ",%s\n", ptr);  //snop modifies to make SP acceptable
			}

			fflush(n_fd);
		}
		else
		{
			fputs(Line_b, n_fd);
		}
	}
	fclose(fp);
    fclose(n_fd);
	system("/bin/mv /etc/smb1.conf /etc/smb.conf");
	//my_system("/bin/mv /etc/smb1.conf /etc/smb.conf");

    return 1;
}

int  Cmp_folder( char *foldername)
{
	FOLDER_VALUE cmp_folder[MAX_FOLDER];
	unsigned char count = 0;
	int i;

	memset( cmp_folder, 0, sizeof(FOLDER_VALUE) * MAX_FOLDER );

	Get_allfolder_m( &count , cmp_folder );

#ifdef NH220
	if(!strcmp(foldername,"~!@#$%^george"))	   //this folder name is for FTP to change directory
	    return 0;
#endif

	for (i=0; i<count; i++)
	{
		if ( !strlen( &(cmp_folder[i].foldername[0]) ) )  break;
		if ( !strcasecmp( &(cmp_folder[i].foldername[0]) , foldername) )	return 0;
	}

	return 1;
}


void Set_hdformat_m(FILE *out){

		FILE *fp;
		char Line_buf[READ_SHORT+1],*data1;
		int i;
		int num=0;
		char num2[5], data_tmp[20];

    ARGUMENT args[]={
	{"percent", num2, 4},
	{NULL,NULL,0}
    };
    parseargs(args);

#ifdef EXT3
	if ((fp = fopen("/etc/percent","r")) == NULL )
#else
	if ((fp = fopen("/etc/mkdos.percent","r")) == NULL )
#endif
		num = atoi(num2) ;
	else{
#ifdef EXT3
		if( atoi(num2) == 200 ){
#else
		if( atoi(num2) == 100 ){
#endif
      //printf("!!!!!!Starting Restart_System();!!!!!!\n");
			//Restart_System(out);
			//printf("back to Set_hdformat_m.cgi\n");
			return;
		}

		fgets( Line_buf, READ_SHORT, fp );

		for(i=0; i<READ_SHORT-1; i++){
			if( Line_buf[i] == '\r' || Line_buf[i] == '\n'){
				Line_buf[i] = '\0' ;
				i = READ_SHORT;
			}
		}

		num = atoi(Line_buf);

	    fclose(fp);
	}
     if((fp=fopen("/htdocs/JavaReturn_h.htm","r")) == NULL)
	return;
     else{
	while(fgets(Line_buf,READ_SHORT,fp) != NULL)
	    fprintf(out,"%s",Line_buf);
	fclose(fp);
    }

       fprintf(out,"<td width=\"100%%\" bgcolor=\"#FFFFFF\">\n<font size=3>&nbsp;&nbsp;Format HD</font><br><br>\n"
       "&nbsp;&nbsp;This may take a few minutes.<br>&nbsp;&nbsp;Please wait for the system to reload the page. Do not close this window.<br>\n"
       "</td>\n</tr>\n<tr height=\"005\"><td></td></tr>\n</table><br>\n"
       "<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");

	for(i=0; i<20; i++){
		if( i < (num/5) )
			fprintf(out, "<td width=4%% bgcolor=\"3A48FF\">&nbsp;</td>");
		else
			fprintf(out, "<td width=4%% bgcolor=\"AEAEAE\">&nbsp;</td>");
	}

	fprintf(out,"</tr><tr><td width=4%%>0%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>100%%</td></tr></table>");

	fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",5000);\nreturn true;}\nfunction reloadurl(){location.replace('/Set_hdformat_m.cgi?percent=%d');}</script><br></body></html>\n", num);

	return ;
}

void Get_allfolder(FILE *out){

		  //NH230 Nonebrand
	    FILE *fp;
	    char LineBuf[READ_SHORT];
	    FOLDER_VALUE folder_name[MAX_FOLDER];
	    unsigned char folder_count = 0;
	    int i, mount_count;
	    char *bg;

	    mount_count = 0;//check_mount();
	    printf("mount_count = %d\n", mount_count);

	    for(i=0;i<mount_count;i++){
	      if(strlen(MOUNT_set[i].folder_name) > 0)
		printf("MOUNT_set[%d].folder_name = %s\n", i, MOUNT_set[i].folder_name);
	    }

	    memset( folder_name, 0, sizeof(FOLDER_VALUE) * MAX_FOLDER );
	    get_hdcapacity();
if(hdcapacity !=2){//if USB Disk is not found
	fprintf(out,"<html><head><title>Gigabit SOHO NAS</title></head>\n"
	"<body bgColor=#ffffff>\n"
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
	"<P><FONT class=style9>&nbsp;&nbsp;Folder Configuration</FONT></P>\n"
	"<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
	"<TBODY><TR><TD width=\"100%%\" bgColor=#ffffff height=31>\n"
	"<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/dsk_fm.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
	"<TR height=5>\n<TD></TD></TR></TBODY></TABLE>\n"
	"<P><FONT class=style8>&nbsp;&nbsp;USB Disk not found</FONT></P>\n"
	"<P>&nbsp;&nbsp;Folder and Disk format function only activate when USB disk is present</P></body></html>\n");
}
else{
	Get_allfolder_m( &folder_count, folder_name);

	fprintf(out,"<html><head><title>Gigabit SOHO NAS</title>\n"
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
	"<script language=\"JavaScript\">\n"
	"function check_hd(){\n"
	"if(document.form1.hdcapacity.value != 2){\n"
	"	 document.form1.apply.disabled=true;\n}\n}\n</script>\n"
	"</head><body bgColor=#ffffff onload=\"check_hd()\">\n"
	"<P><FONT class=style9>&nbsp;&nbsp;Folder Configuration</FONT></P>\n"
	"<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
	"<TBODY><TR><TD width=\"100%%\" bgColor=#ffffff height=31>\n"
	"<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/dsk_fm.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
	"<TR height=5>\n<TD></TD></TR></TBODY></TABLE>\n");
	//fprintf(out,"<table border=\"0\"><tr><td bgColor=#333399 width=\"802\">\n");
	//fprintf(out,"<B><FONT class=\"style2\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;¤L | tH?_[</FONT></B><br></td></tr></table><br>\n");

	fprintf(out,"<table border=\"0\" width=\"550\">\n");
	fprintf(out, "<form name=form1 method=\"GET\" action=/Set_delfolder.cgi onsubmit=\"return double_check()\"> ");
	fprintf(out,"<input type=\"hidden\" name=\"hdcapacity\" value=\"%d\">\n",hdcapacity);
	if(folder_count != 0){
	    for(i=0; i<folder_count; i++){
		if( i % 2 == 0){
						bg = "#ff0000";
			}else{
			    bg = "#FFFFFF";
			}
			fprintf(out, "<tr height=60><td width=\"30\" align=\"center\" bgcolor=\"%s\">\n",bg);
			//if(strcasecmp( &(folder_name[i].foldername[0]), "Share" ) != 0 )  //Share could be deleted!!
				fprintf(out,"<input type=checkbox name=\"del_f\" value=\"%s\" OnClick=\"box_change(this.checked)\"></td>",&(folder_name[i].foldername[0]));
				fprintf(out, "<td width=\"100\" align=\"center\" bgcolor=\"%s\"><a href=\"Ch_folderuser_select.cgi?name=%s&Chfolder=D\"><img src=\"images/folder-s.gif\" border=0></a></td><td width=\"150\" align=\"Left\" bgcolor=\"%s\"><a href=\"Ch_folderuser_select.cgi?name=%s&Chfolder=D\"><font color=\"#2222FF\">&nbsp;&nbsp;%s</font></a></td><td width=\"270\" align=\"Left\" bgcolor=\"%s\">&nbsp;&nbsp;%s</td>\n", bg, &(folder_name[i].foldername[0]), bg, &(folder_name[i].foldername[0]), &(folder_name[i].foldername[0]), bg, &(folder_name[i].descrip[0]) );
		}
	    }
		fprintf(out,"</tr></table><br><table border=\"0\" width=\"30%%\" align=\"center\"><tr>\n");

	 //===== allen adds to defense the delete folder event when webserver is not disable =====
		fprintf(out,"<input type=hidden name=config_webenable value=%s>\n",CONFIG.WEB_ENABLE);
		fprintf(out,"<input type=hidden name=config_webfolder value=%s>\n",CONFIG.WEB_FOLDER);
   //=======================================================================================
		//===========================allen adds hidden object for USB mount===========================
		for(i=0;i<4;i++)
		  fprintf(out,"<input type=hidden name=usbfolder%d value=%s>\n", i, MOUNT_set[i].folder_name);
		//============================================================================================
		if(folder_count == MAX_FOLDER)
		   fprintf(out,"<table border=\"0\" width=\"550\"><tr><td width=\"100\"><br></td><td width=\"100\"><input type=button value=\"Add\" name=\"apply\" onClick=\"check_num();\"></td><td width=\"100\"><input type=submit name=\"del\" value=\"Delete\" disabled></td><td width=\"250\"><input type=button value=\"Back\" onClick=\"history.back()\"></td></tr>\n");
		else
		fprintf(out,"<table border=\"0\" width=\"550\"><tr><td width=\"100\"><br></td><td width=\"100\"><input type=button value=\"Add\" name=\"apply\" onClick=\"location.replace('folder_add.htm')\"></td><td width=\"100\"><input type=submit name=\"del\" value=\"Delete\" disabled></td><td width=\"250\"><input type=button value=\"Back\" onClick=\"history.back()\"></td></tr>\n");
		fprintf(out,"<tr>\n<td width=\"100\" valign=top align=right><b>Note:&nbsp;&nbsp;</b></td>\n<td width=\"450\" align=left colspan=3 class=\"style4\">\n");
		fprintf(out,"Disk function tab only activates when USB disk is present.<br>\n");
		fprintf(out,"Click \"Add\" to add a new folder.<br>\n");
		fprintf(out,"Click \"Delete\" to remove the folder you select from the list.<br>\n");
		fprintf(out,"Click the folder icon to change the folder name, description and user authority.</td></tr></form></table>\n");
		fprintf(out, "<MAP name=Map>\n");
		fprintf(out,"<AREA shape=RECT coords=10,1,109,23 href=\"Get_allfolder.cgi\">\n");

		/*
		if(hdcapacity!=2)//if usb disk is found
			fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"disk-formathd.html\">\n");
		fprintf(out,"</MAP>\n");
		*/

		fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"disk-formathd.html\"></MAP>\n");

		fprintf(out,"</body></html>\n");
		fprintf(out,"<script language=\"JavaScript\">var num=0;function box_change(){if (arguments[0] == true){num++;}else if (arguments[0] == false) {num--;}if (num==0) {document.form1.del.disabled=true;}else {document.form1.del.disabled=false;}} \n");
		fprintf(out,"function double_check(){\n");

		//=====allen adds hidden object for USB mount=====
		 if(folder_count == 1){
				  fprintf(out,"ftp0=document.form1.config_webenable.value;\n");
					fprintf(out,"ftp1=document.form1.config_webfolder.value;\n");
			  for(i=0;i<4;i++)
				  fprintf(out," cmp%d=document.form1.usbfolder%d.value;\n", i, i);
				  fprintf(out,"un0=document.form1.del_f.value;\n");
					fprintf(out,"chk0=document.form1.del_f.checked;\n");

				  fprintf(out,"if(ftp0 == 1){\n");
				  fprintf(out," if((un0 == ftp1 && chk0== true) || (un0 == ftp1 && chk0== true)\n");
					fprintf(out,"	|| (un0 == ftp1 && chk0== true) || (un0 == ftp1 && chk0== true)){\n");
					fprintf(out,"	alert(\"You must be setted webserver disabled in advance when excute the delete folder event!!\");\n");
					fprintf(out,"	return false;\n");
					fprintf(out,"	}\n");
					fprintf(out,"}\n");

					fprintf(out,"if((un0 == cmp0 && chk%d== true) || (un0 == cmp1 && chk%d== true) \n");
					fprintf(out,"|| (un0 == cmp2 && chk%d== true) || (un0 == cmp3 && chk%d== true)){\n");
					fprintf(out,"alert(\"You must be umount the folder in advance when excute the delete folder event!!\");\n");
					fprintf(out,"return false;\n");
					fprintf(out,"}\n");
		 }
		 else{
		     fprintf(out,"ftp0=document.form1.config_webenable.value;\n");
				 fprintf(out,"ftp1=document.form1.config_webfolder.value;\n");
				 for(i=0;i<4;i++)
				  fprintf(out," cmp%d=document.form1.usbfolder%d.value;\n", i, i);
				 for(i=0;i<folder_count;i++){
					 fprintf(out,"un%d=document.form1.del_f[%d].value;\n", i, i);
					 fprintf(out,"chk%d=document.form1.del_f[%d].checked;\n", i, i);

					  fprintf(out,"if(ftp0 == 1){\n");
					  fprintf(out," if((un%d == ftp1 && chk%d== true) || (un%d == ftp1 && chk%d== true)\n", i, i, i, i);
						fprintf(out,"	|| (un%d == ftp1 && chk%d== true) || (un%d == ftp1 && chk%d== true)){\n", i, i, i, i);
						fprintf(out,"	alert(\"You must be setted webserver disabled in advance when excute the delete folder event!!\");\n");
						fprintf(out,"	return false;\n");
						fprintf(out,"	}\n");
					  fprintf(out,"}\n");

						 fprintf(out,"if((un%d == cmp0 && chk%d== true) || (un%d == cmp1 && chk%d== true) \n", i, i, i, i);
						 fprintf(out,"|| (un%d == cmp2 && chk%d== true) || (un%d == cmp3 && chk%d== true)){\n", i, i, i, i);
						  fprintf(out,"alert(\"You must be umount the folder in advance when excute the delete folder event!!\");\n");
						  fprintf(out,"return false;\n");
						 fprintf(out,"}\n");
			   }
	   }
	  //================================================
		fprintf(out,"flag=confirm(\"Are you sure you want to delete these folders?\");if(flag){return true;}else{return false;}};\n");
		fprintf(out,"function check_num(){\n	alert(\"The number of folders is over the limit!\");\n	  document.form1.apply.disabled=true;\n}</script>\n");

		return;
}
}


void Get_allfolder_m( unsigned char *count, FOLDER_VALUE *folder_name ){
	return;
}

void Get_folderuser(FILE *out){

	    char folder_name[MAX_NAME+1];
	    FILE * fp;
	    char LineBuf[READ_SHORT];
	    int kind = 0;
	    char folder_des[MAX_DESCHAR+1];

	    ARGUMENT args[]={
		{"name", folder_name, MAX_NAME},
		{NULL,NULL,0}
	    };
	    parseargs(args);
	    memset( folder_des, 0, sizeof(folder_des) );
	    fprintf(out,"<html><head><title>Folder Configuration</title>\n"
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
			"<script language=\"JavaScript\">\n"
			"function check_hd(){\n"
			"if(document.form1.hdcapacity.value != 2){\n"
			"document.form1.apply.disabled=true;\n}\n}\n"
			"</script></head><body onload=\"check_hd()\">\n"
			"<P><FONT class=style9>Folder Configuration</FONT></P>\n"
				    "<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
				    "<TBODY><TR><TD width=\"100%%\" bgColor=#ffffff height=31>\n"
				    "<DIV align=left><IMG src=\"images/dsk_fm.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
				    "<TR height=5>\n<TD></TD></TR></TBODY></TABLE>\n"
				    "<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1>\n"
				    "<TBODY>\n<TR bgColor=#ff0000>\n<TD class=style2 colSpan=3 height=30>&nbsp;&nbsp;Folder Configuration</TD></TR>\n");
//***********			Get_folderinfo_m( folder_name, folder_des);
			fprintf(out,"<form name=\"form1\">\n");
			fprintf(out,"<input type=\"hidden\" name=\"hdcapacity\" value=\"%d\">\n",hdcapacity);
	    fprintf(out,"<tr>\n<td width=\"50\" align=\"center\" rowspan=3><img src=\"images/folder-s.gif\" border=0></td>\n"
			"<th width=\"150\" bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Folder Name</th>\n"
			"<td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;%s</td></tr>\n",folder_name);
	    fprintf(out,"<tr>\n<th bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Description</th>\n"
			"<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;%s</td></tr>\n",folder_des);
	    fprintf(out,"<TR bgColor=#ff0000>\n<TD class=style2 height=30>&nbsp;&nbsp;</TD>\n<TD height=30>\n"
			"&nbsp;&nbsp;<input type=button name=apply value=\"Change\" onClick=\"location.replace('Ch_folderuser_select.cgi?name=%s&Chfolder=D')\">\n",folder_name);
	    fprintf(out,"&nbsp;&nbsp;<input type=button name=back value=\"Back\" onClick=\"location.replace('Get_allfolder.cgi')\"></TD></tr></form>\n"
			"<tr bgColor=#FFFFFF>\n<td colspan=3>\n<table width=\"546\">\n"
			"<TR bgColor=#DDDDDD><TD height=30 colspan=6>&nbsp;&nbsp;Group</TD>\n</tr>\n"
			"<MAP name=Map><AREA shape=RECT coords=10,1,109,23 href=\"Get_allfolder.cgi\">\n"
    		"<AREA shape=RECT coords=110,1,209,23 href=\"disk-formathd.html\"></MAP>\n");
//*********			Get_folderkind_m( &kind, folder_name);
			//printf("kind = %d\n",kind);
		if( kind == 3 ){
		    Get_folderuser_m( folder_name, out);
		}else if( kind == 1 ){
			fprintf(out,"</td></tr></table><img src=\"images/user.gif\" border=0><b>All user are Read Only!<br><br>" );
		}else if( kind == 2 ){
			fprintf(out,"</td></tr></table><img src=\"images/user.gif\" border=0><b>All user are Read&Write!<br><br>" );
		}else if( kind == 4 ){
			fprintf(out,"</td></tr></table><img src=\"images/user.gif\" border=0><b>Share to all user Read Only!<br><br>" );
		}else if( kind == 5 ){
			fprintf(out,"</td></tr></table><img src=\"images/user.gif\" border=0><b>Share to all user Read&Write!<br><br>" );
		}
		return;
	}

void Get_folderuser_m(char *folder_name, FILE *out){

	int num;
	char flag=0;
	USER_VALUE user_list[MAX_USER];

	memset(user_list, 0, sizeof(USER_VALUE) * MAX_USER );

	Get_folder_user_all( folder_name, user_list, &flag);
	num = Get_folder_group_all(folder_name,&flag);
	print_user_group(folder_name,user_list,num,out);
}


void Get_folder_user_all( char *folder_name, USER_VALUE *folder_user, char *flag){

}

//To get the group access right of particular folder by reading /etc/smb.conf
int Get_folder_group_all(char *folder_name, char *flag){

    //return num;
    return 0;
}

void print_user_group(char *folder_name, USER_VALUE *user_list, int num, FILE *out){

	USER_VALUE *p;
	USER_VALUE invalid[MAX_USER];
	USER_VALUE readlist[MAX_USER];
	USER_VALUE writelist[MAX_USER];
	int i, x, y, z, read_group, write_group;

	memset(invalid,0,(sizeof(USER_VALUE)* MAX_USER));
	memset(readlist,0,(sizeof(USER_VALUE)* MAX_USER));
	memset(writelist,0,(sizeof(USER_VALUE)* MAX_USER));
	x = y = z = 0;
	read_group = 0;
	write_group = 0;
	p = user_list;

	for(i=0;i<MAX_USER;i++){
		if(!strlen(&(p->username[0])))
		    break;
		if(strcmp(&(p->user_kind[0]),"A") == 0){
			strcpy(&(invalid[x].username[0]),&(p->username[0]));
			x+=1;
		}else if(strcmp(&(p->user_kind[0]),"R") == 0){
			strcpy(&(readlist[y].username[0]),&(p->username[0]));
			y+=1;
		}else if(strcmp(&(p->user_kind[0]),"W") == 0){
			strcpy(&(writelist[z].username[0]), &(p->username[0]));
			z+=1;
		}
		p++;
	}
	//Group
	fprintf(out, "<tr><td align=\"left\" colspan=5><b><font color=\"#000000\">Read Only Groups</font></b></td></tr><tr>");
	for(i=0;i<num;i++){
		if(!strcmp(GROUP_set[i].kind,"R")){
			read_group++;
			fprintf(out,"<td width=\"20%%\" align=\"center\"><img src=\"images/group-s.gif\" border=0><br><font size=2 color=\"#000000\">%s</font></td>",GROUP_set[i].name);
			if((read_group+1)%5 == 0)
		    fprintf(out,"</tr><tr>\n");
	    }
	}
	if(read_group ==0)
		fprintf(out,"<td width=\"100%%\" align=\"center\" colspan=5><font color=\"#000000\">No groups.</font></td></tr><tr>"
		    "<tr><td align=\"left\" colspan=5><b><font color=\"#000000\">Read&Write Groups</font></b></td></tr><tr>");
	for(i=0;i<num;i++){
		if(!strcmp(GROUP_set[i].kind,"W")){
			write_group++;
			fprintf(out,"<td width=\"20%%\" align=\"center\"><img src=\"images/user-s.gif\" border=0><br><font size=2 color=\"#000000\">%s</font></td>",GROUP_set[i].name);
			if((write_group+1)%5 == 0)
		    fprintf(out,"</tr><tr>\n");
	    }
	}
	if(write_group ==0)
		fprintf(out,"<td width=\"100%%\" align=\"center\" colspan=5><font color=\"#000000\">No groups.</font></td></tr><tr>");
	//User
	fprintf(out,"<TR bgColor=#DDDDDD><TD height=30 colspan=6>&nbsp;&nbsp;User</TD>\n</tr>\n"
		"<tr><td align=\"left\" colspan=5><b><font color=\"#000000\">Read Only Users</font></b></td></tr><tr>");
    if(y == 0){
		fprintf(out,"<td width=\"100%%\" align=\"center\" colspan=5><font color=\"#000000\">No users.</font></td></tr><tr>");
	}else{
		for(i=0;i<y;i++){
		fprintf(out,"<td width=\"20%%\" align=\"center\"><img src=\"images/user-s.gif\" border=0><br><font size=2 color=\"#000000\">%s</font></td>",&(readlist[i].username[0]));
		if((i+1)%5 == 0)
		    fprintf(out,"</tr><tr>\n");
		}
	}
    fprintf(out,"<tr><td align=\"left\" colspan=5><b><font color=\"#000000\">Read&Write Users</font></b></td></tr><tr>");
	if(z == 0){
		fprintf(out,"<td width=\"100%%\" align=\"center\" colspan=5><font color=\"#000000\">No users.</font></td></tr><tr>");
	}else{
		for(i=0;i<z;i++){
			fprintf(out,"<td width=\"20%%\" align=\"center\"><img src=\"images/user-s.gif\" border=0><br><font size=2 color=\"#000000\">%s</font></td>", &(writelist[i].username[0]));
			if((i+1)%5 == 0)
			    fprintf(out,"</tr><tr>\n");
		}
	}
	fprintf(out,"</tr></table><br><br>");
	return;
}

 int Get_alluser(FILE *out){

	    FILE * fp;
	    char LineBuf[READ_SHORT];
	    USER_VALUE folder_user[MAX_USER];
	    char *ptr, *end, *bg;
	    int i, num = 0;

	    memset( folder_user, 0, sizeof(USER_VALUE)*MAX_USER );
	    get_hdcapacity();
	    num = Get_alluserlist( folder_user );
	    if((fp = fopen("/etc/passwd","r")) == NULL)
		return;
	    else{
		while(fgets(LineBuf,READ_SHORT-1,fp) != NULL){
//		    if(strstr(LineBuf,"/mnt") && !strstr(LineBuf,"*")){
		    if(strstr(LineBuf,"/home") && !strstr(LineBuf,"*")){
			    ptr = LineBuf;
			    end = strchr(ptr,':');
			    *end = '\0';
			    for(i=0;i<MAX_USER;i++){

				    if(!strcmp(ptr,&(folder_user[i].username[0]))){
					strcpy(&(folder_user[i].user_ftp[0]),"1");
				break;
			    }
			    }
		    }
		}
		fclose(fp);
	    }
	    /*
	    for(i=0;i<MAX_USER;i++){
		printf("folder_user[%d].username=%s\n",i,folder_user[i].username);
	    }
	    */
	    fprintf(out,"<html><head><title>Gigabit SOHO NAS</title>\n");
	fprintf(out,"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
	"<script language=\"JavaScript\">\n"
	"function check_hd(){\n"
	"if(document.form1.hdcapacity.value != 2){\n"
	"document.form1.apply.disabled=true;\n}\n}\n"
	"</script></head>\n"
	"<body onload=\"check_hd()\">\n"
	"<P><FONT class=style9>&nbsp;&nbsp;User Configuration</FONT></P>\n"
	"<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
	"<TBODY><TR>\n<TD width=\"100%%\" bgColor=#ffffff height=31>\n"
	"<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/sys_usr2.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
	"<TR height=5>\n<TD></TD></TR></TBODY></TABLE>\n"
	"<table border=\"0\" width=\"550\">\n"
	"<form name=form1 method=\"GET\" action=\"Set_deluser.cgi\" onsubmit=\"return double_check()\">\n"
	"<input type=\"hidden\" name=\"hdcapacity\" value=\"%d\">\n",hdcapacity);

	    for (i=0; i<MAX_USER; i++){
		    if ( !strlen( &(folder_user[i].username[0]) ) )
			    break;
			if((i%2)==0){
				#ifdef LONGSHINE
					bg = "#FF0000";
				#else
				bg = "#6266AF";	 //set to bgcolor in column of table base on NH221
				#endif

			}
			else{
			    bg = "#FFFFFF";
			}
		fprintf(out,"<tr height=60>\n<td width=\"30\" align=\"center\" bgcolor=\"%s\">\n",bg);
	  fprintf(out,"<input type=checkbox name=\"del_u\" value=\"%s\" OnClick=\"box_change(this.checked)\"></td>\n",&(folder_user[i].username[0]));
	  fprintf(out,"<td width=\"100\" align=\"center\" bgcolor=\"%s\">\n",bg);
	  fprintf(out,"<a href=\"Get_userinfo.cgi?name=%s\" target=_self><img src=\"images/user-s.gif\" border=0></a></td>\n",&(folder_user[i].username[0]));
	  fprintf(out,"<td width=\"420\" align=\"Left\" bgcolor=\"%s\">&nbsp;&nbsp;\n",bg);
	  fprintf(out,"<a href=\"Get_userinfo.cgi?name=%s\" target=_self><FONT color=#2222FF>%s</FONT></a></td></tr>\n",&(folder_user[i].username[0]),&(folder_user[i].username[0]));
	    }
	    fprintf(out,"</table><br>\n"
	    "<table border=\"0\" width=\"550\">\n<tr>\n"
	    "<td width=\"100\"><br></td>\n");
	    if(num == MAX_USER)
		fprintf(out,"<td width=\"100\"><input type=button value=\"Add\" name=\"apply\" onClick=\"check_num()\"></td>");
	    else
		fprintf(out,"<td width=\"100\"><input type=button value=\"Add\" name=\"apply\" onClick=\"parent.rightFrame.location.href='user_add.htm'\"></td>"
	    "<td width=\"100\"><input type=submit name=\"del\" value=\"Delete\"></td>\n"
	    "<td width=\"250\"><input name=\"button\" type=button onClick=\"history.back()\" value=\"Back\"></td></tr>\n"
	    "<tr>\n<td width=\"100\" valign=top align=right><b>Note:&nbsp;&nbsp;</b></td>\n"
	    "<td width=\"450\" align=left colspan=3 class=\"style4\">\n"
	    "Click \"Add\" to add a new user who is allowed to access the Network Shared Storage.<br>\n"
	    "Click \"Delete\" to remove the user you select from the list.<br>\n"
	    "Click the user icon to change the user name, password and description.\n"
	    "</td>\n</tr>\n</form>\n</table>\n"
	    "<MAP name=Map><AREA shape=RECT coords=11,1,124,23 href=\"network-superadmin.htm\">\n"
	    "<AREA shape=RECT coords=129,2,228,24 href=\"admin-time.htm\">\n"
	    "<AREA shape=RECT coords=227,1,326,23 href=\"Get_alluser.cgi\">\n"
	    "<AREA shape=RECT coords=327,1,426,23 href=\"Get_allgroup.cgi\"></map>\n"
	    "</body></html>\n"
	    "<script language=\"JavaScript\">var num=0;if (num==0) {document.form1.del.disabled=true;} function box_change(){if (arguments[0] == true){num++;}else if (arguments[0] == false) {num--;}if (num==0) {document.form1.del.disabled=true;}else {document.form1.del.disabled=false;}} function double_check(){flag=confirm(\"Are you sure you want to delete these users?\");if(flag){return true;}else{return false;}};\n"
	    "function check_num(){\n    alert(\"The number of users is over the limit!!\");\n    document.form1.apply.disabled=true;\n}</script>");
	    return 1;
	}



int Get_alluserlist( USER_VALUE *folder_user ){

    FILE *fp;
    char Line_b[READ_LONG];
    USER_VALUE *p;
    int i, num=0;
    char *ptr;

    p = folder_user;

    if((fp = fopen("/etc/smbpasswd","r")) == NULL)	return ;

    while(fgets(Line_b, READ_LONG-1, fp) != NULL){
	if (Line_b[0] == '#' || Line_b[0] == '\r' || Line_b[0] == '\n')	  continue;

	for(i=0;i<READ_LONG-1;i++){
		//snop modifies to make SP acceptable
	    //if( (Line_b[i] == ':') || (Line_b[i] == ' ') || (Line_b[i] == '\n') || (Line_b[i] == '\r'))
	    if( (Line_b[i] == ':') || (Line_b[i] == '\n') || (Line_b[i] == '\r')){
		Line_b[i] = '\0';
		i=READ_LONG;
	    }
	}
	ptr = Line_b;
	if(ptr != NULL && strcmp(ptr,"nobody")){
		strcpy( &(p->username[0]), ptr);
		p++;
		num++;
		}
	}
	fclose(fp);
	return num;
}


#ifdef NOVAC
	void Ch_userinfo(FILE *out){

	    char user_name[MAX_NAME+1];
		char n_passwd[MAX_PASSWD+1];
		char user_des[MAX_DESCHAR+1];
		char tmp[MAX_COMMAND];
		char result=0;

		memset( tmp, 0, sizeof(tmp) );

	    ARGUMENT args[]={
		{"name", user_name, MAX_NAME},
		{"user_newpasswd", n_passwd, MAX_PASSWD},
		{"user_des", user_des, MAX_DESCHAR},
		{NULL,NULL,0}
	    };

	    parseargs(args);
	    result = Ch_userinfo_m( user_name, user_des);

		if (!result)
			Systembusy_page(out, "history.back()");
	    //else if ( strlen(n_passwd) ){	   //snop modifies so that password could be blank
	    else{
			sprintf(tmp, "smbpasswd \"%s\" \"%s\" > /dev/null 2>&1", user_name, n_passwd);
			system(tmp);
		    Wait_page( out, 1, "/Get_alluser.cgi");
			//SaveConfiguration();
	    }
	    return;
	}

	int Ch_userinfo_m(char *user_name, char *user_des){

	    FILE *fp, *n_fd;
	    char Line_b[256];

	    if((fp = fopen("/etc/passwd","r")) == NULL)
		return 0;
	    if((n_fd = fopen("/etc/passwd1","w+")) == NULL)
		return 0;

	    while(fgets(Line_b, 255, fp) != NULL){
		    if(!strncmp(Line_b,user_name,strlen(user_name))){
			    sprintf(Line_b,"%s:*:0:0:%s:/var/%s:/bin/sh\n", user_name, user_des, user_name);
		    fprintf(n_fd, "%s",Line_b);
		}else{
		    fprintf(n_fd,"%s",Line_b);
		}
	    }
	    fclose(fp);
	    fclose(n_fd);
	    system("/bin/mv /etc/passwd1 /etc/passwd");
	    return 1;
	}
#else
	void Ch_userinfo(FILE *out){

	    char user_name[MAX_NAME+1];
		char n_passwd[MAX_PASSWD+1];
		char user_des[MAX_DESCHAR+1];
		char tmp[MAX_COMMAND];
		char result=0;
	    char ftp_user[2], ftp_admin[2];
	    char ftp_folder[256];

		memset( tmp, 0, sizeof(tmp) );

	    ARGUMENT args[]={
		{"name", user_name, MAX_NAME},
		{"user_newpasswd", n_passwd, MAX_PASSWD},
		{"user_des", user_des, MAX_DESCHAR},
		{"ftp_user", ftp_user, 2},
		{"mnt_dir", ftp_folder, 256},
		{"ftp_admin", ftp_admin, 2},
		{NULL,NULL,0}
	    };

	    parseargs(args);
#ifdef NH220
	strcpy(ftp_admin,"1");
	ftp_folder[0] = '\0';
#endif
	    result = Ch_userinfo_m( user_name, user_des, ftp_user, ftp_folder, ftp_admin);
	    //result = Ch_userinfo_m( user_name, user_des );

		if (!result)
			Systembusy_page(out, "history.back()");
	    //else if ( strlen(n_passwd) ){	   //snop modifies so that password could be blank
	    else{
			sprintf(tmp, "smbpasswd \"%s\" \"%s\" > /dev/null 2>&1", user_name, n_passwd);
			system(tmp);
		if(!strcmp(ftp_user,"1")){
		    sprintf(tmp, "passwd \"%s\" \"%s\" > /dev/null 2>&1", user_name, n_passwd);
			    system(tmp);
			}
		    Wait_page( out, 1, "/Get_alluser.cgi");
			SaveConfiguration();
	    }
	    return;
	}

int Ch_userinfo_m(char *user_name, char *user_des, char *ftp_user, char *ftp_folder, char *ftp_admin){

	    FILE *fp, *n_fd;
	    char Line_b[256];

	    if((fp = fopen("/etc/passwd","r")) == NULL)
		return 0;
	    if((n_fd = fopen("/etc/passwd1","w+")) == NULL)
		return 0;

	    while(fgets(Line_b, 255, fp) != NULL){
		    if(!strncmp(Line_b,user_name,strlen(user_name))){
			    if(!strcmp(ftp_user,"1")){	      // SAMBA and FTP User
				if(!strcmp(ftp_admin,"1"))    // Supervisor
				    sprintf(Line_b,"%s:*:0:0:%s:/mnt:/bin/sh\n", user_name, user_des);
				else			      // General User
				    sprintf(Line_b,"%s:*:0:0:%s:/mnt/%s:/bin/sh\n", user_name, user_des, ftp_folder);
				fprintf(n_fd, "%s",Line_b);
			    }else{			      // SAMBA User
				sprintf(Line_b,"%s:*:0:0:%s:/var/%s:/bin/sh\n", user_name, user_des, user_name);
			fprintf(n_fd, "%s",Line_b);
		    }
		}else{
		    fprintf(n_fd,"%s",Line_b);
		}
	    }
	    fclose(fp);
	    fclose(n_fd);
	    system("/bin/mv /etc/passwd1 /etc/passwd");
	    return 1;
	}
#endif


void Ch_username_m(FILE *out){

    FILE *fp, *n_fd;
    char Line_b[READ_LONG];
		char user_name[MAX_NAME+1];
		char n_username[MAX_NAME+1];
		char *ptr,*end;
		char result = 0;
		char url[MAX_URL+1];

		memset(url, 0, sizeof(url));

    ARGUMENT args[]={
	{"name", user_name, MAX_NAME},
	{"n_username", n_username, MAX_NAME},
	{NULL,NULL,0}
    };

    parseargs(args);

//    if ( !strcasecmp( user_name, "root" ) )	return;

    result = Cmp_user( n_username );

	if (!result){
		sprintf(url, "history.back()");
	Warmming_page(out, "User Name", url);
		return;
	}

	result = 0;

	if((fp = fopen("/etc/smb.conf","r")) == NULL)	return ;
	if((n_fd = fopen("/etc/smb1.conf","w+")) == NULL)	return ;

	while(fgets(Line_b, READ_LONG-1, fp) != NULL){
		if ( strstr(Line_b, "Invalid users =") != NULL){
			ptr = Line_b;
			delete_crlf( ptr );

		//fprintf(n_fd, "   Invalid users =");
			fprintf(n_fd, "	  Invalid users = ");  //snop modifies to make SP acceptable
			end = strchr( ptr, '=');
			end += 2;
			ptr = end;
			//printf("Invalid users\n");
			find_ch_username( ptr, user_name, n_username, n_fd);
		}else if ( strstr(Line_b, "readlist =") != NULL){
			ptr = Line_b;
			delete_crlf( ptr );

			//fprintf(n_fd, "   readlist =");
			fprintf(n_fd, "	  readlist = ");
			end = strchr( ptr, '=');
			end += 2;
			ptr = end;
			//printf("readlist\n");
			find_ch_username( ptr, user_name, n_username, n_fd);
		}else if ( strstr(Line_b, "writelist =") != NULL){
			ptr = Line_b;
			delete_crlf( ptr );

			//fprintf(n_fd, "   writelist =");
			fprintf(n_fd, "	  writelist = ");
			end = strchr( ptr, '=');
			end += 2;
			ptr = end;
			//printf("writelist\n");
			find_ch_username( ptr, user_name, n_username, n_fd);
		}
		else   fputs(Line_b, n_fd);
	}

	fclose(fp);
    fclose(n_fd);

	result = 0;//Ch_username_file( user_name, n_username);
	if (!result)
	{
		sprintf(url, "location.replace('/Get_userinfo.cgi?name=%s')", user_name);
		Systembusy_page(out, url);
	}
	else
	{
		system("/bin/mv /etc/smb1.conf /etc/smb.conf");
		//my_system("/bin/mv /etc/smb1.conf /etc/smb.conf");
		system("/bin/mv /etc/smbpasswd1 /etc/smbpasswd");
		//my_system("/bin/mv /etc/smbpasswd1 /etc/smbpasswd");
		system("/bin/mv /etc/passwd1 /etc/passwd");
		//my_system("/bin/mv /etc/passwd1 /etc/passwd");
		sprintf(url, "/Get_userinfo.cgi?name=%s", n_username);
		Wait_page( out, 1, url);
//#frankie
		SaveConfiguration();
#ifdef NH220
		my_system(out,"killall ftpd");
		my_system(out,"echo 0 > /var/run/ftpd_num");
#endif
		my_system( out, "/etc/smb restart &");
		//my_system("/etc/smb restart &");
	}

	return;
}

void find_ch_username(char *ptr, char *username, char *n_username, FILE *n_fd){

	char flag = 0;
	char *end;
	char f = 0;
	char n = 0;

	while ( ptr != NULL ){
		//end = strchr( ptr, ' ');
		end = strchr( ptr, ',');    //snop modifies to make SP acceptable
		if( end != NULL )
		    *end = '\0';
		else{
		    n = 1;
		    break;  //snop adds
		}

		if ( !strcmp( ptr, username ) )	 flag = 1;

		if (!flag){
			//snop modifies to make SP acceptable
			//fprintf(n_fd," %s", ptr);
			fprintf(n_fd,"%s,", ptr);
			fflush(n_fd);
		}else{
			//fprintf(n_fd," %s", n_username);
			fprintf(n_fd,"%s,", n_username);
			fflush(n_fd);
			f = 1;
			flag = 0;
		}

		if (f){
			ptr = ++end;
			if (!n){
				fprintf(n_fd,"%s", ptr);
				fflush(n_fd);
			}
			break;
		}

		if( end == NULL )
		    break;
		ptr = ++end;
	}

	fprintf(n_fd,"\n");
	fflush(n_fd);

	return;
}

    void Ch_foldername(FILE *out){

	    FILE * fp;
	    char LineBuf[READ_SHORT];
		  char folder_name[MAX_NAME+1];
		  char folder_des[MAX_DESCHAR+1];
		  int mount_count, i;

	    ARGUMENT args[]={
		{"name", folder_name, MAX_NAME},
		{"des",folder_des,MAX_DESCHAR},
		{NULL,NULL,0}
	    };

	    parseargs(args);

	    //printf("ch_foldername=%s\n", folder_name);

	    fprintf(out,"<html><head><title>Home Page</title>\n"
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
			"</head><body>\n"
			"<P><FONT class=style9>&nbsp;&nbsp;Share-Folder configuration</FONT></P>\n"
			"<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
			"<TBODY><TR>\n<TD width=\"100%%\" bgColor=#ffffff height=31>\n"
			"<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/dsk_fm.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
			"<TR height=5>\n<TD></TD></TR></TBODY></TABLE>\n"
    		    "<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1>\n"
    		    "<TBODY>\n<TR bgColor=#6defff>\n<TD class=style2 colSpan=3 height=30>&nbsp;&nbsp;Folder Name</TD>\n</TR>\n"
    		    "<form name=\"form1\" method=\"GET\" action=\"Ch_foldername_m.cgi\" onsubmit=\"return check_form()\">\n"
    		    "<tr>\n<td width=\"50\" align=\"center\" rowspan=5><img src=\"images/folder-s.gif\" border=0></td>\n"
    		    "<th width=\"150\" bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Old Folder Name</th>\n"
    		    "<td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;%s<input type=hidden name=\"name\" value=\"%s\"></td></tr>\n",folder_name,folder_name);
    	fprintf(out,"<tr>\n<th bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;New Folder Name</th>\n"
    		    "<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"n_foldername\" value=\"\" maxlength=\"15\"></td></tr>\n"
    		    "<tr>\n<th bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Folder Description</th>\n"
    		    "<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;%s</td></tr>\n",folder_des);
    	fprintf(out,"<tr>\n<th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th>\n"
    		    "<td bgcolor=\"#FFFFFF\">\n&nbsp;&nbsp;Folder Name is limited to 15 characters.<br>\n"
    		    "&nbsp;&nbsp;Discription is limited to 48 characters.</td></tr>\n"
    		    "<TR bgColor=#6defff>\n<TD class=style2 height=30>&nbsp;&nbsp;</TD>\n<TD height=30>\n"
    		    "&nbsp;&nbsp;<input type=submit value=\"Apply\">&nbsp;&nbsp;&nbsp;&nbsp;<input type=reset value=\"Undo\">&nbsp;&nbsp;&nbsp;&nbsp;<INPUT onclick=\"location.replace('Get_allfolder.cgi')\" type=button value=\"Back\" name=id_cancel>\n");


      //allen adds to defense rename thr USB folder
      mount_count = 0;//check_mount();
      for(i=0;i<4;i++)
		  fprintf(out,"<input type=hidden name=usbfolder%d value=%s>\n", i, MOUNT_set[i].folder_name);
    		fprintf(out,"</TD>\n</tr>\n</form>\n</TBODY>\n</TABLE>\n"
		"<MAP name=Map><AREA shape=RECT coords=10,1,109,23 href=\"Get_allfolder.cgi\">\n"
		"<AREA shape=RECT coords=110,1,209,23 href=\"disk-formathd.html\"></MAP>\n"
		"</body></html>\n"
		"<script language=\"JavaScript\">\n"
		"function check_form(){\n"
		"		 un=document.form1.name.value;\n"
		"		 cmp0=document.form1.usbfolder0.value;\n"
		"		 cmp1=document.form1.usbfolder1.value;\n"
		"		 cmp2=document.form1.usbfolder2.value;\n"
		"		 cmp3=document.form1.usbfolder3.value;\n"
		"		 if(un==cmp0 || un==cmp1 || un==cmp2 || un==cmp3){\n"
		"		   alert(\"This folder is a USB folder so that it can't be rename!!\");\n"
		"		   return false;\n"
		"		 }\n"
		"newn = document.form1.n_foldername.value;if (check_newn()==0){warming_b(\"New Folder Name\");document.form1.n_foldername.focus();return false;}if (check_nm()==0){document.form1.n_foldername.focus();return false;}return true;};function check_newn(){var ok=1;if (newn==null||newn==\"\") {ok=0;}return ok;}function check_nm(){var ok=1;var temp; for(var i=0;i<newn.length;i++){temp=newn.charCodeAt(i);if( !( (temp==0x21) || (temp>=0x24 && temp<=0x24) || (temp>=0x28 && temp<=0x29) || (temp>=0x2d && temp<=0x2e) || (temp>=0x30 && temp<=0x39) || (temp>=0x40 && temp<=0x5a)|| (temp==0x5e) || (temp>=0x61 && temp<=0x7b) || (temp>=0x7d && temp<=0x7e)) ){warming_char();ok=0;break;}} return ok;}function warming_b(){alert(arguments[0]+\": Can not be blank!\");}function warming_char(){alert(\"Folder Name ERROR. Characters allowed: a~z A~Z 0~9 ! $ ^ ( ) - { } . ~\");}</script>");
		//fprintf(out,"<script language=\"JavaScript\">function check_form(){newn = document.form1.n_foldername.value;if (check_newn()==0){warming_b(\"VtH?_[¼\");document.form1.n_foldername.focus();return false;}if (check_nm()==0){document.form1.n_foldername.focus();return false;}return true;};function check_newn(){var ok=1;if (newn==null||newn==\"\") {ok=0;}return ok;}function check_nm(){var ok=1;var temp; for(var i=0;i<newn.length;i++){temp=newn.charCodeAt(i);if(temp<33||temp>126){warming_char();ok=0;break;}} return ok;}function warming_b(){alert(arguments[0]+\":ðüÍµÄ­¾³¢B\");}function warming_char(){alert(\"VtH?_[¼És³È¶?ªüÍ³êÄ¢Ü·B\");}</script>");

		return ;
	}

void Ch_foldername_m(FILE *out){

    FILE *fp, *n_fd;
    char Line_b[READ_LONG];
		char folder_name[MAX_NAME+1];
		char n_foldername[MAX_NAME+1];
		char *ptr,*end;
		char f = 0;
		char url[MAX_URL+1];
		char tmp[MAX_URL+1];
		char path[MAX_URL+1];
		char result = 0;

		memset(url, 0, sizeof(url));
		memset(tmp, 0, sizeof(tmp));
		memset(path, 0, sizeof(path));

    ARGUMENT args[]={
	{"name", folder_name, MAX_NAME},
	{"n_foldername", n_foldername, MAX_NAME},
	{NULL,NULL,0}
    };

    parseargs(args);

    //printf("name = %s, n_foldername = %s\n",folder_name,n_foldername);
    result = Cmp_folder( n_foldername );

	if(!result){
	    sprintf(url, "history.back()");
#if defined(LOBOS) || defined(NOVAC) || defined(CENTURY)
		Warmming_page(out, "tH?_[¼", url);
#else
	Warmming_page(out, "Folder Name", url);
#endif
		return;
	}

//printf("old-foldername=%s\n", folder_name);
//printf("new-foldername=%s\n", n_foldername);

	if((fp = fopen("/etc/smb.conf","r")) == NULL)	return ;
	if((n_fd = fopen("/etc/smb1.conf","w+")) == NULL)	return ;

	sprintf(path,"path = /mnt/%s",folder_name);
	while(fgets(Line_b, READ_LONG-1, fp) != NULL){
	if(!f){
		if( Line_b[0] == '[' ){
		ptr = Line_b;
		end = strchr(ptr,'[');
		if (end != NULL) *end = '\0';
		ptr = ++end;
		end = strchr(ptr,']');
		if (end != NULL) *end = '\0';

//======================= reserve the special =========================
#if defined(LOBOS) || defined(NOVAC) || defined(CENTURY)
		if ( strcmp( ptr, "global") == 0 ){
#else
    #ifndef NH220
		//if ( strcmp( ptr, "global") == 0 || strcmp( ptr, FTP_SHARE) ==0 || strcmp( ptr, SMB_SHARE) == 0 ){
		if ( strcmp( ptr, "global") == 0 || strcmp( ptr, FTP_SHARE) ==0 ){
    #else
		//if ( strcmp( ptr, "global") == 0 || strcmp( ptr, SMB_SHARE) == 0 ){
		if ( strcmp( ptr, "global") == 0 ){
    #endif
#endif
			fprintf(n_fd, "[%s]\n", ptr);
			continue;
				}

			if ( strcmp( ptr, folder_name ) == 0){
			fprintf(n_fd, "[%s]\n", n_foldername);
			//f = 1;
		}else
		    fprintf(n_fd, "[%s]\n", ptr);

	    }else if(strstr(Line_b,path)){
		fprintf(n_fd,"	 path = /mnt/%s\n",n_foldername);
		f = 1;
	    }else
		fputs(Line_b, n_fd);
		}else
		    fputs(Line_b, n_fd);
	}
	fclose(fp);
    fclose(n_fd);

	system("/bin/mv /etc/smb1.conf /etc/smb.conf");
	//my_system("/bin/mv /etc/smb1.conf /etc/smb.conf");
	sprintf( tmp, "/bin/mv /mnt/%s /mnt/%s > /dev/null 2>&1", folder_name, n_foldername );
	system( tmp );
	//my_system( tmp );
	sprintf(url, "/Get_folderuser.cgi?name=%s", n_foldername);
	Wait_page( out, 1, url);
    //#Restart Samba
	SaveConfiguration();
#ifdef NH220
	my_system(out,"killall ftpd");
	my_system(out,"echo 0 > /var/run/ftpd_num");
#endif
	my_system( out, "/etc/smb restart &");
	//my_system("/etc/smb restart &");
	return;
}

void find_mntfolder(FILE *out)	/*is it in share.htm->find.htm? which isn't exist!*/
{
	FILE *fp;
	char LineBuf[READ_SHORT];
	FOLDER_VALUE hd_folder[MAX_FOLDER];
	char count = 0;
	char result = 0;
	int i;

	memset(hd_folder, 0, sizeof(hd_folder));

    if((fp = fopen("/htdocs/chfolder_kind_h.htm","r")) == NULL)
	return;
    else
    {
	while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

	result = find_mntfolder_m( &count, hd_folder );

	if (!result)
		Systembusy_page(out, "location.replace('/find.htm')");
	else  if ( count == 0 )
	{
		fprintf(out, "There are not any folder find!!!");
	}
	else
	{
		fprintf(out, "<form name=form1 method=\"POST\" action=/Set_delfolder.cgi><td colspan=6><font size=2 face=\"Arial, Helvetica, sans-serif\"><b>Here are all the folder which is not shared currently.<br>Please click some foleder you want to add and share in next page.<br><br><br></font></td><tr>");

	    for (i=0; i<count; i++)
	    {

		fprintf(out,"<td width=\"10%%\" align=\"center\" bgcolor=\"#FFCCFF\"><a href=\"Set_addmntfolder.cgi?name=%s\"><img src=\"images/folder-s.gif\" border=0></a></td><td width=\"23%%\" align=\"Left\" bgcolor=\"#FFFFFF\"><font side=2><a href=\"Set_addmntfolder.cgi?name=%s\">%s</a></font></a></td>", &(hd_folder[i].foldername[0]), &(hd_folder[i].foldername[0]), &(hd_folder[i].foldername[0]) );
		if ( (i+1) % 3 == 0 )  fprintf(out, "</tr><tr>" );
	}
	 #if defined(LOBOS) || defined(CENTURY)
	     fprintf(out,"</table><br><table border=\"0\" width=\"30%%\" align=\"center\"><tr><table><tr><td width=\"23%%\"></td><td width=\"30%%\"><input type=button value=\"ßé\" onClick=\"history.back()\"></td></tr></table><table><tr><td>&nbsp;</td></tr></table></tr></table></form></body></html>\n<script language=\"JavaScript\">var num=0;function box_change(){if (arguments[0] == true){num++;}else if (arguments[0] == false) {num--;}if (num==0) {document.form1.del.disabled=true;}else {document.form1.del.disabled=false;}}</script>");
	#else
	     fprintf(out,"</table><br><table border=\"0\" width=\"30%%\" align=\"center\"><tr><table><tr><td width=\"23%%\"></td><td width=\"30%%\"><input type=button value=\"Back\" onClick=\"history.back()\"></td></tr></table><table><tr><td>&nbsp;</td></tr></table></tr></table></form></body></html>\n<script language=\"JavaScript\">var num=0;function box_change(){if (arguments[0] == true){num++;}else if (arguments[0] == false) {num--;}if (num==0) {document.form1.del.disabled=true;}else {document.form1.del.disabled=false;}}</script>");
	#endif
	}
	return;
}

int  find_mntfolder_m( char *count, FOLDER_VALUE *hd_folder)
{
	char *ptr,*end;
	char LineBuf[READ_SHORT];
	FILE *alldir;
	FOLDER_VALUE *p;
	FOLDER_VALUE smb_folder[MAX_FOLDER];
	char smb_count = 0;
	char flag = 0;
	int i;

	memset(smb_folder, 0, sizeof(smb_folder));

	system("/bin/ls -F -w 1 /mnt/ | grep -v spool> /var/allfile");
	//my_system("/bin/ls -F -w 1 /mnt/ > /etc/allfile");

	if(( alldir = fopen( "/var/allfile", "r")) == NULL )  return 0;
	p = hd_folder;

	Get_allfolder_m( &smb_count , smb_folder );

	while(fgets(LineBuf, READ_SHORT-1, alldir) != NULL)
	{
		ptr = LineBuf;
		end = strchr( ptr, '/');
		if ( end != NULL )
		{
			*end = '\0';
#ifndef LOBOS
    #ifndef CENTURY
    #ifndef NH220
			if ( !strcasecmp( FTP_SHARE, ptr ) )  continue;
	#endif
			if ( !strcasecmp( SMB_SHARE, ptr ) )  continue;
    #endif
#endif
			if ( !smb_count )	 flag = 0;
			else
			{
				for (i=0; i<smb_count; i++)
				{
					if ( !strcasecmp( &(smb_folder[i].foldername[0]), SMB_SHARE) )	continue;
					if ( !strcasecmp( &(smb_folder[i].foldername[0]) , ptr ) )
					{
						flag = 1;
						break;
					}
				}
			}

			if (!flag)
			{
				strcpy( &(p->foldername[0]), ptr );
				p++;
				*count += 1;
			}
			else	flag = 0;
		}
	}
	fclose(alldir);
	system("rm -f /var/allfile");
	//my_system("rm -f /var/allfile");
	return 1;
}

void Set_addmntfolder(FILE *out)
{
    FILE *fp;
    char LineBuf[READ_SHORT];
	char folder_name[MAX_NAME+1];

     ARGUMENT args[]=
    {
	{"name", folder_name, MAX_NAME},
	{NULL,NULL,0}
    };

    parseargs(args);

    if((fp = fopen("/htdocs/folder-detail_h.htm","r")) == NULL)	 return;
    else
    {
	while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

    fprintf(out, "<form method=\"GET\" action=\"Set_addfolder.cgi\"><td bgColor=#0050a0 width=\"602\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#ffffff size=3>Share - Folder - \"%s\" configuration</FONT></B><br></td></tr></table><br><table border=\"0\" width=\"85%%\" align=\"center\"><tr>", folder_name);

	fprintf(out, "<td width=\"15%%\" align=\"center\" rowspan=4><img src=\"images/folder-s.gif\" border=0><br>%s</td><td width=\"5%%\"></td><td width=\"25%%\" bgcolor=\"#FFCCFF\"><b><font size=\"3\">Folder Name:</font></b></td><td width=\"35%%\" bgcolor=\"#FFFFFF\"><input type=text name=\"name\" value=\"%s\"><input type=\"hidden\" name=\"old_name\" value=\"%s\"><input type=\"hidden\" name=\"flag\" value=\"new\">", folder_name, folder_name, folder_name );

	fprintf(out, "</td><td width=\"15%%\"></td></tr><tr><td width=\"5%%\"></td><td width=\"30%%\" bgcolor=\"#FFCCFF\"><b><font size=\"3\">Folder Description:</font></b></td><td width=\"30%%\" bgcolor=\"#FFFFFF\"><input type=text name=\"folder_des\" value=\"%s\"></td><td width=\"15%%\"></td></tr>", folder_name );
	fprintf(out, "<tr></tr><tr></tr></table><table border=\"0\" width=\"30%%\" align=\"center\"><tr><td width=\"33%%\"><input type=submit value=\"Apply\"></td><td width=33%%><input type=reset value=\"Undo\"></td><br><td width=\"33%%\"><input type=button value=\"Back\" onClick=\"location.replace('/access.htm')\"></td></tr></table></form><br><br>");
}

void Ch_ftppasswd(FILE *out)
{
	char name[MAX_NAME+1];
	char n_password[MAX_PASSWD+1];
	char tmp[MAX_URL+1];
	char message = 0;

	memset ( tmp, 0, MAX_URL );

    ARGUMENT args[]=
    {
	{"name", name, MAX_NAME},
	{"n_password", n_password, MAX_NAME},
	{NULL,NULL,0}
    };

    parseargs(args);

	if ( strcasecmp( name, "ftpuser") != 0 )  message = 1;
	//else if ( check_digit( n_password ) != 1 )  message = 2;
	else if ( parsestr1( n_password, "/*ftpuser") != NULL )  message =	3;
	//else if ( stristr( n_password, "ftpuser") != NULL )  message =	3; //removed by me. RAW
	else if ( strchr( n_password, ':' ) != NULL )  message =  4;
	else if ( strlen( n_password ) < 6 )   message = 5;
	else
	{
		sprintf( tmp, "passwd ftpuser %s > /dev/null 2>&1", n_password);
		system( tmp );
		//my_system( tmp );
		SaveConfiguration();
		Wait_page( out, 1, "/FTP.htm" );
//		my_system( out, "/etc/inetd restart &");		//inetd need to restart?
		return;
	}

//	Ftperror_page(out, message, "location.replace('/FTP.htm')");	//inserted by RAW
FILE *fp;
char LineBuf[READ_SHORT];

    if((fp=fopen("/htdocs/JavaReturn_h.htm","r")) == NULL)
	return;
    else
    {
	while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

    fprintf(out,"<td bgColor=#0050a0 width=\"602\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#ffffff size=3>Warning...</FONT></B><br></td></tr></table>"
		"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><font color=#ff0000>FTP Password Warning: <br><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>");

    switch(message){
    case 1:
	fprintf(out,"Please don't change this ftpname!");
	break;
    case 2:
	fprintf(out,"New Password must have at least 1 numberal!");
	break;
    case 3:
	fprintf(out,"Please don't use something like your ftpname as password!");
	break;
    case 4:
	fprintf(out,"Please don't contain a special character - \":\" as password!");
	break;
    case 5:
    	fprintf(out,"New Password must have at least 6 characters!");
    }
    #if defined(LOBOS) || defined(CENTURY) || defined(CENTURY)
	    fprintf(out, "</b></font><br><br>Please try again...<br></td></tr><tr><td width=100%% align=\"right\"><input type=button value=\"ßé\" onClick=\"%s\"></td></tr></table>\n", "location.replace('/FTP.htm')" );
    #else
	    fprintf(out, "</b></font><br><br>Please try again...<br></td></tr><tr><td width=100%% align=\"right\"><input type=button value=\"Go Back\" onClick=\"%s\"></td></tr></table>\n", "location.replace('/FTP.htm')");
    #endif
}

/* not used
void parse_ufile_data( char *ufile )//extract filename from string "...ufile=filename&....." and copy it in area at point ufile
{
	char *ptr, *end;
	char buf_post[1000];

	strcpy( buf_post, postdata );
	ptr = buf_post;

	while (ptr != NULL)
	{
		end = w_strtok(&ptr, '=');
		if (end != NULL)
		{
		    if ( strcasecmp( end, "ufile" ) == 0 )
		    {
		        end = w_strtok(&ptr, '&');
			if (end != NULL)
				{
				strcpy( ufile, end) ;
				return;
				}
		    }
		    else
		    {
			end = w_strtok(&ptr, '&');
		    }
		}
		else	break;
	}
	return ;
}*/
extern int method1;
extern char *postdata;

//name:size_limit:format:file_name
int save_bfile(FILE *out, char *form_name, unsigned long long size_limit, char *format, char *file_name){

    FILE *fp;
    int ret_val = 1;
    unsigned long i = 0, b_len, t_len;
    char *boundary, *var_name = NULL, *var_name1 = NULL, *newbuf, *tmps, *tmp, *tmp1;

    if(method1 != POST_BOUNDARY || !postdata) return 1;

printf("content_length = %d\n",content_length);

    var_name = (char *) malloc(strlen(form_name) + 60);
    if(!var_name) return 1;


    sprintf(var_name, "Content-Disposition: form-data; name=\"%s\"; filename=\"", form_name);
    newbuf = (char *) memmem_in(postdata, content_length, var_name, &i);
//maybe can free var_name here
    if(newbuf && (*newbuf != '\"')){
	    tmps = strchr(newbuf, '\"');
	    if(tmps == NULL){
		fprintf(out, "\" - not found\n");
		free(var_name);
		return 1;
	    }
	    *tmps = '\0';
	    tmp = parsestr1(newbuf, format);
	    if(tmp == NULL){
		fprintf(out, "file_name:%s not matches with format:%s\n", newbuf, format);
		free(var_name);
		return 1;
	    }
	    tmp1 = strchr(file_name, '*');	//file_name = path/to/*file
	    if(tmp1){
		b_len = strlen(file_name) + strlen(newbuf) + 10;//makes for shure
		*tmp1 = '\0';
		tmp1++;
		var_name1 = (char *) malloc(b_len);
		if(var_name1 != NULL){
		    sprintf(var_name1, "%s%s%s", file_name, newbuf, tmp1);
		    file_name = var_name1;
		} else {
		    fprintf(out, "unable allocate memory\n");
		    free(var_name);
		    *(tmp1-1) = '*';
		    return 1;
		}
		*(tmp1-1) = '*';
	    }
//printf("File: %s", file_name);


	    b_len = content_length - i - strlen(var_name);	   // newbuf length, i is the offset

	    i = 0;
	    tmps = (char *) memmem_in(newbuf, b_len, "\r\n\r\n", &i);	// find data starting point

	    
	    if(i  && (b_len - i)){
		//NH200 begin
		b_len = b_len - i - 4;

		if( strncmp(tmps, "MAGICNUM", 8 ) == 0 ){

				tmps += 512;
				b_len -= 512;

		}
		boundary = "\r\n--------------------";//0x0d,0x0a
		newbuf = (char *) memmem_in(tmps, b_len, boundary, &t_len);
//		    t_len -= 2;		//SeaMonkey tunning

		if(t_len > 0){// found end boundary, t_len is the offset to boundary from tmps
		    if(size_limit && t_len > size_limit){
			fprintf(out, "File name:%s\nFile size:%ld > Size limit: %ld\n", file_name, t_len, size_limit);
			free(var_name);
			if(var_name1) free(var_name1);
			return 1;
		    }
		    fprintf(out, "File name:%s\nFile size:%ld\n", file_name, t_len);
		    if ((fp = fopen(file_name, "w+")) == NULL){
			fprintf(out, "Couldn't open %s to write!!\n", file_name);
			free(var_name);
			if(var_name1) free(var_name1);
			return 1;
		    }
		    fprintf(out, "start to write!\n");
		    if (  fwrite(tmps, t_len, 1, fp) != 1 ){
			fprintf(out, "writing files fails\n");
			free(var_name);
			if(var_name1) free(var_name1);
			return 1;
		    }
		    fprintf(out, "writing ... Please wait!! \n");
		    fclose(fp);
		    //Wait_page( out, 1, "/Upgrade_page.cgi" );
		    fprintf(out, "done\n");
		    ret_val = 0;
		}
	    }
	    if(var_name1) free(var_name1);
	}
	if(var_name) free(var_name);
return ret_val;
}

/*search needle in haystack and return the end of needle
    i - is offset from haystack to needle pointer
*/
void *memmem_in(char *haystack, long haystack_len, char *needle, long *i)	/*other places witch use this function must be changed to end of needle*/
{
    char *last_possible;
    int needle_len;
    
    needle_len = strlen(needle);// only one string is possible

    *i = 0;

    if (needle_len == 0)
	/* The first occurrence of the empty string is deemed to occur at
	   the beginning of the string.	 */
	return (void *) haystack;

    last_possible = (char *) haystack + haystack_len - needle_len;

    for ( ;haystack <= last_possible; ++haystack)
    {
	if (*haystack == *needle &&
	    !memcmp((void *) (haystack + 1),(void *) (needle + 1), needle_len - 1))
	{
		return (void *) (haystack + needle_len);
	}
	*i += 1;
    }

    *i = 0;
    return NULL;
}

int  check_firmware_version(char *check_buf){	//used in firmware_upgrade and in write_upload_file
	FILE *fp;
	char *ptr, *end;
	char Line_b[READ_LONG];
	char *buf;

	char o_MAGICNUM[30];
	char o_PRODUCT_ID[20];			/*reads version file and compare with check_buf entry*/
	char o_CUSTOMER[20];
	char o_VERSION[25];
	char o_RAM[4];

	char n_MAGICNUM[30];
	char n_PRODUCT_ID[20];
	char n_CUSTOMER[20];
	char n_VERSION[25];
	char n_RAM[4];

	if((fp = fopen("/etc/version","r")) == NULL)	return 0;

	while(fgets(Line_b, READ_LONG-1, fp) != NULL)
    {
	if ((ptr = strstr(Line_b, "MAGICNUM='")) != NULL)
		{
	    ptr+=10;
	    end = strchr(ptr,'\'');
	    *end = '\0';
	    strcpy(o_MAGICNUM, ptr);
		}
		else if ((ptr = strstr(Line_b, "PRODUCT_ID='")) != NULL)
		{
	    ptr+=12;
	    end = strchr(ptr,'\'');
	    *end = '\0';
	    strcpy(o_PRODUCT_ID, ptr);
		}
		else if ((ptr = strstr(Line_b, "CUSTOMER='")) != NULL)
		{
	    ptr+=10;
	    end = strchr(ptr,'\'');
	    *end = '\0';
	    strcpy(o_CUSTOMER, ptr);
		}
		else if ((ptr = strstr(Line_b, "VERSION='")) != NULL)
		{
	    ptr+=9;
			      ptr+=1;					// skip "v1.x"	- 'v'
	    end = strchr(ptr,'\'');
	    *end = '\0';
	    strcpy(o_VERSION, ptr);
	    if((ptr = strchr(o_VERSION,' ')) != NULL){
		ptr+=1;
		end = strchr(ptr,')');
		*end = '\0';
		strcpy(o_RAM,ptr);
	    }
		}
    }
    fclose(fp);

    linux_size = 0;
    root_size = 0;
    ramdisk2_size = 0;
	buf = check_buf ;
	if((ptr = strstr(buf,"MAGICNUM=")) != NULL){
		ptr+=9;
		end = strchr(ptr,0x0D);
		if(end == NULL)
			end = strchr(ptr,0x0A);
		*end = '\0';
		strcpy(n_MAGICNUM,ptr);
		buf = ++ end;
	}
	if((ptr = strstr(buf,"PRODUCT_ID=")) != NULL){
		ptr+=11;
		end = strchr(ptr,0x0D);
		if (end == NULL)
			end = strchr(ptr,0x0A);
		*end = '\0';
		strcpy(n_PRODUCT_ID,ptr);
		buf = ++ end;
	}
	if((ptr = strstr(buf,"CUSTOMER=")) != NULL){
		ptr+=9;
		end = strchr(ptr,0x0D);
		if(end == NULL )
			end = strchr(ptr,0x0A);
		*end = '\0';
		strcpy(n_CUSTOMER,ptr);
		buf = ++ end;
	}
	if ((ptr = strstr(buf,"VERSION=")) != NULL){
		ptr+=8;
		ptr+=1;						// skip "v1.x"	- 'v'
		end = strchr(ptr,0x0D);
		if ( end == NULL )
			end = strchr(ptr,0x0A);
		*end = '\0';
		strcpy(n_VERSION,ptr);
		buf = ++ end;
		if((ptr = strchr(n_VERSION,' ')) != NULL){
		ptr+=1;
		end = strchr(ptr,')');
		*end = '\0';
		strcpy(n_RAM,ptr);
    }
	}
    if((ptr = strstr(buf,"LINUX=")) != NULL){
		ptr+=6;
		end = strchr(ptr,0x0D);
		if(end == NULL )
			end = strchr(ptr,0x0A);
		*end = '\0';
		linux_size = atoi(ptr);
		buf = ++ end;
	  }
	if((ptr = strstr(buf,"ROOT=")) != NULL){
		ptr+=5;
		end = strchr(ptr,0x0D);
		if(end == NULL )
			end = strchr(ptr,0x0A);
		*end = '\0';
		root_size = atoi(ptr);
		buf = ++ end;
	}
	if((ptr = strstr(buf,"RAMDISK2=")) != NULL){
		ptr+=9;
		end = strchr(ptr,0x0D);
		if(end == NULL )
			end = strchr(ptr,0x0A);
		*end = '\0';
		ramdisk2_size = atoi(ptr);
		buf = ++ end;
	}
	if ( strcmp( o_MAGICNUM, n_MAGICNUM ) ){
		//printf("MAGICNUM: o_MAGICNUM = %s, n_MAGICNUM = %s\n",o_MAGICNUM,n_MAGICNUM);
		return 0;
    }
	if ( strcmp( o_PRODUCT_ID, n_PRODUCT_ID ) ){
		//printf("PRODUCT_ID: o_PRODUCT_ID = %s, n_PRODUCT_ID = %s\n",o_PRODUCT_ID,n_PRODUCT_ID);
		return 0;
	}
	/*if(strcmp(o_RAM,n_RAM)){
	      return 0;
	  }*/
	if ( strcasecmp( o_CUSTOMER, n_CUSTOMER)!= 0 ){			// NONEBRAND can upgrade to any customer
		//printf("CUSTMOER\n");
		if ( (strcasecmp( o_CUSTOMER, "NONEBRAND" ) != 0  && strcasecmp( n_CUSTOMER, "NONEBRAND" ) != 0) &&
			(strcasecmp( o_CUSTOMER, "NONBRAND" ) != 0  && strcasecmp( n_CUSTOMER, "NONBRAND" ) != 0) )
		{
			//printf("NONEBRAND: o_CUSTOMER = %s, n_CUSTOMER= %s\n",o_CUSTOMER,n_CUSTOMER);
		    return 0;
		}
	}//else if ( atof(o_VERSION) == atof(n_VERSION) )
		//return 0;

	return 1;

}

void Reboot_page1(FILE *out, char *filename){
/*    #ifdef DEBUG1
    printf("start Reboot_page1()\n");
    #endif
    fprintf(out,"<script language=\"JavaScript\">\n"
		"window.location.href=\"%s\";\n"
		"location.replace('http://%s:%s/%s');\n"
		"</script>\n",filename, CONFIG.IP, CONFIG.ADMIN_PORT, filename);
    #ifdef DEBUG1
    printf("CONFIG.IP=%s,filename=%s\n", CONFIG.IP, filename);
    #endif
*/
}


/* -replace- to the page 'filename'
    for work need IP and PORT from config file
*/
void boot_page(FILE *out, char *filename){
    fprintf(out,"<script language=\"JavaScript\">\n"
		"location.replace('http://%s:%s/"
		, CONFIG.IP, CONFIG.ADMIN_PORT);
    print(out, filename);

    fprintf(out,"');\n</script>\n");
    //fprintf(out,"window.location.href=\"%s\";\n",filename);
}

void Wait_page(FILE *out, int second, char *url){

#ifdef DEBUG1
printf("wait_page url=%s, second=%d\n",url,second);
#endif

    if(copy_file("./Waitpage_h.htm", out))	//we are in chdir() directory 
	    return;

/*    if(!strcmp(url,"/network-host.htm"))
	fp=fopen("./Waitpage_host.htm","r");
    else if(!strcmp(url,"/Get_alluser.cgi") || strstr(url,"/Get_userinfo.cgi"))
	fp=fopen("./Waitpage_user.htm","r");
    else if(!strcmp(url,"/Get_allfolder.cgi") || strstr(url,"/Get_folderuser.cgi"))
	fp=fopen("./Waitpage_folder.htm","r");
 #ifndef NOVAC
    else if(!strcmp(url,"Ch_group.cgi"))
	fp=fopen("./Waitpage_group.htm","r");
 #endif
    else
	fp=fopen("./Waitpage_user.htm","r");

    if(fp == NULL)
       return;
*/

#if defined(HAWKING)
	fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b><font color=\"#000000\">Your change was successfully! Please wait system reload page.</font> \n");
#else
    fprintf(out,"<td>\n<font size=3>Your change is successful! Please wait system reload page.</font><br><br>\n</td></tr></table>\n");
#endif

    fprintf(out,"<script language=\"javascript\">\nsettime();\n"
		"function settime(){setTimeout(\"reloadurl()\",%d000);\nreturn true;}\n"
		"function reloadurl(){location.replace(\"%s\");}</script></body></html>\n", second, url);

	return;
}

void JavaReturn_page(FILE *out, int second, char *ip, char *page){	//Wait_page is simmular to that function

#if 0		//RAW
	    FILE *fp;
	    char LineBuf[READ_SHORT];

	    if(!strcmp(page,"default") || !strcmp(page,"restart") || !strcmp(page,"url") || !strcmp(page_global,"url"))
		fp = fopen("./JavaReturn_maintain.htm","r");
	    else if(!strcmp(page,"format"))
		fp = fopen("./JavaReturn_hd.htm","r");
	    else if(!strcmp(page,"sync"))
		fp = fopen("./JavaReturn_sync.htm","r");
	    if(fp == NULL)
		return;
	    else{
		while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
		    fprintf(out,"%s",LineBuf);
		fclose(fp);
	    }
#else
	    if(copy_file("./JavaReturn_h.htm",out))	return;
#endif
	    if(!strcmp(page,"url")){
		    fprintf(out,"<td width=\"100%%\" bgcolor=\"#FFFFFF\">\n");
		    fprintf(out,"<form name=\"host\"><font size=3>&nbsp;&nbsp;The auto firmware update will connect to web site within <input type=\"text\" name=\"time\" value=\"%d\" size=\"3\" readonly> seconds.<br>&nbsp;&nbsp;Please wait in order to ensure that the operating environment is safe.</font></form></td></tr>\n", second);
		    fprintf(out,"<tr height=\"5\"><td></td></tr>\n</table>\n");
//		printf("return url page!!\n");
	    }
	    else{
		    fprintf(out,"<td width=\"100%%\" bgcolor=\"#FFFFFF\">\n");
		    fprintf(out,"<form name=\"host\"><font size=3>&nbsp;&nbsp;The system will restart automatically and reload in <input type=\"text\" name=\"time\" value=\"%d\" size=\"3\" readonly> seconds.<br>&nbsp;&nbsp;Please wait in order to ensure that the operating environment is safe.</font></FORM></td></tr>\n", second);
		    fprintf(out,"<tr height=\"5\"><td></td></tr>\n</table>\n");
//	      printf("return Show_maintain page!!\n");
	    }
    	    fprintf(out,"<script language=\"javascript\">\n"
	    "sec = document.host.time.value;\nsettime();\n"
	    "function settime(){\nif (sec > 0) {\nsec--;\ndocument.host.time.value = sec;\ntimerId = setTimeout(\"settime()\", 1000);\n}else{"
	    "top.location.href=('http://%s:%s');}}\n""</script></body></html>\n", ip, CONFIG.ADMIN_PORT);
}

/*
void reboot(void)
{
	system("kill -SIGTERM 1");
	//my_system("kill -SIGTERM 1");
	return;
}
*/

int parse_post_data(char *postdata,USER_VALUE *post_user){

    USER_VALUE *p;
	int i=0, open=0, pass=0, num=0;
	char *ptr, *ptr1, *end, *orig, *dest;

	memset(GROUP_set,0,sizeof(GROUP_VALUE)*MAX_GROUP);
	p = post_user;
	ptr = ptr1 = postdata;
	if((orig = strstr(ptr1,"auth")) != NULL){
		dest = strstr(orig,"=");
		dest++;
		if(!strncmp(dest,"pass",4)){
			open = 1;
		    return open;
		}
	}
	while(ptr != NULL){
	    if(i < MAX_USER){
		    end = strchr(ptr, '=');
		    if(end != NULL){
			    *end = '\0';
			    if(strcmp(ptr,END_MARK) == 0)
				break;
			    name_decode(ptr);
			    if(strstr(ptr,"user_")){
				ptr+=5;
			    strcpy(&(p->username[0]),ptr);
			    #ifdef DEBUG1
			    printf("&(p->username[0]) = %s\n",&(p->username[0]));
			    #endif
			    pass = 1;
			}else if(strstr(ptr,"group_")){
				ptr+=6;
				strcpy(GROUP_set[num].name,ptr);
				#ifdef DEBUG1
				printf("GROUP_set[%d].name = %s\n",num,GROUP_set[num].name);
				#endif
				pass = 2;
			}else
			    pass = 3;
		    ptr = ++end;
		}
		end = strchr(ptr,'&');
		if(end != NULL){
			    *end = '\0';
			    if(pass == 1){
			strcpy(&(p->user_kind[0]),ptr);
			//printf("&(p->user_kind[0]) = %s\n",&(p->user_kind[0]));
			p++;
		    }else if(pass == 2){
			strcpy(GROUP_set[num].kind,ptr);
			//printf("GROUP_set[%d].kind = %s\n",num,GROUP_set[num].kind);
			num++;
		    }
		    ptr = ++end;

		}else
		    break;
	    }else
		break;
	    if(pass == 1)
		i++;
	}
	return open;
}

void parse_deluser_postdate(int *count, USER_VALUE *deluser, char *post_data){

	char *ptr, *end, *orig;
	int i=0;
	USER_VALUE *p;

	p = deluser;
	//ptr = postdata;
	ptr = post_data;
	//snop adds to skip hdcapacity
	ptr = strchr(ptr, '&');
	ptr++;
	while (ptr != NULL){
		if (i < MAX_USER){
			end = strchr(ptr, '=');
			if (end != NULL){
				*end = '\0';
				if ( strcmp( ptr, "del") == 0 )
				    break;
				if ( strcmp( ptr, "del_u") == 0 ){
				ptr = ++end;
					end = strchr(ptr, '&');
				if(end != NULL){
						*end = '\0';
#if defined(LOBOS) || defined(NH220) || defined(CENTURY)
						if(strcasecmp( ptr, "root") != 0){
#else
			if(strcasecmp( ptr, "root") != 0 && strcasecmp( ptr, "ftpuser") != 0){
#endif
							httpd_decode(ptr);	  //snop adds to deocde special characters
							//printf("ptr = %s\n",ptr);
						strcpy( &(p->username[0]), ptr) ;
						*count+=1;
					}
					p++;
					ptr = ++end;
				}else
				    break;
			}
		}
		}
		i++;
	}
	return;
}

int deluser_to_passwd( int count, USER_VALUE *deluser){

	FILE *fp, *n_fd;
	USER_VALUE *p;
	char Line_buf[READ_LONG];
	char Line_b[READ_LONG];
	char flag=0;
	int i;

    if((fp = fopen("/etc/passwd","r")) == NULL)		return 0;
    if((n_fd = fopen("/etc/passwd1","w+")) == NULL)	return 0;

    while(fgets(Line_buf, READ_LONG-1, fp) != NULL){
	if (Line_buf[0] == '\r' || Line_buf[0] == '\n'){
		fputs(Line_buf, n_fd);
		continue;
	}

		strcpy( Line_b, Line_buf ) ;

	for(i=0;i<READ_LONG-1;i++){
		//snop modifies to make SP acceptable
	    //if( (Line_b[i] == ':') || (Line_b[i] == ' ') || (Line_b[i] == '\n') || (Line_b[i] == '\r')){
	    if( (Line_b[i] == ':') || (Line_b[i] == '\n') || (Line_b[i] == '\r')){
		Line_b[i] = '\0';
		i=READ_LONG;
	    }
	}

	if(Line_b != NULL){
		p = deluser;
			for(i=0; i<count; i++){
			if( !strcmp( Line_b, &(p->username[0]) ) ){
					flag=1;
					break;
				}
				p++;
			}

			if (!flag){
				fputs(Line_buf, n_fd);
				fflush(n_fd);
			}
			else   flag=0;

		}
	}
	fclose(fp);
	fclose(n_fd);

	system("/bin/mv /etc/passwd1 /etc/passwd");
	//my_system("/bin/mv /etc/passwd1 /etc/passwd");
	return 1;
}



//void Upgrade_page1(FILE *out)
void Upgrade_page1(FILE *out){

    FILE *fp;
    char LineBuf[256];
    #ifdef DEBUG1
    printf("Show the Upgrade_page1.cgi!!!!!!!\n");
    #endif
    if((fp=fopen("/htdocs/Waitpage_up_hold.htm","r")) == NULL){
	printf("Can't open Waitpage_up_hold.htm failure!!!\n");
	return;
    }
    else{
	while(fgets(LineBuf,256,fp) != NULL){
		  #ifdef DEBUG1
	    printf("LineBuf=%s\n",LineBuf);
	    #endif
	    fprintf(out,"%s",LineBuf);
	}
	fclose(fp);
    }
   #ifdef DEBUG1
   printf("start time box acccounting!!\n");
   #endif
	 fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><font size=3><b>This may take a few minutes.</b><br><b>Please wait in order to ensure that the operating environment is safe.</b><br><br><br><br>\n<b>Note: The system will reboot automatically and reload the home page in few minutes.</b>\n" );
	 fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",180000);\nreturn true;}\nfunction reloadurl(){location.replace(\"http://%s:%s/\");}</script><br></body></html>\n", CONFIG.IP,CONFIG.ADMIN_PORT );
   //fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",240000);\nreturn true;}\nfunction reloadurl(){location.replace(\"http://%s/\");}</script><br></body></html>\n", CONFIG.IP );

	 return;
}

//void Upgrade_page(FILE *out)
void Upgrade_page(FILE *out, char *ip){ //simmulare to JavaReturn_page

    FILE *fp;
    char LineBuf[256];

    #ifdef DEBUG1
    printf("Show the Upgrade_page.cgi!!!!!!!\n");
    #endif
    if((fp=fopen("/htdocs/Waitpage_up_hold.htm","r")) == NULL){
	printf("Can't open Waitpage_up_hold.htm failure!!!\n");
	return;
    }
    else{
	while(fgets(LineBuf,256,fp) != NULL){
		  #ifdef DEBUG1
	    printf("LineBuf=%s\n",LineBuf);
	    #endif
	    fprintf(out,"%s",LineBuf);
	}
	fclose(fp);
    }
   #ifdef DEBUG1
   printf("start time box acccounting!!\n");
   #endif
	 fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><font size=2>This may take a few minutes.<br>Please wait in order to ensure that the operating environment is safe.<br><br><br><br>\nNote: The system will reboot automatically and reload the home page in few minutes.\n" );
	 //fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",180000);\nreturn true;}\nfunction reloadurl(){location.replace(\"http://%s/\");}</script><br></body></html>\n", CONFIG.IP );
   fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",3000);\nreturn true;}\nfunction reloadurl(){location.replace(\"http://%s:%s/\");}</script><br></body></html>\n", CONFIG.IP,CONFIG.ADMIN_PORT );
   //Reboot_page1(out,"maintain.htm");
	 return;
}

void upgrade_error( FILE *out ){

    FILE *fp;
    char LineBuf[READ_SHORT];
    #ifdef DEBUG1
    printf("Show the Upgrade_error()\n");
    #endif
    if((fp=fopen("/htdocs/Waitpage_h.htm","r")) == NULL)
	return;
    else{
	while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

//    if ( message == PRODUCT_ID)
//	sprintf( message_string, "New firmware's is not suitable with this system." );
//    else if ( message == VERSION)
//	sprintf( message_string, "New firmware's Version is the same as old." );

//	fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b>Firmware Upgrade Error... <br><br>\nerror message: <font color=ff0000>%s</font><br>", message_string );

	fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b><font color=#000000>Firmware Upgrade Error... <br><br>\nerror message: </font><font color=ff0000>New firmware's is not suitable with this system.</font><br>"
		    "<br><font color=#000000>system will restart now.<br>Please wait and ensure a safe operating environment.</font><br>");

	fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",150000);\nreturn true;}\nfunction reloadurl(){location.replace(\"/\");}</script><br></body></html>\n");

	return;
}

#if defined(LOBOS) || defined(CENTURY)
	void Goto_page(FILE *out, char *url, char *type, char *folder_name){

	    FILE *fp;
	    char LineBuf[READ_SHORT];

	    if((fp=fopen("/htdocs/Gotopage_h.htm","r")) == NULL)
		return;
	    else{
		while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
		    fprintf(out,"%s",LineBuf);
		fclose(fp);
	    }

	    if(!strcmp( type, "User"))
#ifdef CENTURY
		fprintf(out,"<table border=\"0\" width=\"550\" align=\"center\"><tr><td><br><b>ÝèÍÏX³êÜµ½B <br><br>±Ì?[U[ <font color=\"#0000ff\"><b>\"%s\"</b></font>\n", folder_name);
#else
			fprintf(out,"<table border=\"0\" width=\"85%%\" align=\"center\"><tr><td><br><b>ÝèÍÏX³êÜµ½B <br><br>±Ì?[U[ <font color=\"#0000ff\"><b>\"%s\"</b></font>\n", folder_name);
#endif
		else
#ifdef CENTURY
	    fprintf(out,"<table border=\"0\" width=\"550\" align=\"center\"><tr><td><br><b>ÝèÍÏX³êÜµ½B <br><br>±Ì?[U[ <font color=\"#0000ff\"><b>\"%s\"</b></font>\n", folder_name);
#else
			fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b>ÝèÍÏX³êÜµ½B <br><br>±Ì?[U[ <font color=\"#0000ff\"><b>\"%s\"</b></font>\n", folder_name);
#endif

			fprintf(out,"ÌANZX§ÀðÏX·éê?ÍAu¤LvÝèÌ<a href=\"%s\">tH?_[</a>ðN?bNµÄ­¾³¢B<br></body></html>\n",url);

		return;
	}
#elif defined(NOVAC)
    void Goto_page(FILE *out, char *url, char *type, char *folder_name){

	    FILE *fp;
	    char LineBuf[READ_SHORT];

        fprintf(out,"<html><head><TITLE>Novac NAS Setup</TITLE>\n");
        fprintf(out,"<META http-equiv=Content-Type content=\"text/html; charset=Shift_JIS\">\n");
	fprintf(out,"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n");
	fprintf(out,"</head><BODY text=#000000 bgColor=#ffffff>\n");
	fprintf(out,"<P><FONT class=style9>VXe?Ýè</FONT></P>\n");
	fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(out,"<tr>\n<td width=\"100%\" bgcolor=\"#FFFFFF\" height=\"31\">\n");
	fprintf(out,"<div align=\"left\"><img src=\"images/sys_usr2.gif\" usemap=\"#Map\" border=\"0\"></div></td></tr>\n");
	fprintf(out,"<tr height=\"5\"><td></td></tr>\n</table><br>\n");
	fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr>\n");
	fprintf(out,"<td width=\"100%\" bgcolor=\"#FFFFFF\">\n<font size=3>ÝèÍÏX³êÜµ½B</font><br><br>\n");
	fprintf(out,"		±Ì?[U[ <font color=\"#0000ff\"><b>\"%s\"</b></font> ÌANZX§ÀðÏX·éê?ÍA<a href=\"%s\" target=_self><font color=\"#FF2222\">±±</font></a> ðN?bNµÄ­¾³¢B<br>\n",folder_name,url);
	fprintf(out,"</td>\n</tr>\n<tr height=\"5\"><td></td></tr></table>\n");
	fprintf(out,"<map name=\"Map\">\n");
	fprintf(out,"<AREA shape=RECT coords=010,1,109,23 href=\"network-superadmin.htm\">\n");
	fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"admin-time.htm\">\n");
	fprintf(out,"</map>\n");
	fprintf(out,"</body>\n</html>\n");
		return;
	}
#else
    void Goto_page(FILE *out, char *url, char *type, char *folder_name){

	    FILE *fp;
	    char LineBuf[READ_SHORT];

	    if((fp=fopen("./Gotopage_h.htm","r")) == NULL)
		return;
	    else{
		while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
		    fprintf(out,"%s",LineBuf);
		fclose(fp);
	    }

	    if ( !strcmp( type, "User") )
			fprintf(out,"<table border=\"0\" width=\"85%%\" align=\"center\"><tr><td><br><b><font color=\"#000000\">Your change was succeeded! </font><br><br><font color=\"#000000\">This new %s </font><font color=\"#0000ff\"><b>\"%s\"</b></font><font color=\"#000000\"> default limit is a </font><font color=\"#ff0000\"><b>Limited User</b></font><font color=\"#000000\"> for all Folders.</font> \n",type, folder_name);
		else
			fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b><font color=\"#000000\">Your change was succeeded! </font><br><br><font color=\"#000000\">This new %s </font><font color=\"#0000ff\"><b>\"%s\"</b></font><font color=\"#000000\"> default limit is </font><font color=\"#ff0000\"><b>Read/Write</b></font><font color=\"#000000\"> for all Users. </font>\n", type, folder_name);

			fprintf(out,"<br><br><font color=\"#000000\">If you want change this new %s's limits of authority, <br>please <a href=\"%s\">Click Here!</font></a><br></body></html>\n", type, url);
	}
#endif


//remove all that _pages, RAW
void Systembusy_page( FILE *out, char *go_url){
	return;
}

void Warmming_page( FILE *out, char *message, char *go_url){
	return;
}

void warmming_specialname( FILE *out, char *username, char *go_url){

    FILE *fp;
    char LineBuf[READ_SHORT];

#ifndef NOVAC
    if((fp=fopen("/htdocs/JavaReturn_h.htm","r")) == NULL)
	return;
    else{
	while(fgets(LineBuf,READ_SHORT-1,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }
#endif

	fprintf(out,"<td bgColor=#0050a0 width=\"602\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#ffffff size=3>Warning...</FONT></B><br></td></tr></table>"
		    "<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><font color=#ff0000>Warning: The name <b>\" %s \"</b> is a system special name.</font><br><br><font color=\"#000000\">Please add user for other name...</font><br></td></tr><tr><td width=100%% align=\"right\"><input type=button value=\"Go Back\" onClick=\"%s\"></td></tr></table>\n", username, go_url );
    return;
}

void Error_page( FILE *out, char *go_url){
}

void ftp_setup(FILE *out){	/*simmular to ftp-setup.htm w\o hdcapacity. comment out by RAW*/
/*fprintf(out,"\
<HTML><HEAD><TITLE>Gigabit SOHO NAS</TITLE><link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\
<script language=\"JavaScript\">\
function numbersonly(myfield, e, dec)\
{\
var key;\
var keychar;\
\
if (window.event)\
   key = window.event.keyCode;\
else if (e)\
   key = e.which;\
else\
   return true;\
keychar = String.fromCharCode(key);\
\
\
if ((key==null) || (key==0) || (key==8) ||\
    (key==9) || (key==13) || (key==27) )\
   return true;\
\
\
else if (((\"0123456789\").indexOf(keychar) > -1))\
   return true;\
\
\
else if (dec && (keychar == \".\"))\
   {\
   myfield.form.elements[dec].focus();\
   return false;\
   }\
else\
   return false;\
}\
\
\
function check_form(){\
    unit=document.host.ftp_unit.value;\
    port=document.host.ftp_port.value;\
    time=document.host.ftp_time.value;\
    if(check_unit()==0){\
	warmming_unit();\
	document.host.ftp_unit.focus();\
	return false;\
    }\
    if(check_port()==0){\
	warmming_port();\
	document.host.ftp_port.focus();\
	return false;\
    }\
    if(check_time()==0){\
	warmming_time();\
	document.host.ftp_time.focus();\
	return false;\
    }\
    function check_unit(){\
	var ok=1;\
	if(unit == null || unit == \"\" || unit > 8 || unit < 0)\
	    ok=0;\
	return ok;\
    }\
    function check_port(){\
	var ok=1;\
	if(port ==null || port == \"\" || port < 1 || port > 65535)\
	    ok=0;\
	return ok;\
    }\
    function check_time(){\
	var ok=1;\
	if(time ==null || time == \"\" || time <= 0 || time.indexOf(\".\") > 0)\
	    ok=0;\
	return ok;\
    }\
    function warmming_unit(){\
	alert(\"Error!!! Maximum Unit must be between 0 and 8!\");\
    }\
    function warmming_port(){\
	alert(\"Error!!! Port Number must be between 1 and 65535!\");\
    }\
    function warmming_time(){\
	alert(\"Error!!! Timeout must be an interger greater than 0!\");\
    }\
    if(document.host.anony[0].checked==true)\
	document.host.anonymity.value = 1;\
    else\
	document.host.anonymity.value = 0;\
}\
function load_anony(){\
    if(document.host.anonymity.value == 1)\
	document.host.anony[0].checked=true;\
    else\
	document.host.anony[1].checked=true;\
}\
</script>\
"); //first part of HTML page ends here

#ifdef LONGSHINE
	fprintf(out,"\
	</HEAD>\
	<BODY text=#000000 bgColor=#ffffff onload=\"load_anony()\">\
	<P><FONT class=style9>&nbsp;&nbsp;FTP Configuration</FONT></P>\
	<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
	<tr>\
		<td width=\"100%\" bgcolor=\"#FFFFFF\" height=\"31\">\
			<div align=\"left\">&nbsp;&nbsp;<img src=\"images/option_ftp.gif\" usemap=\"#Map\" border=\"0\"></div>\
		</td>\
	</tr>\
	<tr height=\"005\"><td></td></tr>\
	</table>\
	<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1>\
	<TBODY>\
	  <TR>\
		<TD class=longshine_2 colSpan=3 height=30>&nbsp;&nbsp;FTP Setting</TD>\
	  </TR>\
		<form name=host method=\"Get\" action=\"Set_FTP.cgi\" onsubmit=\"return check_form()\">\
	");

#elif MICRONET

	fprintf(out,"\
	</HEAD>\
	<BODY text=#000000 bgColor=#ffffff onload=\"load_anony()\">\
	<P><FONT class=style9>&nbsp;&nbsp;FTP Configuration</FONT></P>\
	<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
	<tr>\
		<td width=\"100%\" bgcolor=\"#FFFFFF\" height=\"31\">\
			<div align=\"left\">&nbsp;&nbsp;<img src=\"images/option_ftp.gif\" usemap=\"#Map\" border=\"0\"></div>\
		</td>\
	</tr>\
	<tr height=\"005\"><td></td></tr>\
	</table>\
	<TABLE borderColor=#009999 cellSpacing=0 cellPadding=0 width=550 border=1>\
	<TBODY>\
	  <TR bgColor=#008080>\
		<TD class=style2 colSpan=3 height=30>&nbsp;&nbsp;FTP Setting</TD>\
	  </TR>\
		<form name=host method=\"Get\" action=\"Set_FTP.cgi\" onsubmit=\"return check_form()\">\
	");

#else
	fprintf(out,"\
	</HEAD>\
	<BODY text=#000000 bgColor=#ffffff onload=\"load_anony()\">\
	<P><FONT class=style9>&nbsp;&nbsp;FTP Configuration</FONT></P>\
	<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
	<tr>\
		<td width=\"100%\" bgcolor=\"#FFFFFF\" height=\"31\">\
			<div align=\"left\">&nbsp;&nbsp;<img src=\"images/option_ftp.gif\" usemap=\"#Map\" border=\"0\"></div>\
		</td>\
	</tr>\
	<tr height=\"005\"><td></td></tr>\
	</table>\
	<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1>\
	<TBODY>\
	  <TR bgColor=#020759>\
		<TD class=style2 colSpan=3 height=30>&nbsp;&nbsp;FTP Setting</TD>\
	  </TR>\
		<form name=host method=\"Get\" action=\"Set_FTP.cgi\" onsubmit=\"return check_form()\">\
	");
#endif


#ifdef LONGSHINE

fprintf(out,"<tr><th width=\"150\" bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Maximum Unit Number</th><td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_unit\" onKeyPress=\"return numbersonly(this, event)\"		       value=\"%s\"  OnChange=\"cmdCopyName()\" size=20 maxlength=\"15\"></td></tr>",CONFIG.FTP_MAX);
fprintf(out,"<tr><th		   bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Port Number	 </th><td		bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_port\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20></td></tr>",CONFIG.FTP_PORT);
fprintf(out,"<tr><th		   bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Timeout		 </th><td		bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_time\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20>Minutes</td></tr>",CONFIG.FTP_TIMEOUT);
fprintf(out,"\
  <tr>\
	<th bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Allow Anonymous</th>\
	<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"radio\" name=\"anony\"  value=\"1\">Yes\
	<input type=\"radio\" name=\"anony\" value=\"0\">No</td>\
	<input type=\"hidden\" name=\"anonymity\" value=\"%s\">\
  </tr>\
  <tr>", CONFIG.FTP_ANONYMITY);

fprintf(out, "\
	<th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th>\
	<td bgcolor=\"#FFFFFF\" class=\"style4\">\
		&nbsp;&nbsp;1. The maximum number of unit is up to 8.<br>\
	&nbsp;&nbsp;2. The port number is between 1 and 65535.<BR>\
	&nbsp;&nbsp;3. FTP settings are disabled when USB disk is absent.<BR>\
	<BR>\
	&nbsp&nbsp&nbspAllows remote access over a local network or the Internet to files on &nbsp&nbsp&nbspthe Network\
	Shared Storage system.<BR> &nbsp&nbsp&nbspAlso large FTP downloads can be performed by the Network\
	Shared &nbsp&nbsp&nbspStorage and allow your laptop, PC, or MAC to continue to be used for &nbsp&nbsp&nbspother\
		applications.\
	</td>\
  </tr>\
  ");


fprintf(out,"\
  <TR>\
	<TD class=longshine_2 colSpan=2 align=center>\
	");

	if(hdcapacity!=2) //if USB disk is not present
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp disabled>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\" disabled>&nbsp;&nbsp;&nbsp;&nbsp;");
	else
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\">&nbsp;&nbsp;&nbsp;&nbsp;");

#elif MICRONET

fprintf(out,"<tr><th width=\"150\" bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Maximum Unit Number</font></th><td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_unit\" onKeyPress=\"return numbersonly(this, event)\"		      value=\"%s\"  OnChange=\"cmdCopyName()\" size=20 maxlength=\"15\"></td></tr>",CONFIG.FTP_MAX);
fprintf(out,"<tr><th		   bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Port Number	 </font></th><td	       bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_port\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20></td></tr>",CONFIG.FTP_PORT);
fprintf(out,"<tr><th		   bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Timeout		 </font></th><td	       bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_time\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20>Minutes</td></tr>",CONFIG.FTP_TIMEOUT);
fprintf(out,"\
  <tr>\
	<th bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Allow Anonymous</th>\
	<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"radio\" name=\"anony\"  value=\"1\">Yes\
	<input type=\"radio\" name=\"anony\" value=\"0\">No</td>\
	<input type=\"hidden\" name=\"anonymity\" value=\"%s\">\
  </tr>\
  <tr>", CONFIG.FTP_ANONYMITY);

fprintf(out, "\
	<th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th>\
	<td bgcolor=\"#FFFFFF\" class=\"style4\">\
		&nbsp;&nbsp;1. The maximum number of unit is up to 8.<br>\
	&nbsp;&nbsp;2. The port number is between 1 and 65535.<BR>\
	&nbsp;&nbsp;3. FTP settings are disabled when USB disk is absent.<BR>\
	<BR>\
	&nbsp&nbsp&nbspAllows remote access over a local network or the Internet to files on &nbsp&nbsp&nbspthe Network\
	Shared Storage system.<BR> &nbsp&nbsp&nbspAlso large FTP downloads can be performed by the Network\
	Shared &nbsp&nbsp&nbspStorage and allow your laptop, PC, or MAC to continue to be used for &nbsp&nbsp&nbspother\
		applications.\
	</td>\
  </tr>\
  ");



fprintf(out,"\
  <TR bgColor=#008080>\
	<TD class=style2 height=30 colspan=1>&nbsp;&nbsp;</TD>\
	<TD height=30>&nbsp;&nbsp;\
	");

	if(hdcapacity!=2) //if USB disk is not present
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp disabled>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\" disabled>&nbsp;&nbsp;&nbsp;&nbsp;");
	else
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\">&nbsp;&nbsp;&nbsp;&nbsp;");

#else

fprintf(out,"<tr><th width=\"150\" bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Maximum Unit Number</th><td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_unit\" onKeyPress=\"return numbersonly(this, event)\"		       value=\"%s\"  OnChange=\"cmdCopyName()\" size=20 maxlength=\"15\"></td></tr>",CONFIG.FTP_MAX);
fprintf(out,"<tr><th		   bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Port Number	 </th><td		bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_port\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20></td></tr>",CONFIG.FTP_PORT);
fprintf(out,"<tr><th		   bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Timeout		 </th><td		bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"ftp_time\" onKeyPress=\"return numbersonly(this, event)\" maxlength=\"15\" value=\"%s\"  size=20>Minutes</td></tr>",CONFIG.FTP_TIMEOUT);
fprintf(out,"\
  <tr>\
	<th bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Allow Anonymous</th>\
	<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"radio\" name=\"anony\"  value=\"1\">Yes\
	<input type=\"radio\" name=\"anony\" value=\"0\">No</td>\
	<input type=\"hidden\" name=\"anonymity\" value=\"%s\">\
  </tr>\
  <tr>", CONFIG.FTP_ANONYMITY);

fprintf(out, "\
	<th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th>\
	<td bgcolor=\"#FFFFFF\" class=\"style4\">\
		&nbsp;&nbsp;1. The maximum number of unit is up to 8.<br>\
	&nbsp;&nbsp;2. The port number is between 1 and 65535.<BR>\
	&nbsp;&nbsp;3. FTP settings are disabled when USB disk is absent.<BR>\
	<BR>\
	&nbsp&nbsp&nbspAllows remote access over a local network or the Internet to files on &nbsp&nbsp&nbspthe Network\
	Shared Storage system.<BR> &nbsp&nbsp&nbspAlso large FTP downloads can be performed by the Network\
	Shared &nbsp&nbsp&nbspStorage and allow your laptop, PC, or MAC to continue to be used for &nbsp&nbsp&nbspother\
		applications.\
	</td>\
  </tr>\
  ");


fprintf(out,"\
  <TR bgColor=#020759>\
	<TD class=style2 height=30 colspan=1>&nbsp;&nbsp;</TD>\
	<TD height=30>&nbsp;&nbsp;\
	");

	if(hdcapacity!=2) //if USB disk is not present
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp disabled>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\" disabled>&nbsp;&nbsp;&nbsp;&nbsp;");
	else
		fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=save_ftp>&nbsp;&nbsp;&nbsp;&nbsp;\
				<INPUT type=\"reset\" value=\"Undo\" name=\"id_cancel\">&nbsp;&nbsp;&nbsp;&nbsp;");


#endif
	fprintf(out,"\
		<INPUT type=\"button\" value=\"Back\" onclick=\"history.back()\">\
		</TD>\
		</TR>\
	</TBODY>\
	</TABLE>\
	<map name=\"Map\">\
	<!--\
	<AREA shape=RECT coords=10,1,113,23 href=\"service_number.htm\">\
	<AREA shape=RECT coords=114,1,220,23 href=\"ftp_setup.cgi\">\
	<AREA shape=RECT coords=221,1,330,23 href=\"security-ip.htm\">\
	<AREA shape=RECT coords=331,1,440,23 href=\"security-mac.htm\">\
	<AREA shape=RECT coords=441,1,550,23 href=\"ShowWgetCGI_1.cgi\">\
	-->\
	<AREA shape=RECT coords='11, 1, 110, 23' href=\"ftp_setup.cgi\">\
	<AREA shape=RECT coords='112, 2, 212, 24' href=\"security-ip.htm\">\
	<AREA shape=RECT coords='214, 1, 320, 23' href=\"security-mac.htm\">\
	<AREA shape=RECT coords='322, 1, 435, 23' href=\"ShowWgetCGI_1.cgi\">\
	</map></form></BODY></HTML>");
*/
}

void write_system(char *buf, long long size, int mode, char *cmd)
{
	int pid, status;
	int pipe_fd[2];
	int wrote;
	int err = 0;
	long long i = 0;

if(!size) return;

if(mode){	//mode==1 -> cat mode
	while(buf[i] && (size - 1 - i)){
	    i++;
	}
}

    if (pipe(pipe_fd) == 0) {

	if (!(pid=fork())){	//child
	    close(pipe_fd[0]);	//read end

	    close(fd);	    	//}for inetd
	    close(sockfd);

	    close(1);
	    close(2);
	    dup2(pipe_fd[1], 1);	//1 -stdout
	    dup2(pipe_fd[1], 2);	//2 -stderr
	    close(pipe_fd[1]);

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);

	    exit(1);
	}else{			//parent
	    close(pipe_fd[1]);	//write end
	    if (pid > 0) {	// fork -- parent
		while(i<(size-1) && read(pipe_fd[0], (buf+i), 1) > 0){
//printf("n=%d w=%d",n,wrote);
//		    if(i >= (size-1)) break;
		    i++;
		}
		buf[i] = '\0';
		wait(&status);
	    } else	//fork failed
		err = 2;

	    close(pipe_fd[0]);	//close rest
	
	}

    } else err = 1;	//make a pipe failed
    if(err)	fprintf(stderr, "my_system: Unable to run child process %s, errno %d\n", cmd, err);
}


void my_system(FILE *out, char *cmd)
{
	int pid, status;
	int pipe_fd[2];
	int wrote;
	int err = 0;
	int buf;
	
    if (pipe(pipe_fd) == 0) {

	if (!(pid=fork())){	//child
	    close(pipe_fd[0]);	//read end

	    close(fd);	    	//}for inetd
	    close(sockfd);

	    close(1);
	    close(2);
	    dup2(pipe_fd[1], 1);	//1 -stdout
	    dup2(pipe_fd[1], 2);	//2 -stderr
	    close(pipe_fd[1]);

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);
	    exit(0);
	}else{			//parent
	    close(pipe_fd[1]);	//write end
	    if (pid > 0) {	// fork -- parent
		while(read(pipe_fd[0], &buf, 1) > 0){
		    wrote = fwrite(&buf, 1, 1, out);
//printf("n=%d w=%d",n,wrote);
		    if(wrote < 1){
			err = 3;	//write failed
			break;
		    }
		}
//		waitpid(pid, &status, 0);
		wait(&status);
	    } else	//fork failed
		err = 2;

	    close(pipe_fd[0]);	//close rest
	
	}

    } else err = 1;	//make a pipe failed
    if(err)	fprintf(out, "my_system: Unable to run child process %s, errno %d\n", cmd, err);
}

void system_(char *cmd)
{
	int pid, status;
	int err = 0;
	
	if (!(pid=fork())){	//child

	    close(fd);	    	//for inetd
	    close(sockfd);

//	    close(1);
//	    close(2);

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);
	    exit(1);
	}else{			//parent
	    if (pid > 0) {	// fork -- parent
		wait(&status);
	    } else	//fork failed
		err = 1;

	}

    if(err)	fprintf(stderr, "system: Unable to run child process %s, errno %d\n", cmd, err);
}

void name_decode(char *string){

	char *dest, *orig;

	if(string[0] == '_' && isdigit(string[1])){
		orig = strchr(string,'_');
		strcpy(string,orig+1);
	}
	if(strstr(string,"s_p_a")){
		while((orig = strstr(string,"s_p_a")) != NULL){
	    dest = orig+5;
	    *orig = '\0';
	    strcat(string," ");
	    strcat(string,dest);
	}
    }
    if(strstr(string,"a_n_d")){
		while((orig = strstr(string,"a_n_d")) != NULL){
	    dest = orig+5;
	    *orig = '\0';
	    strcat(string,"&");
	    strcat(string,dest);
	}
    }
    return;
}

void name_encode(char *decode_string, char *orig_string){

	int i, l;

	for(i=0,l=0;i<strlen(orig_string);i++){
	    if(isspace(orig_string[i])){
	    decode_string[l] = 's';
	    l++;
	    decode_string[l] = '_';
	    l++;
	    decode_string[l] = 'p';
	    l++;
	    decode_string[l] = '_';
	    l++;
	    decode_string[l] = 'a';
	    l++;
	}else if(orig_string[i] == '&'){
		decode_string[l] = 'a';
		l++;
		decode_string[l] = '_';
		l++;
		decode_string[l] = 'n';
		l++;
		decode_string[l] = '_';
		l++;
		decode_string[l] = 'd';
		l++;
	}else{
	    decode_string[l] = orig_string[i];
	    l++;
	}
    }
    decode_string[l] = '\0';
    return;
}

	/***************hier was function  void Show_maintain(char *data, FILE *out)***********************/


/*void filename_error(FILE *out){

	FILE *fp;
    char LineBuf[256];

    if((fp=fopen("/htdocs/Waitpage_h.htm","r")) == NULL)
	return;
    else{
	while(fgets(LineBuf,255,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

	fprintf(out,"<table border=\"0\" width=\"70%%\" align=\"center\"><tr><td><br><b><font color=#%s>Unicode Filename Error... <br><br>\nerror message: </font><font color=ff0000>Unicode filename must be ntx_charset.h.</font><br>",get_font_color());
	fprintf(out,"<br><font color=#%s><br>Please don't change the unicode filename name.</font><br></td></tr>\n",get_font_color());
	fprintf(out,"<tr><td align=center><input type=button value=Back name=back onclick=history.back()></td></tr></table>\n");
	fprintf(out,"<br></body></html>\n");
	return;
}*/



/************ hier was show_USB_detail & Show_USB_disk ****************************************************/
/*
int check_mount_failure_sp(void){
int check_mount_failure(void){
int check_mount(void){
void Set_USB_Disk(char *data, FILE *out){
*/

void Add_Group(FILE *out){

    FILE *fp;
    int i, num = 0;
    char LineBuf[256], description[51];
    USER_DES_VALUE user_info[MAX_USER];

    if((fp = fopen("/htdocs/group_add.htm","r")) == NULL)
	return;
    else{
	while(fgets(LineBuf,255,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

    num = Get_allusername(user_info);
    if(num == 0){
	fprintf(out,"<tr bgcolor=\"#FFFFFF\"><td colspan=3 class=style4>No authorized users</td></tr>\n");
    }else{
	for(i=0;i<num;i++){
		Get_user_description(user_info[i].username,description);
		//printf("description = %s\n",description);
		strcpy(user_info[i].descrip,description);
	}
	for(i=0;i<num;i++){
		fprintf(out,"<TR bgColor=#FFFFFF>\n");
		fprintf(out,"<TD width=050><input type=\"checkbox\" name=\"user\" value=%s></TD>\n",user_info[i].username);
		fprintf(out,"<TD width=090><img src=\"images/user-s.gif\" border=0><br>%s</TD>\n",user_info[i].username);
		fprintf(out,"<TD width=400>%s</td></tr>\n",user_info[i].descrip);
		fprintf(out,"<TR bgColor=#FFFFFF>\n");
		fprintf(out,"<TD colspan=3><hr></TD></tr>\n");
	    }
    }
    fprintf(out,"</table>\n<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",END_MARK,END_MARK);
    fprintf(out,"</form></TBODY></TABLE></BODY></HTML>\n");
}

//To get all the authorized users by reading /etc/smbpasswd
//Return the number of all authorized users
int Get_allusername(USER_DES_VALUE *folder_user){

    FILE *fp;
    char Line_b[READ_LONG];
    USER_DES_VALUE *p;
    int i, num=0;
    char *ptr;

    p = folder_user;

    if((fp = fopen("/etc/smbpasswd","r")) == NULL)
	return;

    while(fgets(Line_b, READ_LONG-1, fp) != NULL){
	if (Line_b[0] == '#' || Line_b[0] == '\r' || Line_b[0] == '\n')
	    continue;
	for(i=0;i<READ_LONG-1;i++){
	    if( (Line_b[i] == ':') || (Line_b[i] == '\n') || (Line_b[i] == '\r')){
		Line_b[i] = '\0';
		i=READ_LONG;
	    }
	}
	ptr = Line_b;
	if(ptr != NULL && strcmp(ptr,"nobody")){
		strcpy( &(p->username[0]), ptr);
		p++;
		num++;
		}
	}
	fclose(fp);
	return num;
}

//To get the description of all authorized users by reading /etc/passwd
void Get_user_description(char *user_name, char *description){

	int i;
    FILE *fp;
    char *ptr, *end, *folder_ptr;
    char Line_b[READ_LONG];

    if((fp = fopen("/etc/passwd","r")) == NULL)
	return;
    //printf("user_name = %s\n",user_name);
    while(fgets(Line_b, READ_LONG-1, fp) != NULL){
	if (Line_b[0] == '#' || Line_b[0] == '\r' || Line_b[0] == '\n')
	    continue;

	for(i=0;i<READ_LONG-1;i++){
	    if( ( (Line_b[i] == '\n') || (Line_b[i] == '\r')) ){
		Line_b[i] = '\0';
		i=READ_LONG;
	    }
	}

	ptr = Line_b;
	if (ptr != NULL){
	    end = strchr( ptr, ':');
	    if (end != NULL)
		*end = '\0';
	    if (strcmp(ptr,user_name) != 0)
		continue;

	    ptr = ++end;
	    if(ptr == NULL){
		fclose(fp);
		return;
	    }

	    if(strstr(ptr,"/mnt") && !strstr(ptr,"*")){
		folder_ptr = strstr(ptr,"/mnt");
		folder_ptr += 5;
		if((end = strchr(folder_ptr,':')) != NULL)
		    *end = '\0';
	    }
		    for(i=0; i<=2; i++){
			    end = strchr( ptr,':');
			    if(end != NULL)
				ptr = ++end;
		    }
		    end = strchr(ptr, ':');
		    if(end != NULL)
			*end = '\0';
	    strcpy(description,ptr);
	    #ifdef DEBUG1
	    printf("description = %s\n",description);
	    #endif
	    }
    }
    fclose(fp);
    return;
}

/*
//To get all the existent groups by reading /etc/group
//Return the number of existent groups
int Read_groupconf(void){

	FILE *fp;
	char *ptr, *end;
	char LineBuf[256];
	int num = 0, user_num = 0;

	memset(GROUP_set,0,sizeof(GROUP_VALUE)*MAX_GROUP);
	if((fp = fopen("/etc/group","r")) == NULL)
	    return num;
	else{
	    while(fgets(LineBuf,255,fp) != NULL){
		if(!strstr(LineBuf,"comment:")){
		    ptr = LineBuf;
		    if((end = strchr(ptr,':')) != NULL)
			*end = '\0';
		    strcpy(GROUP_set[num].name,ptr);

		    printf("GROUP_set[%d].name=%s\n", num, GROUP_set[num].name);

		    ptr = end + 1;
		    if((ptr = strrchr(ptr,':')) != NULL)
			ptr++;
		    ptr = strtok(ptr,",");
		if(ptr != NULL){
			user_num = 0;
			strcpy(GROUP_set[num].user[user_num],ptr);

			printf("GROUP_set[%d].user[%d]=%s\n", num, user_num, GROUP_set[num].name);

			while((ptr = strtok(NULL,",")) != NULL){
				user_num++;
				strcpy(GROUP_set[num].user[user_num],ptr);
			}
		}
	    }else if(strstr(LineBuf,"comment:") && strstr(LineBuf,GROUP_set[num].name)){
		if((ptr = strchr(LineBuf,':')) != NULL)
		    ptr++;
		strcpy(GROUP_set[num].description,ptr);

		printf("GROUP_set[%d].description=%s\n", num, GROUP_set[num].description);

		num++;
	    }
	}
	fclose(fp);
    }
    return num;
}
*/

//To get all the existent groups by reading /etc/group
//Return the number of existent groups
int Read_groupconf(void){

	FILE *fp;
	char *ptr, *end;
	char LineBuf[256];
	int num = 0, user_num = 0, i;

	/*
	for(i=0;i<5;i++){
    printf("groupconf-1;GROUP_set[%d].kind=%s\n",i,GROUP_set[i].kind);
    //strcpy(GROUP_set[i].kind_temp,GROUP_set[i].kind);
    //printf("after GROUP_set[%d].kind_temp=%s\n",i,GROUP_set[i].kind_temp);
	}

	//memset(GROUP_set,0,sizeof(GROUP_VALUE)*MAX_GROUP);

	for(i=0;i<5;i++){
    printf("groupconf-2;GROUP_set[%d].kind=%s\n",i,GROUP_set[i].kind);
    //strcpy(GROUP_set[i].kind_temp,GROUP_set[i].kind);
    //printf("after GROUP_set[%d].kind_temp=%s\n",i,GROUP_set[i].kind_temp);
	}
	*/

	if((fp = fopen("/etc/group","r")) == NULL)
	    return num;
	else{
	    while(fgets(LineBuf,255,fp) != NULL){
		if(!strstr(LineBuf,"comment:")){
		    ptr = LineBuf;
		    if((end = strchr(ptr,':')) != NULL)
			*end = '\0';
		    strcpy(GROUP_set[num].name,ptr);
		    //printf("Read_groupconf;GROUP_set[%d].name=%s\n",num,GROUP_set[num].name);
		    //printf("GROUP_set[%d].name=%s\n", num, GROUP_set[num].name);

		    ptr = end + 1;
		    if((ptr = strrchr(ptr,':')) != NULL)
			ptr++;
		    ptr = strtok(ptr,",");
		if(ptr != NULL){
			user_num = 0;
			strcpy(GROUP_set[num].user[user_num],ptr);
			//printf("GROUP_set[%d].user[%d]=%s\n", num, user_num, GROUP_set[num].name);
			while((ptr = strtok(NULL,",")) != NULL){  //key point
				user_num++;
				strcpy(GROUP_set[num].user[user_num],ptr);
				//printf("GROUP_set[%d].user[%d]=%s\n",num,user_num,GROUP_set[num].user[user_num]);
			}
		}
       }
	    else if(strstr(LineBuf,"comment:") && strstr(LineBuf,GROUP_set[num].name)){
	    //else if(strstr(LineBuf,"comment:") && strstr(LineBuf,","))
		if((ptr = strchr(LineBuf,':')) != NULL)
		    ptr++;
		strcpy(GROUP_set[num].description,ptr);
		//printf("GROUP_set[%d].description=%s\n", num, GROUP_set[num].description);
		num++;
	    }
	}
	fclose(fp);
	}
    //printf("num=%d\n",num);
    return num;
}

//Parse the string to get the information on new group
int parser_new_group(char *data){

	int num = 0;
	char *ptr, *end;
	//printf("data = %s\n",data);
	if((ptr=strstr(data,"group_name=")) != NULL){
		ptr+=11;
		end = strchr(ptr,'&');
		//if(end != NULL)
		*end = '\0';
		strcpy(NEW_GROUP.name,ptr);
		//printf("NEW_GROUP.name = %s\n",NEW_GROUP.name);
		ptr = end + 1;
	}
	if((ptr=strstr(ptr,"group_des=")) != NULL){
		ptr+=10;
		end = strchr(ptr,'&');
		//if(end != NULL)
		*end = '\0';
		strcpy(NEW_GROUP.description,ptr);
		//printf("NEW_GROUP.description = %s\n",NEW_GROUP.description);
		ptr = end + 1;
	}
	while((ptr=strstr(ptr,"user=")) != NULL){
		ptr+=5;
		end = strchr(ptr,'&');
		//if(end != NULL)
		*end = '\0';
		strcpy(NEW_GROUP.user[num],ptr);
		//printf("NEW_GROUP.user[%d] = %s\n",num,NEW_GROUP.user[num]);
		ptr = end + 1;
		num++;
	}
	//printf("count=%d\n",num);
	return num;
}

//To show the particular group information
void Ch_group(FILE *out){

	FILE *fp;
	char name[17], description[51], LineBuf[256];
	USER_DES_VALUE user_info[MAX_USER];
	int i, j, num = 0, count = 0, find = 0;

	ARGUMENT args[]={
	{"name",name,17},
	{NULL,NULL,0}
    };
    parseargs(args);

    if((fp = fopen("/htdocs/group_ch.htm","r")) == NULL)
	return;
    else{
	while(fgets(LineBuf,255,fp) != NULL)
	    fprintf(out,"%s",LineBuf);
	fclose(fp);
    }

    num = Get_group_information(name);
    #ifdef DEBUG1
    printf("num=%d\n",num);
    #endif
    /*
    fprintf(out,"<html><head><title>Gigabit SOHO NAS</title>\n");
    fprintf(out,"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n");
    fprintf(out,"<script language=\"JavaScript\">\n");
    fprintf(out,"function chk_folder(){\n");
    fprintf(out,"var name = document.soho.group_name.value;\n");
    fprintf(out,"if(name.indexOf(\"1\") == 0 || name.indexOf(\"2\") == 0 || name.indexOf(\"3\") == 0  || name.indexOf(\"4\") == 0\n");
    fprintf(out,"|| name.indexOf(\"5\") == 0 || name.indexOf(\"6\") == 0  || name.indexOf(\"7\") == 0  || name.indexOf(\"8\") == 0\n");
    fprintf(out,"|| name.indexOf(\"9\") == 0 || name.indexOf(\"0\") == 0 ){\n");
    fprintf(out,"alert(\"Please don't use the number to be folder name!!\");\n");
    fprintf(out,"document.soho.group_name.focus();\n");
    fprintf(out,"return history.go(0);}\n");
    fprintf(out,"}\n");
    fprintf(out,"</script>\n");
    fprintf(out,"</head><body>\n");
    */
    fprintf(out,"<P><FONT class=style9>&nbsp;&nbsp;Group Configuration</FONT></P>\n"
		"<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
		"<TBODY><TR>\n"
		"<TD width=\"100%%\" bgColor=#ffffff height=31>\n"
		"<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/sys_group.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
		"<TR height=5>\n"
		"<TD></TD></TR></TBODY></TABLE>\n");

#ifdef MICRONET
	fprintf(out,"<TABLE borderColor=#009999 cellSpacing=0 cellPadding=0 width=550 border=1><TBODY>\n");
#else
    fprintf(out,"<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1><TBODY>\n");
#endif

    fprintf(out,"<TR bgColor=#6defff>\n"
		"<TD class=style2 colSpan=3 height=30>&nbsp;&nbsp;Group - %s</TD></TR>\n",name);
    fprintf(out,"<tr><td width=\"050\" align=\"center\" rowspan=4><img src=\"images/group-s.gif\" border=0></td>\n");

#ifdef MICRONET
    fprintf(out,"<th width=\"150\" bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Group Name</th>\n");
#else
	fprintf(out,"<th width=\"150\" bgcolor=\"#dddddd\" align=left>&nbsp;&nbsp;Group Name</th>\n");
#endif

    fprintf(out,"<td width=\"350\" bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"group_name\" maxlength=15 size=20 value=%s></td></tr>\n",NEW_GROUP.name);
    fprintf(out,"<input type=\"hidden\" name=\"old_name\" value=%s>\n",NEW_GROUP.name);
#ifdef MICRONET
    fprintf(out,"<tr><th bgcolor=\"#eeffff\" align=left><font color=\"#006666\">&nbsp;&nbsp;Group Description</th>\n");
#else
	fprintf(out,"<tr><th bgcolor=\"#eeffff\" align=left>&nbsp;&nbsp;Group Description</th>\n");
#endif
    fprintf(out,"<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;<input type=\"text\" name=\"group_des\" maxlength=48 size=40 value=%s></td></tr>\n",NEW_GROUP.description);
    fprintf(out,"<tr><th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th><td bgcolor=\"#FFFFFF\">\n");
    fprintf(out,"1.Group Name is limited to 15 characters.<BR>\n");
    fprintf(out,"2.Discription is limited to 48 characters.<BR>\n");
    fprintf(out,"3.Characters allowed: a~z A~Z 0~9 ! $ ^ ( ) - { } . ~<br></td></tr>\n");
#ifdef LONGSHINE
    fprintf(out,"<tr><TD class=longshine_2 colSpan=3 align=center>&nbsp;&nbsp;\n"
		"&nbsp;&nbsp;<INPUT type=submit value=\"Apply\" name=id_set>&nbsp;&nbsp;&nbsp;&nbsp;<input type=reset value=\"Undo\">\n"
		"&nbsp;&nbsp;&nbsp;&nbsp;<INPUT onclick=\"history.back();\" type=button value=\"Back\" name=id_cancel></TD></tr>\n");
#else
    fprintf(out,"<tr bgcolor=\"#6defff\"><TD class=style2 height=30>&nbsp;&nbsp;</TD>\n"
		"<TD height=30>\n"
		"&nbsp;&nbsp;<INPUT type=submit value=\"Apply\" name=id_set>&nbsp;&nbsp;&nbsp;&nbsp;<input type=reset value=\"Undo\">\n"
		"&nbsp;&nbsp;&nbsp;&nbsp;<INPUT onclick=\"history.back();\" type=button value=\"Back\" name=id_cancel></TD></tr>\n");
#endif
    fprintf(out,"<tr bgColor=#FFFFFF>\n<td colspan=4>\n<table width=\"546\">\n"
		"<TR bgColor=#DDDDDD><TD height=30 colspan=3>&nbsp;&nbsp;Authorized Users</TD></tr>\n"
		"<MAP name=Map>\n"
		"<AREA shape=RECT coords=11,1,124,23 href=\"network-superadmin.htm\">\n"
			    "<AREA shape=RECT coords=129,2,228,24 href=\"admin-time.htm\">\n"
			    "<AREA shape=RECT coords=227,1,326,23 href=\"Get_alluser.cgi\">\n"
			    "<AREA shape=RECT coords=327,1,426,23 href=\"Get_allgroup.cgi\"></MAP>\n");

    if(num == 0){
	fprintf(out,"<tr bgcolor=\"#FFFFFF\"><td colspan=3 class=style4>No authorized users</td></tr>\n");
    }else{
	count = Get_allusername(user_info);
	#ifdef DEBUG1
	printf("count=%d;num=%d\n",count,num);
	#endif
		for(i=0;i<count;i++){
			Get_user_description(user_info[i].username,description);
			//printf("description = %s\n",description);
			strcpy(user_info[i].descrip,description);
		}
	for(i=0;i<count;i++){
		fprintf(out,"<TR bgColor=#FFFFFF>\n");
		find = 0;
		for(j=0;j<num;j++){

			//printf("user_info[%d].username = %s, NEW_GROUP.user[%d] = %s\n", i, user_info[i].username, j, NEW_GROUP.user[j]);
	    strtok(user_info[i].username,",");
	    strtok(NEW_GROUP.user[j]," ");
	    #ifdef DEBUG1
	    printf("After strtok user_info[%d].username = %s\n", i, user_info[i].username);
			printf("user_info[%d].username=%s with length=%d;NEW_GROUP.user[%d]=%s with length=%d\n",i,user_info[i].username,strlen(user_info[i].username),j,NEW_GROUP.user[j],strlen(NEW_GROUP.user[j]));
			#endif
			if(strstr(NEW_GROUP.user[j],user_info[i].username)){
			fprintf(out,"<TD width=050><input type=\"checkbox\" name=\"user\" value=%s checked></TD>\n",user_info[i].username);
			find = 1;
			break;
		  }
		}
		if(find == 0)
		    fprintf(out,"<TD width=050><input type=\"checkbox\" name=\"user\" value=%s></TD>\n",user_info[i].username);
		fprintf(out,"<TD width=090><img src=\"images/user-s.gif\" border=0><br>%s</TD>\n",user_info[i].username);
		fprintf(out,"<TD width=400>%s</td></tr>\n",user_info[i].descrip);
		fprintf(out,"<TR bgColor=#FFFFFF>\n");
		fprintf(out,"<TD colspan=3><hr></TD></tr>\n");
	     }
	}
	fprintf(out,"</table>\n<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",END_MARK,END_MARK);
    fprintf(out,"</form></TBODY></TABLE></BODY></HTML>\n");
}

//To get the information of patticular group by reading /etc/group
//Return the number of user of particular group

int Get_group_information(char name[17]){

	FILE *fp;
	char *ptr, *end;
	char LineBuf[256];
	int user_num = 0, find = 0;

	if((fp = fopen("/etc/group","r")) != NULL){
	    while(fgets(LineBuf,255,fp) != NULL){
		if(strstr(LineBuf,name) && !strstr(LineBuf,"comment")){
			strcpy(NEW_GROUP.name,name);
			ptr = strrchr(LineBuf,':');
			ptr++;

			 if(strstr(ptr,",") != NULL){
			   ptr = strtok(ptr,",");

		if(ptr != NULL){
			user_num = 0;
			strcpy(NEW_GROUP.user[user_num],ptr);
			while((ptr = strtok(NULL,",")) != NULL){
				user_num++;
				if((end = strchr(ptr,'\n')) != NULL)
				    *end = '\0';
				strcpy(NEW_GROUP.user[user_num],ptr);
				//NEW_GROUP.user[user_num][strlen(NEW_GROUP.user[user_num])+1] = '\0';
				//printf("NEW_GROUP.user[%d] = %s\n",user_num,NEW_GROUP.user[user_num]);
			}
		}
	   }
	   else{
	     strcpy(NEW_GROUP.user[user_num],ptr);
	     #ifdef DEBUG1
	     printf("NEW_GROUP.user[%d]=%s\n", user_num, NEW_GROUP.user[user_num]);
	     #endif
	     if((end = strchr(ptr,' ')) != NULL)
		  *end = '\0';
	   }
      }

	    if(strstr(LineBuf,name) && strstr(LineBuf,"comment")){
		ptr = strrchr(LineBuf,':');
		ptr++;
		strcpy(NEW_GROUP.description,ptr);
		break;
	    }
	}
	fclose(fp);
    }
    user_num++;
    return user_num;
}

//To delete the selected group in /etc/smb.conf
void find_del_group(char *ptr, int count, FILE *fo){

	int i, num=0;
	char flag = 0;
	char *end;

	while(ptr != NULL){
		end = strchr(ptr,',');
		if(end != NULL )
		    *end = '\0';
		if(*ptr == '@'){
			ptr++;
		    for(i=0;i<count; i++){
			    if(!strcmp(ptr,DEL_GROUP[i])){
				    flag=1;
				    break;
			    }
			}
		}
		if(!flag){
			if(num==0){
			fprintf(fo," %s", ptr);
			num=1;
		    }else
			fprintf(fo,",%s", ptr);
			fflush(fo);
		}else
		    flag=0;
		if( end == NULL )
		    break;
		ptr = ++end;
	}
	fprintf(fo,"\n");
	return;
}

/**** hier was Set_usbdisk, set_usbprinter, set_usbque, Printer_scrpage, Show_USB_printer ****/

/*
Set_codepage(FILE *out){

	char code_page[20];

	ARGUMENT args[]={
									  {"code_page", code_page, 20},
									{NULL,NULL,0}
		  };
    parseargs(args);

	printf("code_page=%s\n",code_page);

	strcpy(CONFIG.CODE_SET,code_page);
	SaveConfiguration();
	boot_page(out,"Show_maintain.cgi");
}
*/

/*
void Set_codeset1(FILE *out){

    FILE *fp, *n_fd, *data1;
    char *data, LineBuf[256];
    char character_set[20], code_page[20], code_system[20], data_tmp[128], data1_tmp[128];

    ARGUMENT args[]={
	{"code_page",CONFIG.CODE_SET,20},
	{NULL,NULL,0},
    };
    printf("CONFIG.CODE_SET = %s\n", CONFIG.CODE_SET);

    strcpy(data_tmp,arg);
    strcpy(data1_tmp,arg);

    data1 = data1_tmp;		//something strange!! RAW
    data = data_tmp;
    printf("data = %s; data1 = %s\n", arg, data1);
    parseargs(args);
    printf("CONFIG.CODE_SET = %s; data = %s; data1 = %s\n", CONFIG.CODE_SET, arg, data1);

    if(!strcmp(CONFIG.CODE_SET,"437-8859-1")){
	strcpy(character_set,"ISO8859-1");
	strcpy(code_page,"437");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"737-8859-7")){
	strcpy(character_set,"ISO8859-7");
	strcpy(code_page,"737");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"775-8859-15")){
	strcpy(character_set,"ISO8859-15");
	strcpy(code_page,"775");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"775-8859-13")){
	strcpy(character_set,"ISO8859-13");
	strcpy(code_page,"775");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"850-8859-1")){
	strcpy(character_set,"ISO8859-1");
	strcpy(code_page,"850");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"850-8859-15")){
	strcpy(character_set,"ISO8859-15");
	strcpy(code_page,"850");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"1252-8859-15")){
	strcpy(character_set,"ISO8859-2");
	strcpy(code_page,"1252");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"852-8859-2")){
	strcpy(character_set,"ISO8859-2");
	strcpy(code_page,"852");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"857-8859-9")){
	strcpy(character_set,"ISO8859-9");
	strcpy(code_page,"857");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"866-8859-5")){
	strcpy(character_set,"ISO8859-5");
	strcpy(code_page,"866");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"866-KOI8-R")){
	strcpy(character_set,"KOI8-R");
	strcpy(code_page,"866");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"866-1251")){
	strcpy(character_set,"1251");
	strcpy(code_page,"866");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"932-sjis-")){
	strcpy(character_set,"ISO8859-15");
	strcpy(code_page,"932");
	strcpy(code_system,"sjis");
    }

#ifdef NH221
    else if(!strcmp(CONFIG.CODE_SET,"874-8859-11-")){
	strcpy(character_set,"ISO8859-11");
	strcpy(code_page,"874");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"860-8859-15-")){
	strcpy(character_set,"ISO8859-15");
	strcpy(code_page,"860");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"936-8859-15-")){
	strcpy(character_set,"");
	strcpy(code_page,"936");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"949-8859-15-")){
	strcpy(character_set,"");
	strcpy(code_page,"949");
	strcpy(code_system,"");
    }else if(!strcmp(CONFIG.CODE_SET,"950-8859-15-")){
	strcpy(character_set,"");
	strcpy(code_page,"950");
	strcpy(code_system,"");
    }

    printf("character_set = %s; code_page = %s\n",character_set, code_page);

    if((fp = fopen("/etc/smb.conf","r")) == NULL)
	return;
	  if((n_fd = fopen("/etc/smb1.conf","w+")) == NULL)
	    return;

    while(fgets(LineBuf,255,fp) != NULL){
		if(strstr(LineBuf, "character set =") != NULL){
			 printf("parser character set =\n");
		    fprintf(n_fd,"   character set = %s\n",character_set);
			continue;
		}else if(strstr(LineBuf, "client code page =") != NULL){
			  printf("parser client code page =\n");
			fprintf(n_fd, "	  client code page = %s\n",code_page);
			continue;
		}else if (strstr(LineBuf, "coding system =") != NULL){
			printf("parser coding system =\n");
			fprintf(n_fd, "	  coding system = %s\n",code_system);
			continue;
		}else
			fputs(LineBuf,n_fd);
    }
    fclose(fp);
    fclose(n_fd);
    system("/bin/cp /etc/smb1.conf /etc/smb.conf");
    system("cat /etc/smb.conf");
    system("/bin/rm -f /etc/smb1.conf");
#endif

    SaveConfiguration();
    //my_system(out,"/etc/reboot &");
//    Restart_System( "page=upload" , out );
    Restart_System( out );

    return;
    //my_system(out,"/etc/reset");

    fprintf(out,"<script language=\"JavaScript\">\n");
#if defined(LOBOS)
    fprintf(out,"location.replace('http://%s:%s/lb_ver.htm');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
#elif defined(NOVAC)
    fprintf(out,"location.replace('http://%s:%s/nv_ver.htm');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
#else
    fprintf(out,"location.replace('/Show_maintain.cgi');\n");
    fprintf(out,"location.replace('http://%s:%s/Show_maintain.cgi');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
#endif
    fprintf(out,"</script>\n");
}
*/
/*
void Set_Conf(FILE *out){
  #ifdef DEBUG1
	  printf("hdcapacity=%d\n",hdcapacity);
	#endif
  FILE *fp, *fi;
  long i = 0, j, b_len, t_len, check_result;
  char *ptr, data_tmp[20];
  char *post_data;
  char *boundary, *var_name, *buf1, *newbuf, *tmps;
  char *folder, *temp, *end;
  char LineBuf[256], *storage = "/tmp/config.tgz";

  //strcpy( data_tmp , data );

//sp
#ifdef HTTPD_MAX_ACCEPT
	add_num_crt();
	current_num_crt =1;
#endif//#ifdef HTTPD_MAX_ACCEPT

    post_data = (char *)malloc(content_length+1);
    fread(post_data,content_length,1,out);
    #ifdef DEBUG1
	printf("!!!!!!post_data = %s!!!!!!\n",post_data);
    #endif
//    boundary = strstr(content_type, "-------");
//    b_len = strlen(boundary);
    var_name = (char *) malloc(strlen("ufile3") + 60);
    (void) strcpy(var_name, "Content-Disposition: form-data; name=\"");
    (void) strcat(var_name, "ufile3");
    (void) strcat(var_name, "\"");

    buf1 = (char *) memmem_in(post_data, content_length, var_name, &i);
    #ifdef DEBUG1
	printf("buf1 = %s\n", buf1);
    #endif
    if(buf1){
	(void)strcat(var_name, "; filename=\"");
	newbuf = (char *) memmem_in(post_data, content_length, var_name, &i);
	if(newbuf){
	    newbuf += strlen(var_name);
	    b_len = strlen(newbuf);
	    tmps = strchr(newbuf, '"');
	    if(tmps == NULL)
		return;

	    t_len = strlen(tmps);

	    if(b_len == t_len){
		printf("write_upload_file returned UPLOAD_FIELD_EMPTY 1\n");
		boot_page(out,"Show_maintain.cgi");
		return;
	    }

	    b_len = content_length - i;	   // newbuf length, i is the offset

	    i = 0;
	    tmps = (char *) memmem_in(newbuf, b_len, "\r\n\r\n", &i);	// find data starting point

	    if((i >= 0) && (b_len - i)){
		newbuf = (char *) memmem_in(tmps, (b_len - i), boundary, &t_len);
		if((t_len >= 0) && (b_len - i - t_len)){// found end boundary, t_len is the offset to boundary from tmps
		    tmps += 4;
		    t_len -= 4;
		    if ((fp = fopen(storage, "wb")) == NULL){
			printf("Couldn't open fpath_name(wb) to write!! \n");
			return;
		    }
		    printf("start to write!\n");
		    if (  fwrite(tmps, t_len, 1, fp) != 1 ){
			printf("writinf files fails\n");
			return;
		    }
		    printf("writing ... Please wait!! \n");
		    fclose(fp);
		    free(post_data);
		    free(var_name);
#if 0 //RAW
		    system("/bin/dd if=/tmp/config.tgz of=/dev/mtdblock3 >/dev/null 2>&1"); // for NH221
#endif
		    //system("cd /mnt;tar zxvf /mnt/ntx.tgz;chmod 755 /mnt/ntx_chset.h /mnt/upnp_soho /mnt/watch_prog");
		    //system("/etc/reboot");
		    //Wait_page( out, 1, "/Upgrade_page.cgi" );
		    //fprintf(out,"<script language=\"JavaScript\">\n");
		    //fprintf(out,"location.replace('http://%s:%s/admin-codeset.htm');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
		    //fprintf(out,"</script>\n");
		    //fclose(out);
		    strcpy(data_tmp,"page=upload");
		    printf("data_tmp = %s\n",data_tmp);
		    ptr = data_tmp;
		    printf("ptr = %s\n",ptr);
		    printf("point 5!!\n");
//		    Restart_System( ptr , out );
		    Restart_System( out );
		    return;
		    //system("/etc/reboot");
		}
		printf("write_upload_file returned UPLOAD_FILE_EMPTY x1\n");
	    }
	    printf("write_upload_file returned UPLOAD_FILE_EMPTY x2\n");
	}
	printf("write_upload_file returned UPLOAD_FIELD_EMPTY x3\n");
    }
    printf("write_upload_file returned UPLOAD_CALL_FAIL 6 \n");
    printf("download configuration file to PC suceessfully!!\n");
    boot_page(out,"Show_maintain.cgi");
}
*/

void Set_ntx_web_update(FILE *out){

		FILE *fp, *fip;
		char Line_buf[READ_SHORT+1],*data1,command[50];
		int i, num=0, num_ntx=0;
		char num2[5], num3[5], data_tmp[20], web_pass =1 , ntx_pass = 1;

    printf("starting Set_ntx_web_update.cgi!!\n");
    if(initial == 1){
    sprintf(command,"/etc/rc.d/do_upgrade.sh %s &", URL_global);
    system(command);
	initial = 0;
    }
    ARGUMENT args[]={
	{"web_percent", num2, 4},
	{"ntx_percent", num3, 4},
	{NULL,NULL,0}
    };
    parseargs(args);
   for(i=0;i<20;i++){
		if(web_pass == 1){
	   if((fip = fopen("/tmp/fm_fail","r")) == NULL){
	       open_web = 1;
	       if((fip = fopen("/tmp/web_percent","r")) == NULL){
			open_web = 0;
			 }
			 else{
				open_web =1; // jump point
				web_pass = 0;
				fclose(fip);
			 }
	   }
	   else{
	     open_web =0; // jump point
	     web_pass = 0;
	     fclose(fip);
	   }
	  }
	  if(ntx_pass == 1){
	   if((fip = fopen("/tmp/ntx_fail","r")) == NULL){
	      open_ntx = 1;
	      if((fip = fopen("/tmp/ntx_percent","r")) == NULL){
			open_ntx = 0;
			 }
			 else{
				open_ntx =1; // jump point
				ntx_pass = 0;
				fclose(fip);
			 }
	   }
	   else{
	     open_ntx =0; // jump point
	     ntx_pass = 0;
	     fclose(fip);
	   }

	  }
	   if(web_pass == 0 && ntx_pass == 0)
	     goto lightbar;

	   sleep(1);

	 }
	   lightbar:
	   printf("open_web=%d;open_ntx=%d\n", open_web, open_ntx);
    if(open_web == 0 && open_ntx == 0){
       fprintf(out,"<script language=\"JavaScript\">\n");
		   fprintf(out,"alert(\"The firmware update are failure!!\");\n");
		   fprintf(out,"location.href =\"Show_maintain.cgi\";\n");
		   fprintf(out,"</script>\n");
    }
		if(open_web == 1){
		// === open web_percent === //
		    if ((fp = fopen("/tmp/web_percent","r")) == NULL )
				    num = atoi(num2) ;
				else{
					fgets( Line_buf, READ_SHORT, fp );
					for(i=0; i<READ_SHORT-1; i++){
						if( Line_buf[i] == '\r' || Line_buf[i] == '\n'){
							Line_buf[i] = '\0' ;
							i = READ_SHORT;
						}
					}
					num = atoi(Line_buf);
				    fclose(fp);
				}
		}
		if(open_ntx == 1){
		// === open ntx_percent === //
				if ((fp = fopen("/tmp/ntx_percent","r")) == NULL )
				    num_ntx = atoi(num3);
				else{
				    /*
						if( atoi(num3) == 100 && atoi(num2) == 100 ){
							 if ((fp = fopen("/tmp/writeflash","r")) == NULL )
					   NULL;
				       else{
					   fprintf(out,"<SCRIPT language=JavaScript>\n");
							     fprintf(out,"self.open('updatepop_autoupdate.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");
							     fprintf(out,"</SCRIPT>\n");
							 }
							return;
					  }
					  */
					  fgets( Line_buf, READ_SHORT, fp );
					for(i=0; i<READ_SHORT-1; i++){
						if( Line_buf[i] == '\r' || Line_buf[i] == '\n'){
							Line_buf[i] = '\0' ;
							i = READ_SHORT;
						}
					}
					num_ntx = atoi(Line_buf);
				    fclose(fp);
				}
		}

		if(open_web == 1 && open_ntx == 1){
			if( atoi(num3) == 100 && atoi(num2) == 100 ){
				 fprintf(out,"<SCRIPT language=JavaScript>\n");
				 fprintf(out,"self.open('updatepop_autoupdate.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");
				 fprintf(out,"</SCRIPT>\n");
				 sprintf(command,"gunzip -c /mnt/%s | tar xf - -C /mnt && rm -f /mnt/%s\n", ntx_firmware, ntx_firmware);
				 system(command);

				 sprintf(command,"/etc/rc.d/firmware_upgrade.sh %s&\n", web_firmware);
				 my_system(out,command);
				 return;
			}
		}
		else if(open_web ==1 && open_ntx == 0){
			if( atoi(num2) == 100 ){
				 fprintf(out,"<SCRIPT language=JavaScript>\n");
				 fprintf(out,"self.open('updatepop_autoupdate.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");
				 fprintf(out,"</SCRIPT>\n");
				 //sleep(10);
				 sprintf(command,"/etc/rc.d/firmware_upgrade.sh %s&\n", web_firmware);
				 my_system(out,command);

				 return;
			}
		}
		else if(open_web ==0 && open_ntx == 1){
			if( atoi(num3) == 100 ){
				 fprintf(out,"<SCRIPT language=JavaScript>\n");
				 fprintf(out,"self.open('updatepop_autoupdate.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");
				 fprintf(out,"</SCRIPT>\n");
				 sprintf(command,"gunzip -c /mnt/%s | tar xf - -C /mnt && rm -f /mnt/%s\n", ntx_firmware, ntx_firmware);
				 printf("ntx_command=%s\n",command);
				 system(command);
				 sprintf(command,"/etc/rc.d/firmware_upgrade.sh&\n");
				 my_system(out,command);
			   return;
			}
		}
    if((fp=fopen("/htdocs/JavaReturn_maintain.htm","r")) == NULL)
	return;
    else{
	while(fgets(Line_buf,READ_SHORT,fp) != NULL)
	    fprintf(out,"%s",Line_buf);
	fclose(fp);
    }
       fprintf(out,"<td width=\"100%%\" bgcolor=\"#FFFFFF\">\n<font size=3>&nbsp;&nbsp;Auto Firmware Update</font><br><br>\n");
       fprintf(out,"&nbsp;&nbsp;This may take a few minutes.<br>&nbsp;&nbsp;Please wait for the system to reload the page. Do not close this window.<br>\n");
       fprintf(out,"</td>\n</tr>\n<tr height=\"005\"><td></td></tr>\n</table><br>\n");
		if(open_web == 1){
		   fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");
		   fprintf(out,"<tr><td><font size=3>download web_update file</font></td></tr></table>\n");
		// === show web_percent file percentage bar === //
		  fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");
			for(i=0; i<20; i++){
				if( i < (num/5) )
					fprintf(out, "<td width=4%% bgcolor=\"3A48FF\">&nbsp;</td>");
				else
					fprintf(out, "<td width=4%% bgcolor=\"AEAEAE\">&nbsp;</td>");
			}
		  fprintf(out,"</tr><tr><td width=4%%>0%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>100%%</td></tr></table>");
		}
		if(open_ntx == 1){
		// === show ntx_percent file percentage bar === //
		  fprintf(out,"<br><table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");
		   fprintf(out,"<tr><td><font size=3>download ntx_update file</font></td></tr></table>\n");
		  fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");
			for(i=0; i<20; i++){
				if( i < (num_ntx/5) )
					fprintf(out, "<td width=4%% bgcolor=\"3A48FF\">&nbsp;</td>");
				else
					fprintf(out, "<td width=4%% bgcolor=\"AEAEAE\">&nbsp;</td>");
			}
		  fprintf(out,"</tr><tr><td width=4%%>0%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>100%%</td></tr></table>");
		}
	fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",5000);\nreturn true;}\nfunction reloadurl(){location.replace('/Set_ntx_web_update.cgi?web_percent=%d&ntx_percent=%d');}</script><br></body></html>\n", num, num_ntx);
	//sleep(15);
	return ;
}

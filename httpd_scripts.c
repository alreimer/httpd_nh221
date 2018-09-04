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

void get_hdused( void );

int  Cmp_folder( char *foldername);

void Set_hdformat_m(FILE *out);

void Get_allfolder(FILE *out);
void Get_allfolder_m( unsigned char *count, FOLDER_VALUE *folder_name );
void Get_folderuser(FILE *out);
void Get_folderuser_m( char *folder_name, FILE *out);
void Get_folder_user_all( char *folder_name, USER_VALUE *folder_user, char *flag);

void Ch_userinfo(FILE *out);
#ifdef NOVAC
    int	 Ch_userinfo_m(char *user_name, char *user_des);
#else
    int	 Ch_userinfo_m(char *user_name, char *user_des, char *ftp_user, char *ftp_folder, char *ftp_admin);
#endif
void find_mntfolder(FILE *out);
int  find_mntfolder_m( char *count, FOLDER_VALUE *hd_folder);
void Set_addmntfolder(FILE *out);
void Ch_ftppasswd(FILE *out);

void parse_ufile_data( char *ufile );
//int  write_upload_file( char *que, char *fpath_name, FILE *out);
void *memmem_in(char *haystack, long haystack_len, char *needle, unsigned long long *i);
int  check_firmware_version( char *check_buf );

//======Firmware upgrade======
void Reboot_page1(FILE *out, char *filename);
//============================
void Systembusy_page( FILE *out, char *go_url );
void Warmming_page( FILE *out, char *message, char *go_url);
void warmming_specialname( FILE *out, char *username, char *go_url);
//================ declare the uplaod and download cgi ================
//void Set_Conf(FILE *out);
//=====================================================================
//================ ntx_update and web_update ==========================
void Set_ntx_web_update(FILE *out);
//=====================================================================
//===========Character set file============
//void Set_codeset1(FILE *out);
//=========================================
//===============Service_Number for option iterm================
//void Set_service(FILE *out);
//==========================================
//char * get_unicode(void);

//=============Set Code Page==============
//void Set_codepage(FILE *out);
//==========================================

CGI_ENTRY cgi_entries[] = {
    {"_show_CGIs.cgi",show_CGIs},		//used from copy_CGI.c
/*
    {"Get_allfolder.cgi",Get_allfolder},
    {"Get_folderuser.cgi",Get_folderuser},
    {"Ch_username.cgi",Ch_username},
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
//================ declare the upload and download cgi ================
//    {"Set_Conf.cgi",Set_Conf},
//=====================================================================
//======= CGI Function for Group =============
    {"Add_Group.cgi",Add_Group},
/****************** moved to rc_maintain ********************************************
//=============== Syslog ===============
//    {"Show_syslog.cgi",Show_syslog},
//==========================================
//************************************************************************************
//================ Samba ===============
//    {"samba.cgi",samba},
//============== Dropbear ===============
//    {"Set_dropbear.cgi", Set_dropbear},
*/    {"",NULL}
};

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
	    //get_hdcapacity();
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
	//print_user_group(folder_name,user_list,num,out);
}


void Get_folder_user_all( char *folder_name, USER_VALUE *folder_user, char *flag){

}

//To get the group access right of particular folder by reading /etc/smb.conf
int Get_folder_group_all(char *folder_name, char *flag){

    //return num;
    return 0;
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
		return;//made by RAW because of Systembusy_page is removed and here must stay something
		//Systembusy_page(out, "location.replace('/find.htm')");
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
		//Wait_page( out, 1, "/FTP.htm" );
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

extern int method1;
extern char *postdata;

//name:size_limit:firmware:file_name
int save_bfile_1(FILE *out, char *form_name, unsigned long long size_limit, char *firmware, char *file_name){

    int jump = 0;
    unsigned long long i = 0, b_len, t_len, size;
    char bound[1060], *var_name = NULL, *file, *tmps;

    if(method1 != POST_BOUNDARY || !postdata) return 0;

printf("content_length = %d\n",content_length);

    var_name = (char *) malloc(strlen(form_name) + 60);
    if(!var_name) return 0;

    sprintf(var_name, "Content-Disposition: form-data; name=\"%s\"; filename=\"", form_name);
    file = (char *) memmem_in(postdata, content_length, var_name, &i);
    free(var_name);

    if(file && (*file != '\"')){
	    tmps = strchr(file, '\"');
	    if(tmps == NULL){
		fprintf(out, "\" - not found\n");
		return 0;
	    }
	    *tmps = '\0';

	    size = tmps - file;	//size of file_name
	    b_len = content_length - (tmps - postdata);	   // from tmps length, i is the offset

	    tmps = (char *) memmem_in(tmps, b_len, "\r\n\r\n", &i);	// find data starting point

	    if(i  && (b_len - i)){
		b_len = b_len - i - 4;

		sprintf(bound, "\r\n--%s", boundary);//0x0d,0x0a
		memmem_in(tmps, b_len, bound, &t_len);
		if(t_len > 0){// found end boundary, t_len is the offset to boundary from tmps
		    if(size_limit && t_len > size_limit){
			fprintf(out, "File size:%lld > Size limit: %lld\n", t_len, size_limit);
			return 0;
		    }
		    jump = reg_par(file_name, file, size);
		    if(jump){
			jump = reg_par(firmware, tmps, t_len);
		    }
		}
	    }
	}
return jump;
}

/*search needle in haystack and return the end of needle
    i - is offset from haystack to needle pointer
*/
void *memmem_in(char *haystack, long haystack_len, char *needle, unsigned long long *i)	/*other places witch use this function must be changed to end of needle*/
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

    fprintf(out,"');\n</script>\n<noscript>Follow <a href=\"http://%s:%s/", CONFIG.IP, CONFIG.ADMIN_PORT );
    print(out, filename);
    fprintf(out, "\">link</a></noscript>");
    //fprintf(out,"window.location.href=\"%s\";\n",filename);
}

/*
void reboot(void)
{
	system("kill -SIGTERM 1");
	//my_system("kill -SIGTERM 1");
	return;
}
*/

//remove all that _pages, RAW

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

//b_in(buffer) and s_in(size) are /dev/stdin. If (b_in == NULL) ->/dev/stdin is OFF!
//buf and size are std(out, err) buffers.
void write_system(char *b_in, long long s_in, char *buf, long long size, int mode, char *cmd)
{
	int pid, status;
	int pipe_fd[2], pipe_[2]; //pipe_fd - is for read(stdout stderr) and pipe_ - is for write(stdin)
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
      if (pipe(pipe_) == 0) {
	if (!(pid=fork())){	//child
	    close(pipe_fd[0]);	//read end

	    close(fd);	    	//for inetd
	    close(sockfd);

	    close(1);
	    close(2);
	    dup2(pipe_fd[1], 1);	//1 -stdout
	    dup2(pipe_fd[1], 2);	//2 -stderr
	    close(pipe_fd[1]);

	    close(pipe_[1]);	//write end
	    close(0);
	    dup2(pipe_[0], 0);	//1 -stdin
	    close(pipe_[0]);

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);

	    exit(1);
	}else{			//parent
	    close(pipe_[0]);	//read end
	    close(pipe_fd[1]);	//write end
	    if (pid > 0) {	// fork -- parent

		if(b_in != NULL){
		    if(s_in == 0) s_in = strlen(b_in);
		    if(s_in != 0) write(pipe_[1], b_in, s_in);
		}
		close(pipe_[1]);

		while(i<(size-1) && read(pipe_fd[0], (buf+i), 1) > 0){
//printf("n=%d w=%d",n,wrote);
//		    if(i >= (size-1)) break;
		    i++;
		}
		buf[i] = '\0';
		wait(&status);
	    } else {	//fork failed
		err = 2;
		close(pipe_[1]);
	    }

	    close(pipe_fd[0]);	//close rest
	}
      }else{
        err = 1;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
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

	    close(0);	//	/dev/stdin is off

//	    setsid();
	    execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
//	    execl("/bin/sh", "sh", cmd, (char *) 0);
	    exit(1);
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

	    close(0);	//	/dev/stdin is off

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


//clear this all!
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

/*
//======= CGI Function for USB by snop===============
void Show_USB_Detail(char *data, FILE *out);
void Show_USB_Disk(char *data, FILE *out);
void Set_USB_Disk(char *data, FILE *out);
//void Set_usbdisk(char *data, FILE *out);
//void Set_usbque(char *data, FILE *out);
//void Set_printer(char *data, FILE *out);
//======= CGI Function for USB by allen=====================
void Show_USB_Printer(char *data, FILE *out);
void Set_printer(char *data, FILE *out);
void Set_usbque(char *data, FILE *out);
void Printer_scrpage( char *data, FILE *out );//allen adds to show printer saving info.
//==================================================
//=============Set usbadvanced==============
//void Set_usbadvanced(char *data, FILE *out);
void Set_usbhdformat(char *data, FILE *out);
void Set_usbhdformat_m(char *data, FILE *out);
//==========================================
//=============Show advanced_func1==============
void Show_advanced_func1(char *data, FILE *out);
//==========================================
//================ USB copy ================
void Set_usbcopy(char *data, FILE *out);
//============================================
*/
//============declararion USB variable=============
int usb_first_start=1;
int printer_first_start=1;
//USB_DISK usb_disk[0];
//USB_PRINTER usb_printer[0];
//=================================================


int  Set_delfolder_m_usbmount( char count, FOLDER_VALUE *delfolder )	/*used only in this file - such a sheet!!! RAW*/
{
	FILE *fp,*n_fd;
	char Line_b[READ_LONG];
	FOLDER_VALUE *p;
	char *ptr, *end;
	int i;
	//char f=0;  //snop declare variable
	int f=0;  //allen revise declare variable
	char tmp[MAX_BUF];

	if((fp = fopen("/etc/smb.conf","r")) == NULL)	return 0;
	if((n_fd = fopen("/etc/smb1.conf","w+")) == NULL)	return 0;

	while(fgets(Line_b, READ_LONG-1, fp) != NULL)
    {
//		if ( Line_b[0] == '\r' || Line_b[0] == '\n')  continue;

	if ( Line_b[0] == '[' )
	{
	    ptr = Line_b;
	    end = strchr(ptr,'[');
	    if (end != NULL) *end = '\0';
	    ptr = ++end;
	    end = strchr(ptr,']');
	    if (end != NULL) *end = '\0';
#if defined(LOBOS) || defined(NOVAC) || defined(CENTURY)
	    if ( strcmp( ptr, "global") == 0 )	  //ptr here is the folder name
#else
    #ifndef NH220
	    if ( strcmp( ptr, "global") == 0 || strcmp( ptr, FTP_SHARE) ==0 )
	    //if ( strcmp( ptr, "global") == 0 || strcmp( ptr, FTP_SHARE) ==0 || strcmp( ptr, SMB_SHARE) ==0 )
    #else
	    if ( strcmp( ptr, "global") == 0 )
	    //if ( strcmp( ptr, "global") == 0 || strcmp( ptr, SMB_SHARE) ==0 )
    #endif
#endif
	    {
		fprintf(n_fd, "[%s]\n", ptr);
		continue;
			}
	    p = delfolder;
	    for( i=0; i<count; i++)
	    {
		if ( strcmp( ptr, &(p->foldername[0]) ) == 0)
		{
			f = 1;
			break;
		}
		p++;
		}

	if(f == 1){
				while(fgets(Line_b, READ_LONG-1, fp) != NULL){
					if ( strstr(Line_b, "create mask =") != NULL || strstr(Line_b,"admin users =")){
						//fgets(Line_b, READ_LONG-1, fp);
				f = 0;
						sprintf( tmp, "/bin/rm -fr /mnt/%s > /dev/null 2>&1", &(p->foldername[0]) );
						system( tmp );
						//my_system( tmp );
						break;
					}
			 }
     }
     else{
	    fprintf(n_fd, "[%s]\n", ptr);
     }
		}
		else{
		     fputs(Line_b, n_fd);
		}
	}

	fclose(fp);
    fclose(n_fd);

	return 1;
}



void Show_USB_Disk(FILE *out){

    FILE *fp;
    char *ptr, *end;
    char LineBuf[256], folder_name[20];
    USB_DISK USB_DATA1[4];
    int i, j, k, num = 0, block = 0, count = 0, mounted = 0, index = 0, tmp, value = 0;

    for(i=0;i<2;i++){
	    //memset(usb[i].hddevice,0,5);
	    strcpy(usb[i].hddevice,"");
	    //printf("usb[%d].hddevice=%s\n",i,usb[i].hddevice);
	  }
    system("/etc/rc.d/usb_info.sh 1 > /tmp/usb.log");
    if((fp = fopen("/tmp/usb.log","r")) != NULL){
	while(fgets(LineBuf,255,fp)){
		if(strstr(LineBuf,"sda") != NULL && strstr(LineBuf,"sda1") == NULL && strstr(LineBuf,"sda2") == NULL
		&& strstr(LineBuf,"sda3") == NULL && strstr(LineBuf,"sda4") == NULL){
			if(strlen(usb[0].hddevice) == 0)
			strcpy(usb[0].hddevice,"sda");
			else
			strcpy(usb[1].hddevice,"sda");
		}
		if(strstr(LineBuf,"sdb")!= NULL && strstr(LineBuf,"sdb1") == NULL && strstr(LineBuf,"sdb2") == NULL
		&& strstr(LineBuf,"sdb3") == NULL && strstr(LineBuf,"sdb4") == NULL){
			if(strlen(usb[0].hddevice) == 0)
			strcpy(usb[0].hddevice,"sdb");
			else
			strcpy(usb[1].hddevice,"sdb");
		}
		if(strstr(LineBuf,"sdc")!= NULL && strstr(LineBuf,"sdc1") == NULL && strstr(LineBuf,"sdc2") == NULL
		&& strstr(LineBuf,"sdc3") == NULL && strstr(LineBuf,"sdc4") == NULL){
			if(strlen(usb[0].hddevice) == 0)
			strcpy(usb[0].hddevice,"sdc");
			else
			strcpy(usb[1].hddevice,"sdc");
		}
		if(strstr(LineBuf,"sdd")!= NULL && strstr(LineBuf,"sdd1") == NULL && strstr(LineBuf,"sdd2") == NULL
		&& strstr(LineBuf,"sdd3") == NULL && strstr(LineBuf,"sdd4") == NULL){
			if(strlen(usb[0].hddevice) == 0)
			strcpy(usb[0].hddevice,"sdd");
			else
			strcpy(usb[1].hddevice,"sdd");
		}
		if(strstr(LineBuf,"partitions=")){
			ptr = strstr(LineBuf,"partitions=");
			ptr+=11;
			end = strchr(ptr,':');
			if(end != NULL)
			    *end = '\0';
			USB_DATA1[num].partition = atoi(ptr);
			block = 0;
			continue;
		}
		if(strstr(LineBuf,"filetype=")){
			ptr = LineBuf;
			end = strchr(LineBuf,':');
			if(end != NULL)
				*end = '\0';
		    strcpy(USB_DATA1[num].device[block],ptr);
			ptr = end + 1;
			ptr+=9;
			end = strchr(ptr,':');
			if(end != NULL)
				*end = '\0';
			strcpy(USB_DATA1[num].type[block],ptr);
			ptr = end + 1;
			ptr+=6;
   //====== separate the size and port string ======
			end = strchr(ptr,':');
			if(end != NULL)
			    *end = '\0';
			strcpy(USB_DATA1[num].size[block],ptr);
			block++;
			if(block == USB_DATA1[num].partition)
			   num++;
		}
   //===============================================

			//ptr = end + 1;
			//printf("ptr = %s\n", ptr);

		/*
			end = strchr(ptr,'\n');
			if(end != NULL)
			    *end = '\0';
			strcpy(USB_DATA1[num].size[block],ptr);
			block++;
			if(block == USB_DATA1[num].partition)
			    num++;
		}
		*/
		if(num == 4)
		    break;
	}
	fclose(fp);
	///system("rm -f /tmp/usb.log");
    }
    //allen adds to print out debug info.
    #ifdef DEBUG
	    for(i=0;i<2;i++)
	    printf("usb[%d].hddevice=%s\n",i,usb[i].hddevice);
    #endif
    count = check_mount();
    fprintf(out,"<html><head><TITLE>Gigabit SOHO NAS</TITLE>\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n"
		"</head><BODY text=#000000 bgColor=#ffffff>\n"
		"<P><FONT class=style9>&nbsp;&nbsp;USB Setup</FONT></P>\n"
    //fprintf(out,"<form name=\"soho\" method=\"get\" action=\"Set_USB_Disk.cgi\" onsubmit=\"return validate()\">\n");
		"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n"
		"<tr>\n<td width=\"100%%\" bgcolor=\"#FFFFFF\" height=\"31\">\n"
		"<div align=\"left\">&nbsp;&nbsp;<img src=\"images/usb_disk.gif\" usemap=\"#Map\" border=\"0\"></div></td></tr>\n"
		"<tr height=\"5\"><td></td></tr>\n</table><br>\n"
		"<table width=\"550\" border=\"1\" bordercolor=\"#FFFFFF\" cellpadding=\"0\" cellspacing=\"0\">\n");
    if(num == 0){
#ifdef MICRONET
	fprintf(out,"<tr><td colspan=4 align=center class=style6><font color=\"#006666\">No USB Device Connected</font></td></tr>\n");
#else
	fprintf(out,"<tr><td colspan=4 align=center class=style6>No USB Device Connected</td></tr>\n");
#endif
    }else{
	    for(i=0;i<num;i++){
		fprintf(out,"<tr bgcolor=\"#ff0000\"><td colspan=5 class=style2>USB Device%d</td></tr>\n", i+1);
		fprintf(out,"<tr><td class=style4 align=center><b>Name</b></td><td class=style4><b>Description</b></td><td class=style4><b>File Type</b></td><td class=style4><b>Size</b></td><td></td></tr>\n");
		//fprintf(out,"<tr><td class=style4><b>Share</b></td><td class=style><b>Name</b></td><td class=style4><b>Description</b></td><td class=style4><b>File Type</b></td><td class=style4><b>Size</b></td>\n");

		for(j=0;j<USB_DATA1[i].partition;j++){
		//=====allen adds to filter swap file system type=====
		 if(strstr(USB_DATA1[i].type[j],"swap") == NULL){
			mounted = 0;
			for(k=0;k<count;k++){
			    if(!strcmp(MOUNT_set[k].device_name,USB_DATA1[i].device[j])){
				mounted = 1;
				strcpy(folder_name,MOUNT_set[k].folder_name);
				break;
			    }
			}
			  if(mounted == 1){
				fprintf(out,"<tr><form name=\"soho%d\" method=\"get\" action=\"Set_usbhdformat.cgi\">\n",i);
			fprintf(out,"<input type=\"hidden\" name=\"usb_share\" value=\"%s\">\n",USB_DATA1[i].device[j]);
			//fprintf(out,"<td><input type=\"text\" name=\"usb_folder\" value=\"%s\" size=\"20\" maxlength=\"16\"></td>\n",folder_name);
			fprintf(out,"<td align=center><input type=\"text\" name=\"usb_folder\" value=\"%s\" size=\"20\" maxlength=\"16\" readonly></td>\n",folder_name);
			index+=1;
		    }else{
			fprintf(out,"<tr><form name=\"soho%d\" method=\"get\" action=\"Set_usbhdformat.cgi\">\n",i);
			fprintf(out,"<input type=\"hidden\" name=\"usb_share\" value=\"%s\">\n",USB_DATA1[i].device[j]);
			//fprintf(out,"<td><input type=\"text\" name=\"usb_folder\" size=\"20\" maxlength=\"16\"></td>\n");
			fprintf(out,"<td align=center><input type=\"text\" name=\"usb_folder\" size=\"20\" maxlength=\"16\" readonly></td>\n");
			index+=1;
		    }
		    fprintf(out,"<td class=style4>Partition %d</td><td class=style4>%s</td><td class=style4>%s</td>\n",j+1,USB_DATA1[i].type[j],USB_DATA1[i].size[j]);

		    if(num == 0)
					fprintf(out,"<td><input type=\"submit\" name=\"advanced%d\" value=\"Format\" disabled onclick=\"return fconfirm()\"></td></tr>\n",i);
				else
					fprintf(out,"<td><input type=\"submit\" name=\"advanced%d\" value=\"Format\" onclick=\"return fconfirm()\"></td></tr>\n",i);

		    fprintf(out,"</form>\n");

		}
	     }//for(j=0;j<USB_DATA1[i].partition;j++) loop

	    }
	  }



fprintf(out,"</TABLE>\n");
    fprintf(out,"<map name=\"Map\">\n");
    fprintf(out,"<AREA shape=RECT coords=010,1,109,23 href=\"Show_USB_Detail.cgi\">\n");
    fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"Show_USB_Disk.cgi\">\n");
    fprintf(out,"<AREA shape=RECT coords=210,1,318,23 href=\"Show_USB_Printer.cgi\">\n");
    fprintf(out,"<AREA shape=RECT coords=321,1,512,23 href=\"usbque.htm\">\n");
    fprintf(out,"</map>\n");
    fprintf(out,"</body>\n</html>\n");
    //printf("num=%d\n",num);
    //printf("USB_DATA1[i].partition;=%d\n",USB_DATA1[i].partition;);
    //tmp=num*USB_DATA1[i].partition;
    #ifdef DEBUG1
	printf("index=%d\n",index);
    #endif
    fprintf(out,"<script language=\"JavaScript\">\n");

fprintf(out,"function fconfirm(){\
	flag=confirm('Confirm: All of the sotred data on the chosen USB disk will be erased, are you sure?');\
	if (flag){\
		if(confirm('Double confirm: All of the sotred data on the chosen USB disk will be erased, are you sure?')){\
			return true;\
		}else{	return false;	}\
	}else{	return false;}	}\n");

    fprintf(out,"function check_usb(){\n");
		if(index == 1){
			i=0;
	      fprintf(out,"un%d=document.soho.usb_folder.value;\n",i);
		fprintf(out,"if(un%d == null ||un%d == \"\")  document.soho.usb_share.disabled=true;\n",i,i);
		fprintf(out,"if(un%d == null ||un%d == \"\")  document.soho.usb_folder.disabled=true;\n",i,i);
		   fprintf(out,"for(var j=0;j<un%d.length;j++){\n",i);
		     fprintf(out,"temp=un%d.charCodeAt(j);\n",i);
			   //fprintf(out,"if( !( (temp==0x21) || (temp>=0x23 && temp<=0x29) || (temp>=0x2d && temp<=0x2e) || (temp>=0x30 && temp<=0x39) || (temp>=0x40 && temp<=0x5a)|| (temp==0x5e) || (temp>=0x61 && temp<=0x7b) || (temp>=0x7d && temp<=0x7e)) ){\n");
			   fprintf(out,"if( !( (temp==0x21) || (temp>=0x24 && temp<=0x24) || (temp>=0x28 && temp<=0x29) || (temp>=0x2d && temp<=0x2e) || (temp>=0x30 && temp<=0x39) || (temp>=0x40 && temp<=0x5a)|| (temp==0x5e) || (temp>=0x61 && temp<=0x7b) || (temp>=0x7d && temp<=0x7e)) ){\n");
				    fprintf(out,"alert(\"Mount name \" + un%d + \" contains at least one illegal character. Please retry!!\");\n",i);
				    fprintf(out,"document.soho.usb_folder.focus();\n");
				    fprintf(out,"return false;\n");
				 fprintf(out,"}\n");
			 fprintf(out,"}\n");
		}
		else{
		    for(i=0;i<index;i++){
			fprintf(out,"un%d=document.soho.usb_folder[%d].value;\n",i,i);
			fprintf(out,"if(un%d == null ||un%d == \"\")  document.soho.usb_share[%d].disabled=true;\n",i,i,i);
			fprintf(out,"if(un%d == null ||un%d == \"\")  document.soho.usb_folder[%d].disabled=true;\n",i,i,i);
			   fprintf(out,"for(var j=0;j<un%d.length;j++){\n",i);
			     fprintf(out,"temp=un%d.charCodeAt(j);\n",i);

				   fprintf(out,"if( !( (temp==0x21) || (temp>=0x24 && temp<=0x24) || (temp>=0x28 && temp<=0x29) || (temp>=0x2d && temp<=0x2e) || (temp>=0x30 && temp<=0x39) || (temp>=0x40 && temp<=0x5a)|| (temp==0x5e) || (temp>=0x61 && temp<=0x7b) || (temp>=0x7d && temp<=0x7e)) ){\n");
					    fprintf(out,"alert(\"Mount name \" + un%d + \" contains at least one illegal character. Please retry!!\");\n",i);
					    fprintf(out,"document.soho.usb_folder[%d].focus();\n",i);
					    fprintf(out,"return false;\n");
					 fprintf(out,"}\n");
				 fprintf(out,"}\n");
		    }
		}

      fprintf(out,"}\n");
     fprintf(out,"</script>\n");
}

int check_mount_failure_sp(void){

	FILE *fp;
	int num = 0, i, j;
	char *ptr, *end;
	char LineBuf[256];

	//memset(MOUNT_set,0,sizeof(MOUNT_VALUE)*36);
	//memset(FORMAT_set,0,sizeof(FORMAT_VALUE)*36);
  /*
	for(i=0;i<2;i++){
		strcpy(usb[i].umount,"0");
	}
	*/
	if((fp = fopen("/proc/mounts","r")) != NULL){
		while(fgets(LineBuf,255,fp)){
			if(strstr(LineBuf,"/dev/sd") && strstr(LineBuf,"/mnt")){
				ptr = LineBuf;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				//strcpy(MOUNT_set[num].device_name,ptr);
				#ifdef DEBUG1
					printf("%s: Line %d - MOUNT_set[%d].device_name = %s\n",__func__,__LINE__,num,MOUNT_set[num].device_name);
				#endif
				ptr = end + 1;
				ptr = strstr(ptr,"/mnt");/*write all after /mnt(/folder_of_mount) to folder_name_check. RAW*/
				if(ptr != NULL)
				    ptr+=5;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				strcpy(MOUNT_set[num].folder_name_check,ptr);
				#ifdef DEBUG1
					printf("%s: Line %d - MOUNT_set[%d].folder_name_check = %s\n",__func__,__LINE__,num,MOUNT_set[num].folder_name_check);
				#endif
				num++;
				continue;
			}
		}
		fclose(fp);
	}
	return num;
}

int check_mount_failure(void){

	FILE *fp;
	int num = 0, i, j;
	char *ptr, *end;
	char LineBuf[256];

	//memset(MOUNT_set,0,sizeof(MOUNT_VALUE)*36);

	//memset(FORMAT_set,0,sizeof(FORMAT_VALUE)*36);
  /*
	for(i=0;i<2;i++){
		strcpy(usb[i].umount,"0");
	}
	*/

		if((fp = fopen("/proc/mounts","r")) != NULL){

		while(fgets(LineBuf,255,fp)){
/*
			if((strstr(LineBuf,"/dev/sda1") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sda2") && strstr(LineBuf,"/mnt"))
			|| (strstr(LineBuf,"/dev/sda3") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sda4") && strstr(LineBuf,"/mnt"))){
				if(strstr(usb[0].hddevice,"sda"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sda"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdb1") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdb2") && strstr(LineBuf,"/mnt"))
			|| (strstr(LineBuf,"/dev/sdb3") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdb4") && strstr(LineBuf,"/mnt"))){
				if(strstr(usb[0].hddevice,"sdb"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdb"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdc1") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdc2") && strstr(LineBuf,"/mnt"))
			|| (strstr(LineBuf,"/dev/sdc3") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdc4") && strstr(LineBuf,"/mnt"))){
				if(strstr(usb[0].hddevice,"sdc"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdc"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdd1") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdd2") && strstr(LineBuf,"/mnt"))
			|| (strstr(LineBuf,"/dev/sdd3") && strstr(LineBuf,"/mnt")) || (strstr(LineBuf,"/dev/sdd4") && strstr(LineBuf,"/mnt"))){
				if(strstr(usb[0].hddevice,"sdd"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdd"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
*/
			if(strstr(LineBuf,"/dev/sd") && strstr(LineBuf,"/mnt")){
				ptr = LineBuf;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				//strcpy(MOUNT_set[num].device_name,ptr);
				#ifdef DEBUG1
					printf("%s: Line %d - MOUNT_set[%d].device_name = %s\n",__func__,__LINE__,num,MOUNT_set[num].device_name);
				#endif
				ptr = end + 1;
				ptr = strstr(ptr,"/mnt");/*write all after /mnt(/folder_of_mount) to folder_name_tmp. RAW*/
				if(ptr != NULL)
				    ptr+=5;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				strcpy(MOUNT_set[num].folder_name_tmp,ptr);
				#ifdef DEBUG1
					printf("%s: Line %d - MOUNT_set[%d].folder_name_tmp = %s\n",__func__,__LINE__,num,MOUNT_set[num].folder_name_tmp);
				#endif
				num++;
				continue;
			}
		}
		fclose(fp);
	}
	return num;
}

int check_mount(void){

	FILE *fp;
	int num = 0, i, j;
	char *ptr, *end;
	char LineBuf[256];

	memset(MOUNT_set,0,sizeof(MOUNT_VALUE)*36);

	//memset(FORMAT_set,0,sizeof(FORMAT_VALUE)*36);
	for(i=0;i<2;i++){
		strcpy(usb[i].umount,"0");
	}
	if((fp = fopen("/proc/mounts","r")) != NULL){
		while(fgets(LineBuf,255,fp)){

			if((strstr(LineBuf,"/dev/sda1") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sda2") && strstr(LineBuf,"/mnt/"))
			|| (strstr(LineBuf,"/dev/sda3") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sda4") && strstr(LineBuf,"/mnt/"))){
				if(strstr(usb[0].hddevice,"sda"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sda"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdb1") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdb2") && strstr(LineBuf,"/mnt/"))
			|| (strstr(LineBuf,"/dev/sdb3") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdb4") && strstr(LineBuf,"/mnt/"))){
				if(strstr(usb[0].hddevice,"sdb"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdb"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdc1") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdc2") && strstr(LineBuf,"/mnt/"))
			|| (strstr(LineBuf,"/dev/sdc3") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdc4") && strstr(LineBuf,"/mnt/"))){
				if(strstr(usb[0].hddevice,"sdc"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdc"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if((strstr(LineBuf,"/dev/sdd1") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdd2") && strstr(LineBuf,"/mnt/"))
			|| (strstr(LineBuf,"/dev/sdd3") && strstr(LineBuf,"/mnt/")) || (strstr(LineBuf,"/dev/sdd4") && strstr(LineBuf,"/mnt/"))){
				if(strstr(usb[0].hddevice,"sdd"))
				  strcpy(usb[0].umount,"1");
				else if(strstr(usb[1].hddevice,"sdd"))
				  strcpy(usb[1].umount,"1");
				//if(strlen(usb[0].umount) > 0)
				//strcpy(usb[0].umount,"1");
				//num++;
			}
			if(strstr(LineBuf,"/dev/sd") && strstr(LineBuf,"/mnt/")){
				ptr = LineBuf;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				strcpy(MOUNT_set[num].device_name,ptr);
				#ifdef DEBUG1
				printf("%s: Line %d - MOUNT_set[%d].device_name = %s\n",__func__,__LINE__,num,MOUNT_set[num].device_name);
				#endif
				ptr = end + 1;
				ptr = strstr(ptr,"/mnt");/*write all after /mnt(/folder_of_mount) to folder_name. RAW*/
				if(ptr != NULL)
				    ptr+=5;
				end = strchr(ptr,' ');
				if(end != NULL)
				    *end = '\0';
				strcpy(MOUNT_set[num].folder_name,ptr);
				#ifdef DEBUG1
				printf("%s: Line %d - MOUNT_set[%d].folder_name = %s\n",__func__,__LINE__,num,MOUNT_set[num].folder_name);
				#endif
				num++;
				continue;
			}
		}
		fclose(fp);
	}
	return num;
}

void Set_USB_Disk(FILE *out){

    FILE *fp, *fo;
    char *ptr, *end, *data1, *code_ptr, *code_end;
    USER_VALUE folder_user[MAX_USER];
    FOLDER_VALUE delfolder[MAX_FOLDER];
    char command[100], LineBuf[256], readlist[READ_LONG], temp[20], mount_tmp[20]="[", *feedback, temp_rmfolder[50], temp_mountfolder[20];
    char ptr_key_temp[1024], usb_temp[7], ptr_key_temp1[1024];
    int i, j, count = 0, find = 0, mark = 0, num = 0, result, stop_repeat[8], mount_exist = 0, check_failure = 1, count1 = 0, failure;
    int count2 = 0, count_checkmount = 0, index_checkmount = 0, double_check_mount = 0;
    char codepage_parse[50],iocharset_parse[50],iocharset_parse_iso[50];
    #ifdef DEBUG1
      printf("data=%s\n",data);
    #endif
    // andy 20050321
		DIR *dir = NULL ;
		struct dirent *dirptr;

    //0. get the information to mount
    count = 0;
    memset(MOUNT_set,0,sizeof(MOUNT_VALUE)*36);
    strcpy(ptr_key_temp,arg);
    strcpy(ptr_key_temp1,arg);
    ptr = ptr_key_temp;
     //allen adds this pointer for temperature the data information in order to //2. get the information to given mount usage
    #ifdef DEBUG1
      printf("%s : Line %d - ptr = %s\n",__func__,__LINE__,ptr);
      printf("%s : Line %d - ptr =%s; ptr_key_temp = %s\n",__func__,__LINE__,ptr,ptr_key_temp);
    #endif
    //while(strstr(ptr,"usb_folder=") && count < 36){
    count2 = check_mount_failure();  //allen adds to get mount name infomation to compare same as mount name form cgi sending-data
    while((strstr(ptr,"usb_share=") || strstr(ptr,"usb_folder=")) && count < 36){
	    if(strstr(ptr,"usb_share=")){    //string like dev/sda1
		ptr = strstr(ptr,"usb_share=");
		ptr+=10;
		end = strchr(ptr,'&');
		if(end != NULL)
		    *end = '\0';
		httpd_decode(ptr);
		strcpy(MOUNT_set[count].device_name,ptr);
		#ifdef DEBUG1
		  printf("%s : Line%d - MOUNT_set[%d].device_name = %s\n",__func__,__LINE__,count,MOUNT_set[count].device_name);
		#endif
		if(end != NULL)
		    ptr = end + 1;
		//else
		    //ptr +=24;
		  }
		  //printf("final ptr=%s\n",ptr);//This line is very important to check the reason for "ptr += 12"
		  if(strstr(ptr,"usb_folder=")){
			 ptr = strstr(ptr,"usb_folder=");
		   ptr+=11;
		    if(*ptr == '&')
			break;
	    end = strchr(ptr,'&');
	    if(end != NULL)
	       *end = '\0';
	    httpd_decode(ptr);
	    strcpy(MOUNT_set[count].folder_name,ptr);

//===== allen adds to adjugement repeat mount name to pass it!! =====
  for(i=0;i<count2;i++){
	  if(strstr(MOUNT_set[i].folder_name_tmp,MOUNT_set[count].folder_name)){
       NULL;
	  }
	  else{
	//  andy 20050321 check the mnt folder
				dir=opendir("/mnt");
				while( (dirptr=readdir(dir)) != NULL)
				{
					if(!strcasecmp(dirptr->d_name,MOUNT_set[count].folder_name))
					{
				      fprintf(out,"<html><head>\n");
			fprintf(out,"<title>Error</title>\n");
				  fprintf(out,"<tr><td bgcolor=\"#0050a0\"><b><font size=3 face=\"Tahoma,Arial,Helvetica\" color=\"#FFFFFF\">Error Warning</font>\n");
				  fprintf(out,"</b></td></tr></table>\n");
				  fprintf(out,"<br><table width=550 border=0 align=left><tr><td width=550 align=left><font size=\"2\" class=\"wordtype\">\n");
				  fprintf(out,"<p>Folder name <b>%s</b> exists!! Please change!!\n",dirptr->d_name);
				  fprintf(out,"</td></tr>\n");
				  fprintf(out,"<tr>\n");
				  fprintf(out,"<td width=550 align=left ><font size=\"2\" class=\"wordtype\">\n");
				  fprintf(out,"<form><br><input type=button value=\"Go Back\" onclick=window.history.back()>\n");
				  fprintf(out,"</form></td></tr></table>\n");
				  fprintf(out,"</div></td></tr>\n");
				  fprintf(out,"</td></tr>\n");
				  fprintf(out,"</body></html>\n");
				  return;
					}
			    }
	//======================================
		}
	}
		      #ifdef DEBUG1
		printf("%s : Line%d - MOUNT_set[%d].folder_name = %s\n",__func__,__LINE__,count,MOUNT_set[count].folder_name);
	    #endif
	    if(end != NULL)
	       ptr = end + 1;
	    count++;
	    }
    }
// end
    //1(a). umount all folder
    count = check_mount();
    #ifdef DEBUG1
      printf("%s : Line %d - count = %d\n",__func__,__LINE__,count);
    #endif

      for(i=0;i<count;i++){
		sprintf(command,"sync;sync;sync;sync;sync;killall smbd;umount /mnt/%s",MOUNT_set[i].folder_name);
		#ifdef DEBUG1
		  printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
		#endif
		system(command);
	}
//==================adjust the umount failure situation==================

     for(i=0;i<count;i++){
	//if(strlen(MOUNT_set[i].folder_name_adjust) == 0)
	  mount_exist = 0;  //allen adds to check umount success or failure
	//check_mount();
	if(strlen(MOUNT_set[i].folder_name) > 0)  //allen adds to check umount success or failure
	  mount_exist = 1;
	strcpy(temp_mountfolder,MOUNT_set[i].folder_name);  //allen adds to check umount success or failure

//=======================================================================
//==================adjust the umount failure situation==================

	count1 = check_mount_failure();	 //allen adds to check umount success or failure]
	#ifdef DEBUG
		printf("count1 = %d   mount_exist=%d   check_failure=%d\n", count1,mount_exist,check_failure);
	#endif
	if(mount_exist == 1){  //allen adds to check umount success or failure
	  if(count1 > 0){
		  for(j=0;j<count1;j++){
			  check_failure = 0;  //umount successful
			  #ifdef DEBUG
				  printf("failure is set to 0!!!\n");
				  printf("check_failure=%d\n",check_failure);
			  #endif
		    if(strstr(MOUNT_set[j].folder_name_tmp,temp_mountfolder) != NULL){	//we assume failure when mount folder name exist
			#ifdef DEBUG
			 printf("mount_set[%d]:%s\n",j,MOUNT_set[j].folder_name_tmp);
			 printf("failure is set to 1!!!\n");
			#endif
		       check_failure = 1;
		      break;
		    }
		  }
		  #ifdef DEBUG
			printf("after compare: check_failure = %d\n", check_failure);
		  #endif
/*
//original code (possible err due to the use of i))
		  for(j=0;j<count1;j++){
			  check_failure == 0;  //umount successful
		    if(strstr(MOUNT_set[i].folder_name_tmp,temp_mountfolder) != NULL){	//we assume failure when mount folder name exist
			 printf("mount_set[%d]:%s\n",i,MOUNT_set[i].folder_name_tmp);
			 printf("failure is set to 1!!!\n");
		       check_failure = 1;
		       break;
		    }
		    #ifdef DEBUG
		      printf("check_failure = %d\n", check_failure);
		    #endif
		  }
*/
	  }
	  else if(count1 == 0){
		      check_failure = 0;  //umount successful
	  }
	       #ifdef DEBUG
		 printf("check_failure = %d\n", check_failure);
		 printf("popwin check_failure = %d\n", check_failure);
	       #endif
	    if(check_failure == 1){
	       fprintf(out,"<script language=\"JavaScript\">\n");
	     fprintf(out,"alert(\"Unmount failed: device \"%s\" is currently occupied. Please make sure no one(or download process) is accessing the device.\");\n",temp_mountfolder);
	     fprintf(out,"</script>\n");
	    boot_page(out, "Show_USB_Disk.cgi");
	  }
	  }
//=======================================================================
	sprintf(command,"rmdir /mnt/%s",MOUNT_set[i].folder_name);
	#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
	#endif
	system(command);
     }
    //1(b). rewrite /etc/smb.conf. test is not finished, please check this part.
    memset(delfolder,0,sizeof(delfolder));
    for(i=0;i<count;i++){
	strcpy(delfolder[i].foldername,MOUNT_set[i].folder_name);
	#ifdef DEBUG
	printf("!!!!!umount!!delfolder[%d].foldername = %s!!!!!\n", i, delfolder[i].foldername);
      #endif
    }
    result = Set_delfolder_m_usbmount((char)count,delfolder);
    #ifdef DEBUG
	printf("result = %d\n", result);
    #endif
    //2. get the information to mount
    count = 0;
    memset(MOUNT_set,0,sizeof(MOUNT_VALUE)*36);
    #ifdef DEBUG
	printf("###### ptr_key_temp1 = %s ######\n", ptr_key_temp1);
    #endif
    ptr = ptr_key_temp1;
    // strcpy(ptr,ptr_key_temp);  //CGI data
    #ifdef DEBUG1
    //printf("%s : Line %d - ptr_key_temp = %s\n",__func__,__LINE__,ptr_key_temp);
	printf("%s : Line %d - ptr = %s\n",__func__,__LINE__,ptr);
    #endif
    //while(strstr(ptr,"usb_folder=") && count < 36){
    while((strstr(ptr,"usb_share=") || strstr(ptr,"usb_folder=")) && count < 36){
	    if(strstr(ptr,"usb_share=")){    //string like dev/sda1
		ptr = strstr(ptr,"usb_share=");
		ptr+=10;
		end = strchr(ptr,'&');
		if(end != NULL)
		    *end = '\0';
		httpd_decode(ptr);
		strcpy(MOUNT_set[count].device_name,ptr);
		#ifdef DEBUG1
			printf("%s : Line%d - MOUNT_set[%d].device_name = %s\n",__func__,__LINE__,count,MOUNT_set[count].device_name);
		#endif
		if(end != NULL)
		    ptr = end + 1;
		  }
		  if(strstr(ptr,"usb_folder=")){
			 ptr = strstr(ptr,"usb_folder=");
		   ptr+=11;
		    if(*ptr == '&')
			break;
	    end = strchr(ptr,'&');
	    if(end != NULL)
	       *end = '\0';
	    httpd_decode(ptr);
	    strcpy(MOUNT_set[count].folder_name,ptr);
/*
//  andy 20050321 check the mnt folder
			dir=opendir("/mnt");
			while( (dirptr=readdir(dir)) != NULL)
			{
				if(!strcmp(dirptr->d_name,MOUNT_set[count].folder_name))
				{
				fprintf(out,"<html><head>\n");
		fprintf(out,"<title>Error</title>\n");
			  fprintf(out,"<tr><td bgcolor=\"#0050a0\"><b><font size=3 face=\"Tahoma,Arial,Helvetica\" color=\"#FFFFFF\">Error Warning</font>\n");
			  fprintf(out,"</b></td></tr></table>\n");
			  fprintf(out,"<br><table width=550 border=0 align=center><tr><td width=550 valign=center><font size=\"2\" class=\"wordtype\">\n");
			  fprintf(out,"<p>Folder name <b>%s</b> exists!! Please change!!\n",dirptr->d_name);
			  fprintf(out,"</td></tr></table>\n");
			  fprintf(out,"<table width=550 border=0 align=center><tr>\n");
			  fprintf(out,"<td width=550 valign=center ><font size=\"2\" class=\"wordtype\">\n");
			  fprintf(out,"<form><br><input type=button value=\"Go Back\" onclick=window.history.back()>\n");
			  fprintf(out,"</form></td></tr></table>\n");
			  fprintf(out,"</div></td></tr></table>\n");
			  fprintf(out,"</td></tr></table>\n");
			  fprintf(out,"</body></html>\n");
			  return;
				}
		 }
// end
*/
	  #ifdef DEBUG1
		printf("%s : Line%d - MOUNT_set[%d].folder_name = %s\n",__func__,__LINE__,count,MOUNT_set[count].folder_name);
	    #endif
	    if(end != NULL)
	       ptr = end + 1;
	    count++;
	    }
    }
    //======================================================================================================================
    #ifdef DEBUG1
	printf("%s : Line %d - count = %d\n",__func__,__LINE__,count);
    #endif
    //3. get file type
    memset(usb_port,0,sizeof(USB_PORT)*4);
    system("/etc/rc.d/usb_info.sh 1 > /tmp/usb.log");
    if((fp = fopen("/tmp/usb.log","r")) != NULL){
	while(fgets(LineBuf,255,fp)){
		if(strstr(LineBuf,"/dev/sd")){
			find = 0;
			mark = 0;
			for(i=0;i<count;i++){
				if(strstr(LineBuf,MOUNT_set[i].device_name)){
					//========== save the USB port series for device name ==========
					if(strstr(LineBuf,"PORT01")){
					 strcpy(usb_port[0].folder_name[1],MOUNT_set[i].folder_name);
					 strcpy(usb_port[0].device_name[1],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT02")){
					 strcpy(usb_port[0].folder_name[2],MOUNT_set[i].folder_name);
					 strcpy(usb_port[0].device_name[2],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT03")){
					 strcpy(usb_port[0].folder_name[3],MOUNT_set[i].folder_name);
					 strcpy(usb_port[0].device_name[3],MOUNT_set[i].device_name);
					}
	      if(strstr(LineBuf,"PORT04")){
	       strcpy(usb_port[0].folder_name[4],MOUNT_set[i].folder_name);
					 strcpy(usb_port[0].device_name[4],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT11")){
					 strcpy(usb_port[1].folder_name[1],MOUNT_set[i].folder_name);
					 strcpy(usb_port[1].device_name[1],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT12")){
					 strcpy(usb_port[1].folder_name[2],MOUNT_set[i].folder_name);
					 strcpy(usb_port[1].device_name[2],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT13")){
					 strcpy(usb_port[1].folder_name[3],MOUNT_set[i].folder_name);
					 strcpy(usb_port[1].device_name[3],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT14")){
					 strcpy(usb_port[1].folder_name[4],MOUNT_set[i].folder_name);
					 strcpy(usb_port[1].device_name[4],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT21")){
					 strcpy(usb_port[2].folder_name[1],MOUNT_set[i].folder_name);
					 strcpy(usb_port[2].device_name[1],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT22")){
					 strcpy(usb_port[2].folder_name[2],MOUNT_set[i].folder_name);
					 strcpy(usb_port[2].device_name[2],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT23")){
					 strcpy(usb_port[2].folder_name[3],MOUNT_set[i].folder_name);
					 strcpy(usb_port[2].device_name[3],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT24")){
					 strcpy(usb_port[2].folder_name[4],MOUNT_set[i].folder_name);
					 strcpy(usb_port[2].device_name[4],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT31")){
					 strcpy(usb_port[3].folder_name[1],MOUNT_set[i].folder_name);
					 strcpy(usb_port[3].device_name[1],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT32")){
					 strcpy(usb_port[3].folder_name[2],MOUNT_set[i].folder_name);
					 strcpy(usb_port[3].device_name[2],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT33")){
					 strcpy(usb_port[3].folder_name[3],MOUNT_set[i].folder_name);
					 strcpy(usb_port[3].device_name[3],MOUNT_set[i].device_name);
					}
					if(strstr(LineBuf,"PORT34")){
					 strcpy(usb_port[3].folder_name[4],MOUNT_set[i].folder_name);
					 strcpy(usb_port[3].device_name[4],MOUNT_set[i].device_name);
					}
					find = 1;
					mark = i;
					break;
				}
			}
			if(find == 1){
			  if(strstr(LineBuf,"partitions") == NULL){
					//printf("%s : Line %d - mark = %d\n",__func__,__LINE__,mark);
					//printf("LineBuf=%s\n",LineBuf);
					ptr = strstr(LineBuf,"filetype=");
					//printf("pass1\n");
					//printf("ptr=%s\n",ptr);
					ptr+=9;
					//printf("pass2\n");
					//printf("ptr=%s\n",ptr);
					end = strchr(ptr,':');
					//printf("pass3\n");
					if(end != NULL)
					    *end = '\0';
					//printf("pass4\n");
					#ifdef DEBUG1
					  printf("%s : Line %d - ptr = %s\n",__func__,__LINE__,ptr);
					#endif
					//printf("pass5\n");
					if(strstr(ptr,"FAT"))
					    strcpy(MOUNT_set[mark].system_type,"vfat");
					else if(strstr(ptr,"Linux"))
					    strcpy(MOUNT_set[mark].system_type,"ext3");
					else if(strstr(ptr,"NTFS"))
					    strcpy(MOUNT_set[mark].system_type,"ntfs");
					//printf("pass6\n");
			 //========== save the USB port series for system_type ==========
			   end = end +1;
			   ptr = strchr(end,':');
			   ptr+=1;
			   end = ptr + 6;
			   *end = '\0';

				 if(strstr(ptr,"PORT01"))
					 strcpy(usb_port[0].system_type[1],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT02"))
					 strcpy(usb_port[0].system_type[2],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT03"))
					 strcpy(usb_port[0].system_type[3],MOUNT_set[mark].system_type);
	      if(strstr(ptr,"PORT04"))
					 strcpy(usb_port[0].system_type[4],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT11"))
					 strcpy(usb_port[1].system_type[1],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT12"))
					 strcpy(usb_port[1].system_type[2],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT13"))
					 strcpy(usb_port[1].system_type[3],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT14"))
					 strcpy(usb_port[1].system_type[4],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT21"))
					 strcpy(usb_port[2].system_type[1],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT22"))
					 strcpy(usb_port[2].system_type[2],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT23"))
					 strcpy(usb_port[2].system_type[3],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT24"))
					 strcpy(usb_port[2].system_type[4],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT31"))
					 strcpy(usb_port[3].system_type[1],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT32"))
					 strcpy(usb_port[3].system_type[2],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT33"))
					 strcpy(usb_port[3].system_type[3],MOUNT_set[mark].system_type);
					if(strstr(ptr,"PORT34"))
					 strcpy(usb_port[3].system_type[4],MOUNT_set[mark].system_type);

			   if(!strcmp(ptr,"PORT01"))
				strcpy(usb_port[0].series[1],ptr);
			   if(!strcmp(ptr,"PORT02"))
				strcpy(usb_port[0].series[2],ptr);
			   if(!strcmp(ptr,"PORT03"))
				strcpy(usb_port[0].series[3],ptr);
			   if(!strcmp(ptr,"PORT04"))
				strcpy(usb_port[0].series[4],ptr);
			   if(!strcmp(ptr,"PORT11"))
				strcpy(usb_port[1].series[1],ptr);
			   if(!strcmp(ptr,"PORT12"))
				strcpy(usb_port[1].series[2],ptr);
			   if(!strcmp(ptr,"PORT13"))
				strcpy(usb_port[1].series[3],ptr);
			   if(!strcmp(ptr,"PORT14"))
				strcpy(usb_port[1].series[4],ptr);
			   if(!strcmp(ptr,"PORT21"))
				strcpy(usb_port[2].series[1],ptr);
			   if(!strcmp(ptr,"PORT22"))
				strcpy(usb_port[2].series[2],ptr);
			   if(!strcmp(ptr,"PORT23"))
				strcpy(usb_port[2].series[3],ptr);
			   if(!strcmp(ptr,"PORT24"))
				strcpy(usb_port[2].series[4],ptr);
			   if(!strcmp(ptr,"PORT31"))
				strcpy(usb_port[3].series[1],ptr);
			   if(!strcmp(ptr,"PORT32"))
				strcpy(usb_port[3].series[2],ptr);
			   if(!strcmp(ptr,"PORT33"))
				strcpy(usb_port[3].series[3],ptr);
			   if(!strcmp(ptr,"PORT34"))
				strcpy(usb_port[3].series[4],ptr);
		  //===========================================================
			  }
			}
		}
	}  //while loop
	fclose(fp);
    }
    #ifdef DEBUG1
	printf("%s : Line %d - count = %d\n",__func__,__LINE__,count);
    #endif
    //4(a). mount USB disk
    //allen adds to parse codepage two parameters for adjuge
#ifdef NH221
	if((fp = fopen("/etc/smb.conf","r")) == NULL){
	printf("!!!!!!!Open smb.conf file failure!!!!!!\n");
	return;
      }
      memset( iocharset_parse, 0, 50 );
      memset( codepage_parse, 0, 50 );
	    while(fgets(LineBuf,255,fp) != NULL){
				if(strstr(LineBuf, "character set =") != NULL){
				   code_ptr = strstr(LineBuf,"character set = ");
				   code_ptr+=16;
				    if(strchr(code_ptr,0x0A)){
			   code_end = strchr(code_ptr,0x0A);
			   *code_end = '\0';
			}
				    strcpy(iocharset_parse, code_ptr);
				    #ifdef DEBUG1
				    printf("code_ptr=%s\n",code_ptr);
				    printf("!!!!!!smb.conf; character set = %s\n",iocharset_parse);
				    #endif
				}else if(strstr(LineBuf, "client code page =") != NULL){
			     code_ptr = strstr(LineBuf, "client code page = ");
			     code_ptr+=19;
			if(strchr(code_ptr,0x0A)){
			   code_end = strchr(code_ptr,0x0A);
			   *code_end = '\0';
			}
			strcpy(codepage_parse, code_ptr);
			#ifdef DEBUG1
			printf("code_ptr=%s\n",code_ptr);
			printf("!!!!!!smb.conf; client code page = %s\n",codepage_parse);
			#endif
				}
			 }
	    fclose(fp);
	   #ifdef DEBUG1
     printf("!!!!!!smb.conf; character set =%s\n",iocharset_parse);
     printf("!!!!!!smb.conf; client code page =%s\n",codepage_parse);
     #endif
     // allen adds for translate uppercase 'ISO' to lowercase 'iso'
       strcpy(iocharset_parse_iso,"");
       strcpy(iocharset_parse_iso,"iso");
     if(strstr(iocharset_parse,"ISO")){
	code_ptr = strstr(iocharset_parse,"ISO");
	code_ptr+=3;
	strcat(iocharset_parse_iso,code_ptr);
	strcpy(iocharset_parse,iocharset_parse_iso);
     }
#endif
    for(i=0;i<count;i++){
	sprintf(command,"mkdir -p /mnt/%s",MOUNT_set[i].folder_name);
	#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
	#endif
	system(command);
	if(!strcmp(MOUNT_set[i].system_type,"ext2") || !strcmp(MOUNT_set[i].system_type,"ext3")){	//both read & write
		sprintf(command,"mount -t %s %s /mnt/%s",MOUNT_set[i].system_type,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
		#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
		#endif
	}
	else if (!strcmp(MOUNT_set[i].system_type,"ntfs")){
		sprintf(command,"mount -t %s -o iocharset=utf8 %s /mnt/%s",MOUNT_set[i].system_type,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
		#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
		#endif
	}
#ifdef NH230
	else if (!strcmp(MOUNT_set[i].system_type,"vfat")){
		sprintf(command,"mount -t %s -o iocharset=utf8 %s /mnt/%s",MOUNT_set[i].system_type,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
		#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
		#endif
	}
#endif
#ifdef NH221
	else if (!strcmp(MOUNT_set[i].system_type,"vfat")){
		if(strstr(codepage_parse,"874")){
			//printf("codepage_parse=874!!\n");
			sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=tis-620 %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
			//printf("command=%s\n",command);
		}
		else if(strstr(codepage_parse,"932")){
			//printf("codepage_parse=932!!\n");
			sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=sjis %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
			//printf("command=%s\n",command);
		}
		else if(strstr(codepage_parse,"936")){
			//printf("codepage_parse=936!!\n");
			sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=gb2312 %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
			//printf("command=%s\n",command);
		}
		else if(strstr(codepage_parse,"949")){
			//printf("codepage_parse=949!!\n");
			sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=euc-kr %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
			//printf("command=%s\n",command);
		}
		else if(strstr(codepage_parse,"950")){
			//printf("codepage_parse=950!!\n");
			sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=big5 %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
			//printf("command=%s\n",command);
		}
		else{
		//printf("FAT32;codepage_parse=%s;iocharset_parse=%s\n", codepage_parse, iocharset_parse);
		sprintf(command,"mount -t %s -o shortname=mixed,codepage=%s,iocharset=%s %s /mnt/%s",MOUNT_set[i].system_type,codepage_parse,iocharset_parse,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
		//printf("command=%s\n",command);
		}
	}
#endif
	else{	//read only
		sprintf(command,"mount -t %s %s /mnt/%s -r",MOUNT_set[i].system_type,MOUNT_set[i].device_name,MOUNT_set[i].folder_name);
		#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
		#endif
	}
	#ifdef DEBUG1
		printf("%s : Line %d - command = %s\n",__func__,__LINE__,command);
	#endif
	system(command);
// ============ allen adds to double check mount failure situation =============
    /*
	count_checkmount = check_mount_failure_sp();
	double_check_mount = 1;

	for(index_checkmount=0;index_checkmount<count_checkmount;index_checkmount++){
		if(strstr(MOUNT_set[index_checkmount].folder_name_check,MOUNT_set[i].folder_name)){
		   double_check_mount = 0;
		}
	}
       if(double_check_mount == 1){
	system(command);
	printf(out,"Starting double check mount for %s!!\n", MOUNT_set[i].folder_name);
       }
    */
// =============================================================================
    }

    //mount -t vfat -o codepage=874,iocharset=tis-620 /dev/sda1 /mnt/usb20
    //===== allen adds to kill the USB folder when mount failure =====
    count1 = check_mount_failure();
    #ifdef DEBUG
      printf("!!!!!count = %d, count1 = %d!!!!!\n", count, count1);
    #endif
    for(i=0;i<count;i++){
       failure = 1;  //default to set the mount failure
     for(j=0;j<count1;j++){
      if(strstr(MOUNT_set[i].folder_name,MOUNT_set[j].folder_name_tmp)){
	 failure = 0;  //mount successful
	 #ifdef DEBUG
	   printf("!!!!!mount successful!!MOUNT_set[%d].folder_name =%s!!!!!\n", i, MOUNT_set[i].folder_name);
	 #endif
      }
     }
       if(failure == 1){
	 sprintf(command,"rm -fr /mnt/%s",MOUNT_set[i].folder_name);
	 #ifdef DEBUG
		 printf("!!!!!mount failure!!command = %s!!!!!\n", command);
	 #endif
	 system(command);
       }
    }
    //===============================================================
    memset(stop_repeat, 0, 8);	//allen adds to clear mount folder name for write smb.conf

    //4(b). rewrite /etc/smb.conf. test is not finished, please check this part.
    if((fp = fopen("/etc/smb.conf","r")) == NULL)
	return ;
	if((fo = fopen("/etc/smb1.conf","w+")) == NULL)
	    return ;
	while(fgets(LineBuf,255,fp) != NULL){
		for(i=0;i<count;i++){
			strcpy(mount_tmp,"");
			strcat(mount_tmp,MOUNT_set[i].folder_name);
			strcat(mount_tmp,"");
			//printf("mount_tmp = %s\n", mount_tmp);

			if(strstr(LineBuf,mount_tmp) && strlen(mount_tmp) > 0){
				stop_repeat[i] = 1;
			}
		}
		fputs(LineBuf,fo);
    }
    fclose(fp);
	Get_alluserlist(folder_user);
	 memset( readlist, 0, READ_LONG );
	 #ifdef DEBUG
	   printf("!!!!!readlist = %s; strlen(readlist) = %d!!!!!\n", readlist, strlen(readlist));
	 #endif
    for(i=0;i<MAX_USER;i++){
		if(!strlen(&(folder_user[i].username[0])))
		    break;
		sprintf(&(readlist[strlen(readlist)]),"%s,",&(folder_user[i].username[0]));
	}
	num = Read_groupconf();
    for(i=0;i<num;i++)
	sprintf(&(readlist[strlen(readlist)]),"@%s,",&(GROUP_set[i].name[0]));
    //===== allen add to avoid the abnormal chinese word writen to smb.conf =====
     ptr = strstr(readlist,"all,");
     #ifdef DEBUG
       printf("readlist = %s\n", ptr);
     #endif
	 strcpy(readlist,ptr);
     #ifdef DEBUG
       printf("new readlist = %s\n", readlist);
     #endif
    //===========================================================================
    //========= allen adds to filter replicate user authority situation =========
     ptr+=2; //
     #ifdef DEBUG
       printf("shift ptr = %s\n", ptr);
     #endif
     if(strstr(ptr,"all,") != NULL){
       ptr=strstr(ptr,"all,");
       #ifdef DEBUG
	 printf("processed ptr = %s\n", ptr);
       #endif
       strcpy(readlist,ptr);
     #ifdef DEBUG
       printf("last readlist = %s\n", readlist);
     #endif
     }
    //===========================================================================
    count1 = check_mount_failure();
    for(i=0;i<count;i++){
	failure = 1;
	     if(stop_repeat[i] != 1){
	     //===== allen adds to kill the USB folder when mount failure =====
		 for(j=0;j<count1;j++){
			 if(strstr(MOUNT_set[i].folder_name,MOUNT_set[i].folder_name_tmp) && strlen(MOUNT_set[i].folder_name) >0 &&
			 strlen(MOUNT_set[i].folder_name_tmp) > 0)
			     failure = 0;
			     printf("!!!!!mount sucessful!!it writes to smb.conf!!!!!\n");
		 }
	     //==============================================================
		     if(failure == 0){	// allen adds to kill the USB folder when mount failure
				    fprintf(fo,"\n");
				    fprintf(fo,"[%s]\n",MOUNT_set[i].folder_name);
				    //fprintf(fo,"   comment = %s\n",MOUNT_set[i].folder_name);
				    fprintf(fo,"   comment = USB driver folder\n");
				    fprintf(fo,"   path = /mnt/%s\n",MOUNT_set[i].folder_name);
				    fprintf(fo,"   browseable = yes\n");
				    fprintf(fo,"   public = no\n");
				    fprintf(fo,"   writable = no\n");
				    fprintf(fo,"   read only = yes\n");
				    fprintf(fo,"   Invalid users = \n");
				    fprintf(fo,"   readlist = %s\n", readlist);
				    fprintf(fo,"   writelist = \n");
				    fprintf(fo,"   directory mode = 0777\n");
				    fprintf(fo,"   create mask = 0777\n");
				    fprintf(fo,"\n");
			   }
		   }
    }
	fclose(fo);
		system("/bin/mv /etc/smb1.conf /etc/smb.conf");
		for(i=0;i<count;i++){
		  sprintf(temp_rmfolder,"rm -fr /mnt/%s/lost+found",MOUNT_set[i].folder_name);
		  system(temp_rmfolder);		//allen adds to kill lost+found folder
		}
		my_system(out,"killall ftpd");
		my_system(out,"echo 0 > /var/run/ftpd_num");
		my_system(out,"checksmb");
		my_system(out,"/etc/smb restart &");
    //5. return to Show_USB_Disk.cgi
    fprintf(out,"<script language=\"JavaScript\">\n");
    fprintf(out,"location.replace('http://%s:%s/Show_USB_Disk.cgi');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
    fprintf(out,"</script>\n");
       #ifdef DEBUG1
	 printf("count=%d\n",count);
       for(i=0;i<count;i++)
	 printf(",MOUNT_set[%d].folder_name=%s\n",i,MOUNT_set[i].folder_name);
       #endif

       #ifdef DEBUG
       if(count==0){
		    strcpy(CONFIG.USB_MOUNT1,"");
		    strcpy(CONFIG.USB_MOUNT2,"");
		    strcpy(CONFIG.USB_MOUNT3,"");
		    strcpy(CONFIG.USB_MOUNT4,"");
		    strcpy(CONFIG.USB_MOUNT5,"");
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
       else if(count==1){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    strcpy(CONFIG.USB_MOUNT2,"");
		    strcpy(CONFIG.USB_MOUNT3,"");
		    strcpy(CONFIG.USB_MOUNT4,"");
		    strcpy(CONFIG.USB_MOUNT5,"");
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
       else if(count==2){
	sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    strcpy(CONFIG.USB_MOUNT3,"");
		    strcpy(CONFIG.USB_MOUNT4,"");
		    strcpy(CONFIG.USB_MOUNT5,"");
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
       else if(count==3){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    strcpy(CONFIG.USB_MOUNT4,"");
		    strcpy(CONFIG.USB_MOUNT5,"");
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
		   else if(count==4){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    sprintf(CONFIG.USB_MOUNT4,"%s,%s,%s",MOUNT_set[3].device_name,MOUNT_set[3].folder_name,MOUNT_set[3].system_type);
		    strcpy(CONFIG.USB_MOUNT5,"");
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
       else if(count==5){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    sprintf(CONFIG.USB_MOUNT4,"%s,%s,%s",MOUNT_set[3].device_name,MOUNT_set[3].folder_name,MOUNT_set[3].system_type);
		    sprintf(CONFIG.USB_MOUNT5,"%s,%s,%s",MOUNT_set[4].device_name,MOUNT_set[4].folder_name,MOUNT_set[4].system_type);
		    strcpy(CONFIG.USB_MOUNT6,"");
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
		   else if(count==6){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    sprintf(CONFIG.USB_MOUNT4,"%s,%s,%s",MOUNT_set[3].device_name,MOUNT_set[3].folder_name,MOUNT_set[3].system_type);
		    sprintf(CONFIG.USB_MOUNT5,"%s,%s,%s",MOUNT_set[4].device_name,MOUNT_set[4].folder_name,MOUNT_set[4].system_type);
		    sprintf(CONFIG.USB_MOUNT6,"%s,%s,%s",MOUNT_set[5].device_name,MOUNT_set[5].folder_name,MOUNT_set[5].system_type);
		    strcpy(CONFIG.USB_MOUNT7,"");
		    strcpy(CONFIG.USB_MOUNT8,"");
		   }
       else if(count==7){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    sprintf(CONFIG.USB_MOUNT4,"%s,%s,%s",MOUNT_set[3].device_name,MOUNT_set[3].folder_name,MOUNT_set[3].system_type);
		    sprintf(CONFIG.USB_MOUNT5,"%s,%s,%s",MOUNT_set[4].device_name,MOUNT_set[4].folder_name,MOUNT_set[4].system_type);
		    sprintf(CONFIG.USB_MOUNT6,"%s,%s,%s",MOUNT_set[5].device_name,MOUNT_set[5].folder_name,MOUNT_set[5].system_type);
	sprintf(CONFIG.USB_MOUNT7,"%s,%s,%s",MOUNT_set[6].device_name,MOUNT_set[6].folder_name,MOUNT_set[6].system_type);
	strcpy(CONFIG.USB_MOUNT8,"");
       }
       else if(count==8){
		    sprintf(CONFIG.USB_MOUNT1,"%s,%s,%s",MOUNT_set[0].device_name,MOUNT_set[0].folder_name,MOUNT_set[0].system_type);
		    sprintf(CONFIG.USB_MOUNT2,"%s,%s,%s",MOUNT_set[1].device_name,MOUNT_set[1].folder_name,MOUNT_set[1].system_type);
		    sprintf(CONFIG.USB_MOUNT3,"%s,%s,%s",MOUNT_set[2].device_name,MOUNT_set[2].folder_name,MOUNT_set[2].system_type);
		    sprintf(CONFIG.USB_MOUNT4,"%s,%s,%s",MOUNT_set[3].device_name,MOUNT_set[3].folder_name,MOUNT_set[3].system_type);
		    sprintf(CONFIG.USB_MOUNT5,"%s,%s,%s",MOUNT_set[4].device_name,MOUNT_set[4].folder_name,MOUNT_set[4].system_type);
		    sprintf(CONFIG.USB_MOUNT6,"%s,%s,%s",MOUNT_set[5].device_name,MOUNT_set[5].folder_name,MOUNT_set[5].system_type);
	sprintf(CONFIG.USB_MOUNT7,"%s,%s,%s",MOUNT_set[6].device_name,MOUNT_set[6].folder_name,MOUNT_set[6].system_type);
	sprintf(CONFIG.USB_MOUNT8,"%s,%s,%s",MOUNT_set[7].device_name,MOUNT_set[7].folder_name,MOUNT_set[7].system_type);
       }
      #endif
   #ifdef DEBUG
     printf("PORT01=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[0].series[1], usb_port[0].device_name[1], usb_port[0].folder_name[1], usb_port[0].system_type[1]);
     printf("PORT02=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[0].series[2], usb_port[0].device_name[2], usb_port[0].folder_name[2], usb_port[0].system_type[2]);
     printf("PORT03=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[0].series[3], usb_port[0].device_name[3], usb_port[0].folder_name[3], usb_port[0].system_type[3]);
     printf("PORT04=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[0].series[4], usb_port[0].device_name[4], usb_port[0].folder_name[4], usb_port[0].system_type[4]);
     printf("PORT11=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[1].series[1], usb_port[1].device_name[1], usb_port[1].folder_name[1], usb_port[1].system_type[1]);
     printf("PORT12=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[1].series[2], usb_port[1].device_name[2], usb_port[1].folder_name[2], usb_port[1].system_type[2]);
     printf("PORT13=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[1].series[3], usb_port[1].device_name[3], usb_port[1].folder_name[3], usb_port[1].system_type[3]);
     printf("PORT14=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[1].series[4], usb_port[1].device_name[4], usb_port[1].folder_name[4], usb_port[1].system_type[4]);
     printf("PORT21=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[2].series[1], usb_port[2].device_name[1], usb_port[2].folder_name[1], usb_port[2].system_type[1]);
     printf("PORT22=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[2].series[2], usb_port[2].device_name[2], usb_port[2].folder_name[2], usb_port[2].system_type[2]);
     printf("PORT23=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[2].series[3], usb_port[2].device_name[3], usb_port[2].folder_name[3], usb_port[2].system_type[3]);
     printf("PORT24=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[2].series[4], usb_port[2].device_name[4], usb_port[2].folder_name[4], usb_port[2].system_type[4]);
     printf("PORT31=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[3].series[1], usb_port[3].device_name[1], usb_port[3].folder_name[1], usb_port[3].system_type[1]);
     printf("PORT32=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[3].series[2], usb_port[3].device_name[2], usb_port[3].folder_name[2], usb_port[3].system_type[2]);
     printf("PORT33=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[3].series[3], usb_port[3].device_name[3], usb_port[3].folder_name[3], usb_port[3].system_type[3]);
     printf("PORT34=%s;devicename=%s;foldername=%s;systemtype=%s\n", usb_port[3].series[4], usb_port[3].device_name[4], usb_port[3].folder_name[4], usb_port[3].system_type[4]);
   #endif
    if(strlen(usb_port[0].device_name[1]) > 0 && strlen(usb_port[0].folder_name[1]) > 0 && strlen(usb_port[0].system_type[1]) > 0)
	sprintf(CONFIG.USB_PORT01,"%s,%s,%s",usb_port[0].device_name[1], usb_port[0].folder_name[1], usb_port[0].system_type[1]);
    else
	sprintf(CONFIG.USB_PORT01,"");
    if(strlen(usb_port[0].device_name[2]) > 0 && strlen(usb_port[0].folder_name[2]) > 0 && strlen(usb_port[0].system_type[2]) > 0)
	sprintf(CONFIG.USB_PORT02,"%s,%s,%s",usb_port[0].device_name[2], usb_port[0].folder_name[2], usb_port[0].system_type[2]);
    else
	sprintf(CONFIG.USB_PORT02,"");
    if(strlen(usb_port[0].device_name[3]) > 0 && strlen(usb_port[0].folder_name[3]) > 0 && strlen(usb_port[0].system_type[3]) > 0)
	sprintf(CONFIG.USB_PORT03,"%s,%s,%s",usb_port[0].device_name[3], usb_port[0].folder_name[3], usb_port[0].system_type[3]);
    else
	sprintf(CONFIG.USB_PORT03,"");
    if(strlen(usb_port[0].device_name[4]) > 0 && strlen(usb_port[0].folder_name[4]) > 0 && strlen(usb_port[0].system_type[4]) > 0)
	sprintf(CONFIG.USB_PORT04,"%s,%s,%s",usb_port[0].device_name[4], usb_port[0].folder_name[4], usb_port[0].system_type[4]);
    else
	sprintf(CONFIG.USB_PORT04,"");
    if(strlen(usb_port[1].device_name[1]) > 0 && strlen(usb_port[1].folder_name[1]) > 0 && strlen(usb_port[1].system_type[1]) > 0)
	sprintf(CONFIG.USB_PORT11,"%s,%s,%s",usb_port[1].device_name[1], usb_port[1].folder_name[1], usb_port[1].system_type[1]);
    else
	sprintf(CONFIG.USB_PORT11,"");
    if(strlen(usb_port[1].device_name[2]) > 0 && strlen(usb_port[1].folder_name[2]) > 0 && strlen(usb_port[1].system_type[2]) > 0)
	sprintf(CONFIG.USB_PORT12,"%s,%s,%s",usb_port[1].device_name[2], usb_port[1].folder_name[2], usb_port[1].system_type[2]);
    else
	sprintf(CONFIG.USB_PORT12,"");
    if(strlen(usb_port[1].device_name[3]) > 0 && strlen(usb_port[1].folder_name[3]) > 0 && strlen(usb_port[1].system_type[3]) > 0)
	sprintf(CONFIG.USB_PORT13,"%s,%s,%s",usb_port[1].device_name[3], usb_port[1].folder_name[3], usb_port[1].system_type[3]);
    else
	sprintf(CONFIG.USB_PORT13,"");
    if(strlen(usb_port[1].device_name[4]) > 0 && strlen(usb_port[1].folder_name[4]) > 0 && strlen(usb_port[1].system_type[4]) > 0)
	sprintf(CONFIG.USB_PORT14,"%s,%s,%s",usb_port[1].device_name[4], usb_port[1].folder_name[4], usb_port[1].system_type[4]);
    else
	sprintf(CONFIG.USB_PORT14,"");
    if(strlen(usb_port[2].device_name[1]) > 0 && strlen(usb_port[2].folder_name[1]) > 0 && strlen(usb_port[2].system_type[1]) > 0)
	sprintf(CONFIG.USB_PORT21,"%s,%s,%s",usb_port[2].device_name[1], usb_port[2].folder_name[1], usb_port[2].system_type[1]);
    else
	sprintf(CONFIG.USB_PORT21,"");
    if(strlen(usb_port[2].device_name[2]) > 0 && strlen(usb_port[2].folder_name[2]) > 0 && strlen(usb_port[2].system_type[2]) > 0)
	sprintf(CONFIG.USB_PORT22,"%s,%s,%s",usb_port[2].device_name[2], usb_port[2].folder_name[2], usb_port[2].system_type[2]);
    else
	sprintf(CONFIG.USB_PORT22,"");
    if(strlen(usb_port[2].device_name[3]) > 0 && strlen(usb_port[2].folder_name[3]) > 0 && strlen(usb_port[2].system_type[3]) > 0)
	sprintf(CONFIG.USB_PORT23,"%s,%s,%s",usb_port[2].device_name[3], usb_port[2].folder_name[3], usb_port[2].system_type[3]);
    else
	sprintf(CONFIG.USB_PORT23,"");
    if(strlen(usb_port[2].device_name[4]) > 0 && strlen(usb_port[2].folder_name[4]) > 0 && strlen(usb_port[2].system_type[4]) > 0)
	sprintf(CONFIG.USB_PORT24,"%s,%s,%s",usb_port[2].device_name[4], usb_port[2].folder_name[4], usb_port[2].system_type[4]);
    else
	sprintf(CONFIG.USB_PORT24,"");
    if(strlen(usb_port[3].device_name[1]) > 0 && strlen(usb_port[3].folder_name[1]) > 0 && strlen(usb_port[3].system_type[1]) > 0)
	sprintf(CONFIG.USB_PORT31,"%s,%s,%s",usb_port[3].device_name[1], usb_port[3].folder_name[1], usb_port[3].system_type[1]);
    else
	sprintf(CONFIG.USB_PORT31,"");
    if(strlen(usb_port[3].device_name[2]) > 0 && strlen(usb_port[3].folder_name[2]) > 0 && strlen(usb_port[3].system_type[2]) > 0)
	sprintf(CONFIG.USB_PORT32,"%s,%s,%s",usb_port[3].device_name[2], usb_port[3].folder_name[2], usb_port[3].system_type[2]);
    else
	sprintf(CONFIG.USB_PORT32,"");
    if(strlen(usb_port[3].device_name[3]) > 0 && strlen(usb_port[3].folder_name[3]) > 0 && strlen(usb_port[3].system_type[3]) > 0)
	sprintf(CONFIG.USB_PORT33,"%s,%s,%s",usb_port[3].device_name[3], usb_port[3].folder_name[3], usb_port[3].system_type[3]);
    else
	sprintf(CONFIG.USB_PORT33,"");
    if(strlen(usb_port[3].device_name[4]) > 0 && strlen(usb_port[3].folder_name[4]) > 0 && strlen(usb_port[3].system_type[4]) > 0)
	sprintf(CONFIG.USB_PORT34,"%s,%s,%s",usb_port[3].device_name[4], usb_port[3].folder_name[4], usb_port[3].system_type[4]);
    else
	sprintf(CONFIG.USB_PORT34,"");
   #ifdef DEBUG
     printf("CONFIG.USB_PORT01 = %s\n", CONFIG.USB_PORT01);
     printf("CONFIG.USB_PORT02 = %s\n", CONFIG.USB_PORT02);
     printf("CONFIG.USB_PORT03 = %s\n", CONFIG.USB_PORT03);
     printf("CONFIG.USB_PORT04 = %s\n", CONFIG.USB_PORT04);
     printf("CONFIG.USB_PORT11 = %s\n", CONFIG.USB_PORT11);
     printf("CONFIG.USB_PORT12 = %s\n", CONFIG.USB_PORT12);
     printf("CONFIG.USB_PORT13 = %s\n", CONFIG.USB_PORT13);
     printf("CONFIG.USB_PORT14 = %s\n", CONFIG.USB_PORT14);
     printf("CONFIG.USB_PORT21 = %s\n", CONFIG.USB_PORT21);
     printf("CONFIG.USB_PORT22 = %s\n", CONFIG.USB_PORT22);
     printf("CONFIG.USB_PORT23 = %s\n", CONFIG.USB_PORT23);
     printf("CONFIG.USB_PORT24 = %s\n", CONFIG.USB_PORT24);
     printf("CONFIG.USB_PORT31 = %s\n", CONFIG.USB_PORT31);
     printf("CONFIG.USB_PORT32 = %s\n", CONFIG.USB_PORT32);
     printf("CONFIG.USB_PORT33 = %s\n", CONFIG.USB_PORT33);
     printf("CONFIG.USB_PORT34 = %s\n", CONFIG.USB_PORT34);
   #endif
     SaveConfiguration();
}

/*
void Set_usbdisk(char *data, FILE *out){

	ARGUMENT args[]={
	{"usb_share",CONFIG.USB_SHARE,2},
	{"recycle",CONFIG.USB_RECYCLE,2},
	{NULL,NULL,0}
    };
    parseargs(args);
    if(!strcmp(CONFIG.USB_SHARE,"1"))
	my_system(out,"/bin/mount /dev/sda1 /mnt");
    else
	my_system(out,"umount -r /dev/sda1");
    SaveConfiguration();
    fprintf(out,"<script language=\"JavaScript\">\n");
    fprintf(out,"location.replace('http://%s:%s/usbdisk.htm');\n",CONFIG.IP,CONFIG.ADMIN_PORT);
    fprintf(out,"</script>\n");
}
*/
void Set_printer(FILE *out){

	FILE *fp;
  char Buf[20],*urlcgi_data;

	ARGUMENT args[]={
							  {"enable_pserver", usb_printer.server, 2},
							  //{"enable_pserver2", usb_printer.server2, 2},
							  //{"type", usb_printer.type, 5},
							  //{"size", usb_printer.size, 5},
							  //{"rect", usb_printer.rect, 5},
							  //{"tray", usb_printer.tray, 5},
							  //{"feeder", usb_printer.feeder, 5},
							  //{"reverse", usb_printer.reverse, 5},
							  //{"dectance", usb_printer.dectance, 5},
							  //{"paper", usb_printer.paper, 5},
							  //{"rect2", usb_printer.rect2, 5},
							  //{"qui", usb_printer.qui, 5},
							  //{"ink", usb_printer.ink, 5},
							  //{"printer_scrpage", usb_printer.scrpage, 5},
							{NULL,NULL,0}
  };
    parseargs(args);
    //#ifdef DEBUG1
    printf("usb_printer.server=%s\n",usb_printer.server);
    //printf("usb_printer.server2=%s\n",usb_printer.server2);
    //printf("usb print port=%s\n",usb_printer.port);
    //#endif
    //if(!strcmp(usb_printer.server,"0") && !strcmp(usb_printer.server2,"0")){
    if(!strcmp(usb_printer.server,"0")){
	sprintf(Buf,"killall lpd");
	system(Buf);
	sprintf(Buf,"rm -f /etc/printcap");
	system(Buf);
	//andy 2005
	sprintf(Buf,"kill -SIGHUP `pidof smbd`");
	system(Buf);
    }
    else if(!strcmp(usb_printer.server,"1")){
	//system("/etc/init.d/update_printer.sh");
	sprintf(Buf,"/etc/init.d/update_printer.sh");
	my_system(out,Buf);
    }
    //strcpy(CONFIG.PRINTER_ENABLE,usb_printer.server);

		/*
		printf("usb_printer.scrpage=%s\n",usb_printer.scrpage);
		if(!strcmp(usb_printer.scrpage,"1")){
				strcpy(urlcgi_data,"Set_printer.cgi");
				printf("urlcgi_data=%s\n",urlcgi_data);
				Printer_scrpage(urlcgi_data,out);
		}
		if(printer_first_start == 1){
				strcpy(usb_printer.server,CONFIG.PRINTER_ENABLE);
				strcpy(usb_printer.type,CONFIG.TYPE);
				strcpy(usb_printer.size,CONFIG.SIZE);
				strcpy(usb_printer.rect,CONFIG.RECT);
				strcpy(usb_printer.tray,CONFIG.TRAY);
				strcpy(usb_printer.feeder,CONFIG.FEEDER);
				strcpy(usb_printer.reverse,CONFIG.REVERSE);
				strcpy(usb_printer.dectance,CONFIG.DECTANCE);
				strcpy(usb_printer.paper,CONFIG.PAPER);
				strcpy(usb_printer.rect2,CONFIG.RECT2);
				strcpy(usb_printer.qui,CONFIG.QUI);
				strcpy(usb_printer.ink,CONFIG.INK);
		    printer_first_start=0;
		}

	if((fp = fopen("/htdocs/printer1.htm","r")) == NULL){
	//fclose(fp);
	return;
  }
  else{
      while(fgets(LineBuf,255,fp) != NULL)
	    fprintf(out,LineBuf);
      fclose(fp);
  }
  */
  /*
	    //Printer Server(enable_pserver)
	    fprintf(out,"<FORM name=\"main\" action=\"Set_printer.cgi\" method=\"get\"><TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Print Server</TH>\n");
		if(!strcmp(usb_printer.server,"1"))
	 fprintf(out,"<TD width=400>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\" onclick=\"cmdShareLpOn()\" checked>Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\" onclick=\"cmdShareLpOff()\">No</TD></TR>\n");
		else if(!strcmp(usb_printer.server,"0"))
		   fprintf(out,"<TD width=400>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\" onclick=\"cmdShareLpOn()\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\" onclick=\"cmdShareLpOff()\" checked>No</TD></TR>\n");
      else
		   fprintf(out,"<TD width=400>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\" onclick=\"cmdShareLpOn()\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\" onclick=\"cmdShareLpOff()\">No</TD></TR>\n");
      //Printer Type(type)
		fprintf(out,"<TR bgColor=#333399><TD class=style2 colSpan=2 height=30>&nbsp;&nbsp;Custom Printing</TD></TR>\n");
			fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Printer Type</TH>\n");
			if(!strcmp(usb_printer.type,"0"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\" selected>PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"1"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\" selected>EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"2"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\" selected>EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"3"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\" selected>EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"4"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\" selected>EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"5"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\" selected>EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"6"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\" selected>EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"7"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\" selected>EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"8"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\" selected>EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"9"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\" selected>EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"10"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\" selected>EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"11"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\" selected>EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"13"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\" selected>EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"14"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\" selected>EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"15"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\" selected>EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"16"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\" selected>EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"17"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\" selected>EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"18"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\" selected>EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"19"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\" selected>EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.type,"20"))
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\" selected>EPSON PM-730C</option></select></TD></TR>\n");
		  else
			fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"type\" size=\"1\" onChange=\"cmdChangePrinter()\"><option value=\"0\">PostScript</option><option value=\"1\">EPSON LP-9500C</option><option value=\"2\">EPSON LP-8800C</option><option value=\"3\">EPSON LP-9400</option><option value=\"4\">EPSON LP-8900</option><option value=\"5\">EPSON LP-8700</option><option value=\"6\">EPSON LP-8100</option><option value=\"7\">EPSON LP-7700</option><option value=\"8\">EPSON LP-7500</option><option value=\"9\">EPSON LP-2400</option><option value=\"10\">EPSON LP-2200</option><option value=\"11\">EPSON LP-1900</option><option value=\"13\">EPSON PM-890C</option><option value=\"14\">EPSON PM-870C</option><option value=\"15\">EPSON PM-850PT</option><option value=\"16\">EPSON PM-840C</option><option value=\"17\">EPSON PM-830C</option><option value=\"18\">EPSON PM-740DU</option><option value=\"19\">EPSON PM-740C</option><option value=\"20\">EPSON PM-730C</option></select></TD></TR>\n");
		  //Paper Size(size)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Paper Size</TH>\n");
		  if(!strcmp(usb_printer.size,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\" selected>A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\" selected>A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\" selected>A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"3"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\" selected>B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"4"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\" selected>B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"5"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\" selected>Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"7"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\" selected>Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"10"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\" selected>Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.size,"11"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\" selected>European 4</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"size\" size=\"1\"><option value=\"0\">A3</option><option value=\"1\">A4(Standard)</option><option value=\"2\">A5</option><option value=\"3\">B4</option><option value=\"4\">B5</option><option value=\"5\">Letter</option><option value=\"7\">Legal</option><option value=\"10\">Postcard</option><option value=\"11\">European 4</option></select></TD></TR>\n");
		  //Resolution(rect)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Resolution</TH>\n");
		  if(!strcmp(usb_printer.rect,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect\" size=\"1\"><option value=\"0\" selected>300x300</option><option value=\"1\">600x600(Standard)</option><option value=\"2\">1200x1200</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.rect,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect\" size=\"1\"><option value=\"0\">300x300</option><option value=\"1\" selected>600x600(Standard)</option><option value=\"2\">1200x1200</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.rect,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect\" size=\"1\"><option value=\"0\">300x300</option><option value=\"1\">600x600(Standard)</option><option value=\"2\" selected>1200x1200</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect\" size=\"1\"><option value=\"0\">300x300</option><option value=\"1\">600x600(Standard)</option><option value=\"2\">1200x1200</option></select></TD></TR>\n");
		  //Paper Tray(tray)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Paper Tray</TH>\n");
		  if(!strcmp(usb_printer.tray,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"tray\" size=\"1\"><option value=\"0\" selected>Automatic(Standard)</option><option value=\"1\">Manual</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.tray,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"tray\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\" selected>Manual</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"tray\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">Manual</option></select></TD></TR>\n");
		  //Resolution(feeder)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Resolution</TH>\n");
		  if(!strcmp(usb_printer.feeder,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\" selected>Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\" selected>1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\" selected>2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"3"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\" selected>3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"4"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\" selected>4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"5"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\" selected>5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"6"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\" selected>6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"7"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\" selected>7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"8"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\" selected>8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"9"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\" selected>9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"10"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\" selected>10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"11"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\" selected>11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"12"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\" selected>12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"13"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\" selected>13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"14"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\" selected>14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.feeder,"15"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\" selected>15</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"feeder\" size=\"1\"><option value=\"0\">Automatic(Standard)</option><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option><option value=\"11\">11</option><option value=\"12\">12</option><option value=\"13\">13</option><option value=\"14\">14</option><option value=\"15\">15</option></select></TD></TR>\n");
		  //Both Sides(reverse)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Both Sides</TH>\n");
		  if(!strcmp(usb_printer.reverse,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"reverse\" size=\"1\"><option value=\"0\" selected>Single Side(Standard)</option><option value=\"1\">Both Sides</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.reverse,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"reverse\" size=\"1\"><option value=\"0\">Single Side(Standard)</option><option value=\"1\" selected>Both Sides</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"reverse\" size=\"1\"><option value=\"0\">Single Side(Standard)</option><option value=\"1\">Both Sides</option></select></TD></TR>\n");
		  //Orientation(dectance)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Orientation</TH>\n");
		  if(!strcmp(usb_printer.dectance,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"dectance\" size=\"1\"><option value=\"0\" selected>Portrait(Standard)</option><option value=\"1\">Landscape</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.dectance,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"dectance\" size=\"1\"><option value=\"0\">Portrait(Standard)</option><option value=\"1\" selected>Landscape</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"dectance\" size=\"1\"><option value=\"0\">Portrait(Standard)</option><option value=\"1\">Landscape</option></select></TD></TR>\n");
		  //Paper Type(paper)
		  fprintf(out,"<TR bgColor=#333399><TD class=style2 colSpan=2 height=30>&nbsp;&nbsp;Inkjet Printing</TD></TR>\n");
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Paper Type</TH>\n");
		  if(!strcmp(usb_printer.paper,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\" selected>Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\" selected>Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\" selected>Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"3"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\" selected>Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"4"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\" selected>Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"5"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\" selected>Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"6"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\" selected>PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.paper,"7"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\" selected>PM Mat Paper</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"paper\" size=\"1\"><option value=\"0\">Normal Paper(Standard)</option><option value=\"1\">Super Fine Paper</option><option value=\"2\">Photo Print Paper</option><option value=\"3\">Exclusive Gloss Film</option><option value=\"4\">Exclusive OHP Sheet</option><option value=\"5\">Iron-on Transfer</option><option value=\"6\">PM Photo Paper</option><option value=\"7\">PM Mat Paper</option></select></TD></TR>\n");
		  //Resolution(rect2)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Resolution</TH>\n");
		  if(!strcmp(usb_printer.rect2,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect2\" size=\"1\"><option value=\"0\" selected>120dpi</option><option value=\"1\">360dpi(Standard)</option><option value=\"2\">720dpi</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.rect2,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect2\" size=\"1\"><option value=\"0\">120dpi</option><option value=\"1\" selected>360dpi(Standard)</option><option value=\"2\">720dpi</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.rect2,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect2\" size=\"1\"><option value=\"0\">120dpi</option><option value=\"1\">360dpi(Standard)</option><option value=\"2\" selected>720dpi</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"rect2\" size=\"1\"><option value=\"0\">120dpi</option><option value=\"1\">360dpi(Standard)</option><option value=\"2\">720dpi</option></select></TD></TR>\n");
		  //Print Quality(qui)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Print Quality</TH>\n");
		  if(!strcmp(usb_printer.qui,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\">Photograph(Standard)</option><option value=\"2\">Standard</option><option value=\"3\">Fine</option><option value=\"4\">Super fine</option><option value=\"0\" selected>Draft</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.qui,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\" selected>Photograph(Standard)</option><option value=\"2\">Standard</option><option value=\"3\">Fine</option><option value=\"4\">Super fine</option><option value=\"0\">Draft</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.qui,"2"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\">Photograph(Standard)</option><option value=\"2\" selected>Standard</option><option value=\"3\">Fine</option><option value=\"4\">Super fine</option><option value=\"0\">Draft</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.qui,"3"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\">Photograph(Standard)</option><option value=\"2\">Standard</option><option value=\"3\" selected>Fine</option><option value=\"4\">Super fine</option><option value=\"0\">Draft</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.qui,"4"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\">Photograph(Standard)</option><option value=\"2\">Standard</option><option value=\"3\">Fine</option><option value=\"4\" selected>Super fine</option><option value=\"0\">Draft</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"qui\" size=\"1\"><option value=\"1\">Photograph(Standard)</option><option value=\"2\">Standard</option><option value=\"3\">Fine</option><option value=\"4\">Super fine</option><option value=\"0\">Draft</option></select></TD></TR>\n");
		  //Ink(ink)
		  fprintf(out,"<TR><TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Ink</TH>\n");
		  if(!strcmp(usb_printer.ink,"0"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"ink\" size=\"1\"><option value=\"0\" selected>Color(Standard)</option><option value=\"1\">Monochrome</option></select></TD></TR>\n");
		  else if(!strcmp(usb_printer.ink,"1"))
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"ink\" size=\"1\"><option value=\"0\">Color(Standard)</option><option value=\"1\" selected>Monochrome</option></select></TD></TR>\n");
		  else
		  fprintf(out,"<TD width=400>&nbsp;&nbsp;<select name=\"ink\" size=\"1\"><option value=\"0\">Color(Standard)</option><option value=\"1\">Monochrome</option></select></TD></TR>\n");

		  fprintf(out,"<input type=\"hidden\" name=\"printer_scrpage\" value=\"1\">\n");//control value to printer_scrpage.cgi

		  fprintf(out,"<TR bgColor=#333399><TD class=style2 height=30>&nbsp;&nbsp;</TD><TD height=30>&nbsp;&nbsp; \n");
		  //fprintf(out,"<INPUT type=submit value=\"Apply\" name=id_set onclick=\"location.replace('Printer_scrpage.cgi');\">&nbsp;&nbsp;<INPUT onclick=\"history.back()\" type=button value=\"Back\" name=id_cancel>\n");
		  fprintf(out,"<INPUT type=submit value=\"Apply\" name=id_set>&nbsp;&nbsp;<INPUT onclick=\"history.back()\" type=button value=\"Back\" name=id_cancel>\n");
		  fprintf(out,"</TD></FORM></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>\n");
			fprintf(out,"<MAP name=Map><AREA shape=RECT coords=10,1,109,23 href=\"Show_USB_Detail.cgi\">\n");
			fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"Show_USB_Disk.cgi\"><AREA shape=RECT coords=210,1,318,23 href=\"Set_printer.cgi\">\n");
			fprintf(out,"<AREA shape=RECT coords=321,1,512,23 href=\"Set_usbque.cgi\"></MAP></BODY></HTML>\n");

	    strcpy(CONFIG.PRINTER_ENABLE,usb_printer.server);
			strcpy(CONFIG.TYPE,usb_printer.type);
			strcpy(CONFIG.SIZE,usb_printer.size);
			strcpy(CONFIG.RECT,usb_printer.rect);
			strcpy(CONFIG.TRAY,usb_printer.tray);
			strcpy(CONFIG.FEEDER,usb_printer.feeder);
			strcpy(CONFIG.REVERSE,usb_printer.reverse);
			strcpy(CONFIG.DECTANCE,usb_printer.dectance);
			strcpy(CONFIG.PAPER,usb_printer.paper);
			strcpy(CONFIG.RECT2,usb_printer.rect2);
			strcpy(CONFIG.QUI,usb_printer.qui);
			strcpy(CONFIG.INK,usb_printer.ink);
	    */
	    strcpy(CONFIG.PRINTER_ENABLE,usb_printer.server);
	    //strcpy(CONFIG.PRINTER2_ENABLE,usb_printer.server2);
	    SaveConfiguration();
	    //return;
	    //printer_scrpage(out);
	    printf("before Show_USB_Printer.cgi\n");
	    boot_page(out,"Show_USB_Printer.cgi");

}

void Set_usbque(FILE *out){

	//FILE *fp;
  char Buf[256];

  //sprintf(Buf,"/mnt/lprm");
  sprintf(Buf,"/mnt/lprm `/mnt/lpq | grep -v active | sed -n '/root/s/.*+\\([0-9]\\+\\).*/\\1/pg'`");
  system(Buf);
  /*
	ARGUMENT args[]={
							  {"printer_scrpage", usb_printer.scrpage, 5},
							{NULL,NULL,0}
  };

    parseargs(args);

		printf("usb_printer.scrpage=%s\n",usb_printer.scrpage);
		if(!strcmp(usb_printer.scrpage,"1")){
				strcpy(urlcgi_data,"Set_usbque.cgi");
				printf("urlcgi_data=%s\n",urlcgi_data);
				Printer_scrpage(urlcgi_data,out);
				//strcmp(usb_printer.scrpage,"0");
		}

			fprintf(out,"<HTML><HEAD><TITLE>Gigabit SOHO NAS</TITLE><link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n");
			fprintf(out,"<script language=\"JavaScript1.2\">function Fcheck() {	return confirm(\"Are you sure you want to clear the print queue?\");}</script></HEAD>\n");
			fprintf(out,"<BODY text=#000000 bgColor=#ffffff><P><FONT class=style9>USB Setup</FONT></P>\n");
			fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(out,"<tr><td width=\"100%%\" bgcolor=\"#FFFFFF\" height=\"31\"><div align=\"left\"><img src=\"images/usb_que.gif\" usemap=\"#Map\" border=\"0\"></div></td></tr>\n");
			fprintf(out,"<tr height=\"005\"><td></td></tr></table>\n");
			fprintf(out,"<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 border=1><TBODY>\n");
			fprintf(out,"<form name=host method=\"Get\" action=\"Set_usbque.cgi\" onSubmit=\"return Fcheck()\">\n");
			fprintf(out,"<tr bgcolor=#333399><td width=\"140\" height=\"30\" class=\"style2\">&nbsp;&nbsp;Delete Print Queue</td>\n");
			fprintf(out,"<td width=\"410\" height=\"30\">&nbsp;&nbsp;<input type=\"submit\" value=\"Execute\">&nbsp;&nbsp;<INPUT type=\"button\" value=\"Cancel\" onclick=\"history.back()\"></td></tr>\n");

			fprintf(out,"<input type=\"hidden\" name=\"printer_scrpage\" value=\"1\">\n");//control value to printer_scrpage.cgi

			fprintf(out,"<tr class=\"style4\"><th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th>\n");
			fprintf(out,"<td bgcolor=\"#FFFFFF\">&nbsp;&nbsp;By clearing the print queue of HD-HLAN0DE, all current and pending print<br>\n");
			fprintf(out,"&nbsp;&nbsp;jobs will be deleted and need to be resent to the printer to complete. The<br>&nbsp;&nbsp;process cannot be stopped once it has begun.</td></tr></TBODY></TABLE>\n");
			fprintf(out,"<MAP name=Map><AREA shape=RECT coords=10,1,109,23 href=\"Show_USB_Detail.cgi\"><AREA shape=RECT coords=110,1,209,23 href=\"Show_USB_Disk.cgi\">\n");
			fprintf(out,"<AREA shape=RECT coords=210,1,318,23 href=\"printer.htm\"><AREA shape=RECT coords=321,1,512,23 href=\"Set_usbque.cgi\"></MAP></form></BODY></HTML>\n");
	*/
	boot_page(out,"usbque.htm");
}

void Printer_scrpage(FILE *out ){

	    FILE *fp;
	    char LineBuf[256],urlcgi_data_temp[20];

	    printf("Starting printer_scrpage rightnow!!\n");
	    strcpy(urlcgi_data_temp,arg);
	    printf("urlcgi_data_temp=%s\n",urlcgi_data_temp);
	    strcpy(usb_printer.scrpage,"");
	    printf("after usb_printer.scrpage=%s\n",usb_printer.scrpage);

	    if((fp=fopen("/htdocs/printer_scrpage.htm","r")) == NULL)
		NULL;
	    else{
		while(fgets(LineBuf,255,fp) != NULL)
		    fprintf(out,LineBuf);
		fclose(fp);
	    }
	    //fprintf(out,"<tr bgcolor=#333399> <td class=\"style2\" width=\"602\"><B>Saving Settings to Memory</FONT></B><br></td></tr></table>");
			fprintf(out,"<table border=\"0\" width=\"90%%\" align=\"center\" align=\"center\"><tr><td><font size=4 color=\"black\">\n");
			fprintf(out,"<br><b>Settings have been saved successfully.</td></tr></table>");
			sleep(1);
			boot_page(out,urlcgi_data_temp);
}

void Show_USB_Printer(FILE *out ){

	//printf("here is Show_USB_Printer.cgi\n");
	typedef struct{
	    char version[5];
	    char manufacturer[50];
	    char product[50];
	    int num_interface;
	    char usbclass[10][50];
	}USB_DEVICE;

	FILE *fp;
	char *ptr, *end;
	USB_DEVICE USB_DATA[2];
	char LineBuf[256], interface[5], color[8];
	int i, j, usb=0, num_face=0, num=0, count=0, color_count=0;

  for(i=0;i<2;i++){
	    memset(USB_DATA[i].version,0,5);
	    memset(USB_DATA[i].manufacturer,0,50);
	    memset(USB_DATA[i].product,0,50);
	    USB_DATA[i].num_interface = 0;
	    for(j=0;j<10;j++)
		memset(USB_DATA[i].usbclass[j],0,50);
	}

  if((fp = fopen("/htdocs/printer1.htm","r")) == NULL){
	//fclose(fp);
	return;
  }
  else{
      while(fgets(LineBuf,255,fp) != NULL)
	    fprintf(out,LineBuf);
      fclose(fp);
  }
     //#ifdef DEBUG1
     //printf("CONFIG.PORT=%s\n",CONFIG.PORT);

     system("cat /proc/bus/usb/devices > /var/devices");
	if((fp = fopen("/var/devices","r")) != NULL){
		while(fgets(LineBuf,255,fp)){
			if(strstr(LineBuf,"T:  Bus=")){
				ptr = LineBuf;
				ptr = strstr(ptr,"Lev=");
				ptr+=4;
				end = strchr(ptr,' ');
				*end = '\0';
				if(atoi(ptr) != 0)
				    usb = 1;
				else
				    usb = 0;
				ptr = end + 1;
				ptr =strstr(ptr,"Prnt=");
				ptr+=5;
				end = strchr(ptr,' ');
				*end = '\0';
				if(atoi(ptr) != 0)
				    usb = 1;
				else
				    usb = 0;
				ptr = end + 1;
				ptr = strstr(ptr,"Port=");
				ptr+=5;
				end = strchr(ptr,' ');
				*end = '\0';
				if(atoi(ptr) != 0)
				    usb = 1;
				else
				    usb = 0;
				ptr = end + 1;
				ptr = strstr(ptr,"Cnt=");
				ptr+=4;
				end = strchr(ptr,' ');
				*end = '\0';
				if(atoi(ptr) != 0)
				    usb = 1;
				else
				    usb = 0;
			}
			if((usb == 1) && strstr(LineBuf,"Ver=")){
				ptr = strstr(LineBuf,"Ver=");
				ptr+=5;
				strtok(ptr," ");
				strcpy(USB_DATA[num].version,ptr);
				end = strrchr(USB_DATA[num].version,'0');
				if(end != NULL)
				    *end = '\0';
				//printf("USB_DATA[%d].version = %s\n",num,USB_DATA[num].version);
				count++;
			}
			if((usb == 1) && strstr(LineBuf,"Manufacturer=")){
				ptr = strstr(LineBuf,"Manufacturer=");
				ptr+=13;
				strtok(ptr,"\n");
				strcpy(USB_DATA[num].manufacturer,ptr);
				//printf("USB_DATA[%d].manufacturer = %s\n",num,USB_DATA[num].manufacturer);
		    }
		    if((usb == 1) && strstr(LineBuf,"Product=")){
			ptr = strstr(LineBuf,"Product=");
			ptr+=8;
			strtok(ptr,"\n");
			strcpy(USB_DATA[num].product,ptr);
			//printf("USB_DATA[%d].product = %s\n",num,USB_DATA[num].product);
		    }
		    if((usb == 1) && strstr(LineBuf,"#Ifs=")){
			ptr = strstr(LineBuf,"#Ifs=");
			ptr+=6;
			strtok(ptr," ");
			strcpy(interface,ptr);
			USB_DATA[num].num_interface = atoi(interface);
			//printf("USB_DATA[%d].num_interface = %d\n",num,USB_DATA[num].num_interface);
		    }
		    if((usb == 1) && strstr(LineBuf,"If#=") && strstr(LineBuf,"Cls=")){
			ptr = strstr(LineBuf,"If#=");
			ptr+=5;
			end = strchr(ptr,' ');
			*end = '\0';
			num_face = atoi(ptr);
			#ifdef DEBUG1
			printf("num_face = %d\n",num_face);
			#endif
			ptr = end + 1;
			ptr = strstr(ptr,"Cls=");
			ptr+=7;
			end = strchr(ptr,')');
			*end = '\0';
			strcpy(USB_DATA[num].usbclass[num_face],ptr);
			//printf("USB_DATA[%d].usbclass[%d] = %s\n",num,num_face,USB_DATA[num].usbclass[num_face]);
			if(num_face == USB_DATA[num].num_interface - 1)
			    num++;
		    }
		    if(count > 2)
			break;
		}
		fclose(fp);
		//system("rm -f /var/devices");
	}
     for(i=0;i<2;i++){
     //printf("USB_DATA[%d].product=%s\n",i,USB_DATA[i].product);
     if(!strlen(USB_DATA[i].product) || (strcmp(USB_DATA[i].usbclass[0],"print") && strcmp(USB_DATA[i].usbclass[1],"print")
     && strcmp(USB_DATA[i].usbclass[2],"print") && strcmp(USB_DATA[i].usbclass[3],"print")))
		strcpy(USB_DATA[i].product,"(cannot be acquired)");
     }
     #ifdef DEBUG1
       printf("CONFIG.PRINTER_ENABLE=%s\n",CONFIG.PRINTER_ENABLE);
       printf("CONFIG.PRINTER2_ENABLE=%s\n",CONFIG.PRINTER2_ENABLE);
     #endif

	    fprintf(out,"<FORM name=\"main\" action=\"Set_printer.cgi\" method=\"get\">\n");

#ifdef MICRONET
		fprintf(out,"<TR><TH align=center bgColor=#eeffff><font color=\"#006666\">&nbsp;&nbsp;Printer Enable</TH>\n");
#else
		fprintf(out,"<TR><TH align=center bgColor=#dddddd>&nbsp;&nbsp;Printer Enable</TH>\n");
#endif
		fprintf(out,"<TD align=center>&nbsp;&nbsp;%s</TD>\n",USB_DATA[0].product);
		if(!strcmp(CONFIG.PRINTER_ENABLE,"1"))
	 fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\" checked>Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\">No</TD></TR>\n");
		else if(!strcmp(CONFIG.PRINTER_ENABLE,"0"))
		   fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\" checked>No</TD></TR>\n");
      else
		   fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"1\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver\" value=\"0\">No</TD></TR>\n");
		/*
		fprintf(out,"<TR><TH align=center bgColor=#dddddd>&nbsp;&nbsp;Printer 2 Enable</TH>\n");
		fprintf(out,"<TD align=center>&nbsp;&nbsp;%s</TD>\n",USB_DATA[1].product);
		if(!strcmp(CONFIG.PRINTER2_ENABLE,"1"))
	 fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"1\" checked>Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"0\">No</TD></TR>\n");
		else if(!strcmp(CONFIG.PRINTER2_ENABLE,"0"))
		   fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"1\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"0\" checked>No</TD></TR>\n");
      else
		   fprintf(out,"<TD align=center>&nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"1\">Yes &nbsp;&nbsp;<input type=\"radio\" name=\"enable_pserver2\" value=\"0\">No</TD></TR>\n");
		*/

			fprintf(out,"<TR><TD class=longshine_2 colspan=3 height=30 align=center>&nbsp;&nbsp;&nbsp;&nbsp; \n");

		  //fprintf(out,"<INPUT type=submit value=\"Apply\" name=id_set onclick=\"location.replace('Printer_scrpage.cgi');\">&nbsp;&nbsp;<INPUT onclick=\"history.back()\" type=button value=\"Back\" name=id_cancel>\n");

		  if(strstr(USB_DATA[0].product,"cannot be acquired"))
		    fprintf(out,"<INPUT type=submit value=\"Apply\" name=id_set disabled>&nbsp;&nbsp;<INPUT onclick=\"history.back()\" type=button value=\"Back\" name=id_cancel></TD></TR>\n");
		  else
		    fprintf(out,"<INPUT type=submit value=\"Apply\" name=id_set>&nbsp;&nbsp;<INPUT onclick=\"history.back()\" type=button value=\"Back\" name=id_cancel></TD></TR>\n");
		  /*
		  fprintf(out,"<tr class=\"style4\"><th bgcolor=\"#FFFFFF\" align=center>&nbsp;&nbsp;Note</th><td bgcolor=\"#FFFFFF\" colspan=2>\n");
      fprintf(out,"&nbsp;&nbsp;The option will be decide printer that driver can be enable or <br>&nbsp;&nbsp;disable.\n");
      fprintf(out,"&nbsp;&nbsp;User will press apply button to execute driver through <br>&nbsp;&nbsp;printer setting\n");
      fprintf(out,"&nbsp;&nbsp;web page.Process cannot be stopped once it has <br>&nbsp;&nbsp;begun.</td></tr>\n");
		  */
		  /*
		  fprintf(out,"<tr class=\"style4\"><th bgcolor=\"#FFFFFF\" align=left>&nbsp;&nbsp;Note</th><td bgcolor=\"#FFFFFF\"  colspan=2>\n");
		  fprintf(out,"&nbsp;&nbsp;The option will be decide printer that driver can be enable or disable.<br>\n");
		  fprintf(out,"&nbsp;&nbsp;User will press apply button to execute driver through printer setting<br>\n");
		  fprintf(out,"&nbsp;&nbsp;web page.Process cannot be stopped once it has begun.</td></tr>\n");
		  */
		  fprintf(out,"</FORM></TBODY></TABLE></TD></TR></TBODY></TABLE>\n");
			fprintf(out,"<MAP name=Map><AREA shape=RECT coords=10,1,109,23 href=\"Show_USB_Detail.cgi\">\n");
			fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"Show_USB_Disk.cgi\"><AREA shape=RECT coords=210,1,318,23 href=\"Show_USB_Printer.cgi\">\n");
			fprintf(out,"<AREA shape=RECT coords=321,1,512,23 href=\"usbque.htm\"></MAP></BODY></HTML>\n");
}

/*
void Set_usbadvanced(char *data, FILE *out ){

	  printf("data=%s\n",data);

		ARGUMENT args[]={
									  {"advanced_sel", usbadvanced.sel, 20},
									  {"mount_name", usbadvanced.name, 15},
									  //{"size", usb_printer.size, 5},
									  //{"rect", usb_printer.rect, 5},
									  //{"tray", usb_printer.tray, 5},
									  //{"feeder", usb_printer.feeder, 5},
									  //{"reverse", usb_printer.reverse, 5},
									  //{"dectance", usb_printer.dectance, 5},
									  //{"paper", usb_printer.paper, 5},
									  //{"rect2", usb_printer.rect2, 5},
									  //{"qui", usb_printer.qui, 5},
									  //{"ink", usb_printer.ink, 5},
									  //{"printer_scrpage", usb_printer.scrpage, 5},
									{NULL,NULL,0}
		  };
    parseargs(args);

    printf("usbadvanced.sel=%s\n",usbadvanced.sel);
    printf("usbadvanced.name=%s\n",usbadvanced.name);

		if(!strcmp(usbadvanced.sel,"0")){
				//sp
				#ifdef HTTPD_MAX_ACCEPT
					add_num_crt();
					current_num_crt =1;
				#endif//#ifdef HTTPD_MAX_ACCEPT

					//snop adds
					get_hdcapacity();
					if( hdcapacity == 0 ){
						Error_hdformat( out );
						return;
					}
					Set_hdformat_m( data, out );
				#ifdef EXT3
					my_system(out, "/etc/rc.d/mkEXT3disk.sh /dev/hda &");
				#else
					my_system("/etc/rc.d/mkFAT32disk.sh /dev/hda &");
				#endif
		} // if adjustment
		else if(!strcmp(usbadvanced.sel,"1")){
		boot_page(out,"advanced_func1.htm");
    }

}
*/

void Set_usbhdformat_m(char *data, FILE *out){//data is not used RAW

	FILE *fp,*fp1;
	char Line_buf[READ_SHORT+1],usb_share[128];
	char *end1=NULL,*end2;
	int i;
	int num=0;
	char num2[5];
	char temp[50];

	static int port;

	printf("%s: data:%s\n",__func__,data);

    ARGUMENT args[]={
	{"usb_share", usb_share, 128},
	{"percent", num2, 4},
	{NULL,NULL,0}
    };
//    handle_arg(data);
    parseargs(args);
    if(strlen(usb_share)!=0){
		if( (fp1=fopen("/tmp/usb.log","r")) != NULL ){
			while(fgets(Line_buf,255,fp1)!=NULL){
				if(strstr(Line_buf,usb_share)){
					end1=strrchr(Line_buf,':');
					end2=end1+5;
					end1=end1+6;
					*end1='\0';
					port=atoi(end2);
					break;
				}
			}
			fclose(fp1);
		}
    }



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
			//Restart_System( data, out);
			fclose(fp);
			system("rm /etc/percent");


			//mount USB HD back
			 sprintf(temp, "/etc/rc.d/usb_pnp%d 1",port);
			 printf("temp: %s\n",temp);
			 my_system(out,temp);

			boot_page(out,"Show_USB_Disk.cgi");
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
#ifdef NOVAC
    if((fp=fopen("/htdocs/JavaReturn_hd.htm","r")) == NULL)
#elif defined(CENTURY) || defined(HAWKING) || defined(CENTURY)
     if((fp=fopen("/htdocs/JavaReturn_h.htm","r")) == NULL)
#else
    if((fp=fopen("/htdocs/JavaReturn_hd_warm.htm","r")) == NULL)
#endif
	return;
    else{
	while(fgets(Line_buf,READ_SHORT,fp) != NULL)
	    fprintf(out,"%s",Line_buf);
	fclose(fp);
    }

   #if defined(LOBOS)
       fprintf(out,"<td bgColor=#074420 width=\"802\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#ffffff size=3>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;VXe? | tH[}bg</FONT></B><br></td></tr></table>\n");
       fprintf(out,"<br><br><TABLE width=\"80%\" align=center border=0><TR><td colspan=20>tH[}bgÉÍµÎç­?Ôª©©èÜ·B<br>tH[}bgªI¹·éÆgbvy[Wª\n");
       fprintf(out,"\\¦\n");
       fprintf(out,"³êéÌÅAu?EUðÂ¶È¢Å­¾³¢B</td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr>\n");
   #elif defined(NOVAC)
       fprintf(out,"<td width=\"100%\" bgcolor=\"#FFFFFF\">\n<font size=3>?ú»?Å·B</font><br><br>\n");
       fprintf(out,"		?ú»ÉÍµÎç­?Ôª©©èÜ·B?ú»ªI¹·éÜÅA±ÌÜÜ¨Ò¿­¾³¢B<br>\n");
       fprintf(out,"</td>\n</tr>\n<tr height=\"005\"><td></td></tr>\n</table><br>\n");
       fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>\n");
   #elif defined(CENTURY)
       fprintf(out,"<td bgColor=#DFEFFF width=\"550\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#000000 size=3>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;VXe? | tH[}bg</FONT></B><br></td></tr></table>\n");
       fprintf(out,"<br><br><TABLE width=\"550\" align=center border=0 bgcolor=\"#FFFFFF\"><TR><td colspan=20>tH[}bgÉÍµÎç­?Ôª©©èÜ·B<br>tH[}bgªI¹·éÆgbvy[Wª\n");
       fprintf(out,"\\¦\n");
       fprintf(out,"³êéÌÅAu?EUðÂ¶È¢Å­¾³¢B</td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr>\n");
   #elif defined(HAWKING)
       fprintf(out,"<td bgColor=#"get_banner_bg" width=\"602\"><B><FONT face=\"Arial, Helvetica, sans-serif\" color=#"get_bar_font_color" size=3>Administrator - Format HD...</FONT></B><br></td></tr></table>");
	   fprintf(out,"<br><br><TABLE width=\"80%%\" align=center border=0><TR><td colspan=20><font color=\"#%s\">This may take a few minutes.<br>Please wait for the system to reload the page. Do not close this window.</font></td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr><tr>",get_font_color());
   #else
       fprintf(out,"<td width=\"100%%\" bgcolor=\"#FFFFFF\">\n<font size=3>&nbsp;&nbsp;Format HD</font><br><br>\n");
       fprintf(out,"&nbsp;&nbsp;This may take a few minutes.<br>&nbsp;&nbsp;Please wait for the system to reload the page. Do not close this window.<br>\n");
       fprintf(out,"</td>\n</tr>\n<tr height=\"005\"><td></td></tr>\n</table><br>\n");
       fprintf(out,"<table width=\"550\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>&nbsp;&nbsp;\n");
   #endif
	for(i=0; i<20; i++){
		if( i < (num/5) )
			fprintf(out, "<td width=4%% bgcolor=\"3A48FF\">&nbsp;</td>");
		else
			fprintf(out, "<td width=4%% bgcolor=\"AEAEAE\">&nbsp;</td>");
	}
	#ifdef NOVAC
	    fprintf(out,"</tr><tr><td width=4%%>0%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>25%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>50%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>75%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>100%%</td></tr></table>");
    #else
	fprintf(out,"</tr><tr><td width=4%%>0%%</td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%></td><td width=4%%>100%%</td></tr></table>");
    #endif

	fprintf(out,"<script language=\"javascript\">\nsettime();\nfunction settime(){setTimeout(\"reloadurl()\",5000);\nreturn true;}\nfunction reloadurl(){location.replace('/Set_usbhdformat_m.cgi?percent=%d');}</script><br></body></html>\n", num);

	return ;
}


void Set_usbhdformat(FILE *out){

	char usb_folder[128], LineBuf[256], usb_share[128],data_backup[60];
	char *end1=NULL,*end2;
	char temp[50];
	FILE *fp1,*fp2;

	printf("%s: data:%s\n",__func__,arg);
	strcpy(data_backup,arg);

	ARGUMENT args[]={
	{"usb_folder", usb_folder, 128},
	{"usb_share", usb_share, 128},
	{NULL,NULL,0}
    };
    parseargs(args);

	printf("usb_folder=%s,usb_share=%s\n",usb_folder,usb_share);

   if(strlen(usb_folder) != 0 ){

		if( (fp1=fopen("/tmp/usb.log","r")) != NULL ){
			while(fgets(LineBuf,255,fp1)!=NULL){
				if(strstr(LineBuf,usb_share)){
					end1=strrchr(LineBuf,':');
					end2=end1+5;
					end1=end1+6;
					*end1='\0';
					break;
				}
			}
			fclose(fp1);
		}

		sprintf(temp,"/etc/rc.d/usb_pnp%s 0",end2); // umount usbdisk
		printf("temp = %s\n",temp);
		system(temp);

		end1=usb_share+8;
		*end1='\0';

		printf("before call Set_usbhdformat_m , data_backup=%s\n",data_backup);
		Set_usbhdformat_m( data_backup, out );
		sprintf(temp, "/etc/rc.d/mkEXT3usb.sh %s &",usb_share );
		printf("temp = %s\n",temp);
		my_system(out, temp );
	    //boot_page(out,"Show_advanced_func1.cgi");
	    return;

	}
	else{
		// else of if(strlen(usb_folder) != 0 ){

		end1=usb_share+8;
		*end1='\0';

		printf("before call Set_usbhdformat_m , data_backup=%s\n",data_backup);
		Set_usbhdformat_m( data_backup, out );
		sprintf(temp, "/etc/rc.d/mkEXT3usb.sh %s &", usb_share );
		printf("temp = %s\n",temp);
		my_system(out, temp );
	    //boot_page(out,"Show_advanced_func1.cgi");
	    return;

	}
/*
#ifdef EXT3
	my_system(out, "/etc/rc.d/mkEXT3disk.sh /dev/hda &");
#else
	my_system("/etc/rc.d/mkFAT32disk.sh /dev/hda &");
#endif
*/
}


void Show_advanced_func1(FILE *out){

		fprintf(out,"<HTML><HEAD><TITLE>Gigabit SOHO NAS</TITLE>\n"
			    "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n"
			    "<META content=\"MSHTML 6.00.2600.0\" name=GENERATOR></HEAD>\n"
			    "<script language=\"JavaScript\">\n"
			    "function double_check()\n"
			    "{\n");
	if(strlen(usb[0].hddevice) == 0 && strlen(usb[1].hddevice) == 0 ){
			fprintf(out,"		     un1 = document.USB_advanced.umount1.value;\n"
			"		     un2 = document.USB_advanced.umount2.value;\n"
			"		     if(un1 == 1 && un2 == 0){ \n"
			"		     alert(\"It can't format usb device1 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		     if(un1 == 0 && un2 == 1){ \n"
			"		     alert(\"It can't format usb device2 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
		        "		flag=confirm(\"Confirm: All USB drive data will be erased. Are you sure you want to format the hard drive?\");\n"
		        "		if(flag)\n"
		        "		{ \n"
		        "		flag2=confirm(\"Double confirm: All USB drive data will be erased. Are you sure you want to format the hard drive?\"); \n"
		        "		if(flag2)\n"
		        "		{return true;}\n"
		        "		else{return false;}\n"
		        "		}\n"
		        "		else {return false;};\n");
	}
	else{
			fprintf(out,"		     un1 = document.USB_advanced.umount1.value;\n"
			"		     un2 = document.USB_advanced.umount2.value;\n"
			"		     un3 = document.USB_advanced.deviceformat.value;\n"
			"		     if(un1 == 1 && un2 == 0 && un3 == 1){ \n"
			"		     alert(\"It can't format usb device1 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		     if(un1 == 0 && un2 == 1 && un3 == 2){ \n"
			"		     alert(\"It can't format usb device2 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		     if(un1 == 1 && un2 == 0 && un3 == 3){ \n"
			"		     alert(\"It can't format usb device1 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		     if(un1 == 0 && un2 == 1 && un3 == 3){ \n"
			"		     alert(\"It can't format usb device2 before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		     if(un1 == 1 && un2 == 1){ \n"
			"		     alert(\"It can't format both usb devices before umount!\");\n"
			"		     return false;\n"
			"		     }\n"
			"		flag=confirm(\"Confirm: All USB drive data will be erased. Are you sure you want to format the USB hard drive?\");\n"
			"		if(flag)\n"
			"		{ \n"
			"		flag2=confirm(\"Double confirm: All USB drive data will be erased. Are you sure you want to format the USB hard drive?\"); \n"
			"		if(flag2)\n"
			"		{return true;}\n"
			"		else{return false;}\n"
			"		}\n"
			"		else {return false;};\n");
	}
		fprintf(out,"};\n"
			    "</script>\n"
			    "<BODY text=#000000 bgColor=#ffffff>\n"
			    "<P><FONT class=style9>&nbsp;&nbsp;Advanced Setting</FONT></P>\n"
			    "<TABLE cellSpacing=0 cellPadding=0 width=650 border=0>\n"
			    "<TBODY>\n"
			    "<TR>\n"
			    "<TD width=\"100%%\" bgColor=#ffffff height=31>\n"
			    "<DIV align=left>&nbsp;&nbsp;<IMG src=\"images/usb_disk.gif\" useMap=#Map border=0></DIV></TD></TR>\n"
			    "<TR height=5>\n"
			    "<TD></TD></TR></TBODY></TABLE>\n"
			    "<TABLE cellSpacing=0 cellPadding=0 width=552 border=0>\n"
			    "<TBODY>\n"
			    "<TR bgColor=#ffffff>\n"
			    "<TD width=2>&nbsp;</TD>\n"
			    "<TD width=550>\n"
			    "<TABLE borderColor=#ccccdd cellSpacing=0 cellPadding=0 width=550 \n"
			    "border=1><TBODY>\n"
			    "<TR bgColor=#ff0000>\n"
			    "<TD class=style2 colSpan=2 height=30>&nbsp;&nbsp;Advanced Setting</TD></TR>\n"
			    "<FORM name=\"USB_advanced\" action=\"Set_usbhdformat.cgi\" method=\"get\" onsubmit=\"return double_check()\">\n");
		#ifdef DEBUG
			printf("usb[0].umount=%s\n",usb[0].umount);
			printf("usb[1].umount=%s\n",usb[1].umount);
    #endif
	if(strstr(usb[0].umount,"1") && strstr(usb[1].umount,"1")){
    fprintf(out,"<input type=\"hidden\" name=\"umount1\" value=\"1\">\n");
		fprintf(out,"<input type=\"hidden\" name=\"umount2\" value=\"1\">\n");
	}
	else if(strstr(usb[0].umount,"1") && strstr(usb[1].umount,"0")){
		fprintf(out,"<input type=\"hidden\" name=\"umount1\" value=\"1\">\n");
		fprintf(out,"<input type=\"hidden\" name=\"umount2\" value=\"0\">\n");
	}
	else if(strstr(usb[0].umount,"0") && strstr(usb[1].umount,"1")){
		fprintf(out,"<input type=\"hidden\" name=\"umount1\" value=\"0\">\n");
		fprintf(out,"<input type=\"hidden\" name=\"umount2\" value=\"1\">\n");
	}
	else{
		fprintf(out,"<input type=\"hidden\" name=\"umount1\" value=\"0\">\n");
		fprintf(out,"<input type=\"hidden\" name=\"umount2\" value=\"0\">\n");
	}
		fprintf(out,"<TR>\n");
		fprintf(out,"<TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;Device Select</TH>\n");

	if(strlen(usb[0].hddevice) > 0 && strlen(usb[1].hddevice) > 0 ){
		fprintf(out,"<TD width=400>&nbsp;&nbsp;\n");
		fprintf(out,"<select name=\"deviceformat\" size=\"1\">\n");
		fprintf(out,"<option option value=\"1\">Device 1</option>\n");
		fprintf(out,"<option option value=\"2\">Device 2</option>\n");
		fprintf(out,"<option option value=\"3\">Both</option>\n");
		fprintf(out,"</select>\n");
	}
	else if((strlen(usb[0].hddevice) > 0 && strlen(usb[1].hddevice) == 0) || (strlen(usb[0].hddevice) == 0 && strlen(usb[1].hddevice) > 0 )){
	  fprintf(out,"<TD width=400>&nbsp;&nbsp;\n");
	  fprintf(out,"<select name=\"deviceformat\" size=\"1\">\n");
	  fprintf(out,"<option option value=\"1\">Device 1</option>\n");
	  fprintf(out,"</select>\n");
	}
	else{
	  fprintf(out,"<TD colspan=4 align=center class=style6>\n");
	  fprintf(out,"No device could be used!!\n");
	}
		fprintf(out,"</TD></TR>\n");
		fprintf(out,"<TR>\n");
		fprintf(out,"<TH align=left width=150 bgColor=#dddddd>&nbsp;&nbsp;USB HD Format</TH>\n");
		fprintf(out,"<TD width=400>&nbsp;&nbsp;\n");
		if(strlen(usb[0].hddevice) == 0 && strlen(usb[1].hddevice) == 0 )
		  fprintf(out,"<input type=\"submit\" name=\"reset\" value=\"Yes\" disabled>&nbsp;&nbsp;<input type=\"button\" name=\"reset\" value=\"No\" onclick='history.back();'>\n");
		else
	    fprintf(out,"<input type=\"submit\" name=\"reset\" value=\"Yes\">&nbsp;&nbsp;<input type=\"button\" name=\"reset\" value=\"No\" onclick='history.back();'>\n");
		fprintf(out,"</TD></TR>\n");
		fprintf(out,"<TR bgColor=#ffffff>\n");
		fprintf(out,"<TH align=left width=150 bgColor=#ffffff>&nbsp;&nbsp;Note</TH>\n");
		fprintf(out,"<td width=400 class=\"style4\">If you focus on HD format item immediately, then hard disk will be format for a few minutes. When HD Format will be processed,\n");
		fprintf(out,"it no accepts any operating to Apply one again. The other item that Mount device is focused and key the \"Mount Name\" before click apply button to connnect \n");
		fprintf(out,"mount samba or USB device strictly.\n");
		fprintf(out,"</TD></TR>\n");
		fprintf(out,"</FORM></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>\n");
		fprintf(out,"<map name=\"Map\"> \n");
		fprintf(out,"<AREA shape=RECT coords=10,1,109,23 href=\"Show_USB_Detail.cgi\">\n");
		fprintf(out,"<AREA shape=RECT coords=110,1,209,23 href=\"Show_USB_Disk.cgi\">\n");
		fprintf(out,"<AREA shape=RECT coords=210,1,318,23 href=\"Show_USB_Printer.cgi\">\n");
		fprintf(out,"<AREA shape=RECT coords=321,1,512,23 href=\"usbque.htm\">\n");
		fprintf(out,"</MAP></BODY></HTML>\n");
}

void Set_usbcopy(FILE *out ){

	char buf[50];
	#ifdef DEBUG1
		printf("data=%s\n",data);
	#endif
	ARGUMENT args[]={
	{"mnt_dir",CONFIG.USB_COPY_FOLDER,MAX_NAME},
	{NULL,NULL,0}
    };
	parseargs(args);

	printf("usb_copy_dir=%s\n",CONFIG.USB_COPY_FOLDER);

	SaveConfiguration();
	boot_page(out,"Show_maintain.cgi");
}


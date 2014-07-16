/*all which is replaced into html or removed as not usable*/

#if defined(NH230)
void Show_maintain( FILE *out){

	FILE *fp;
	char LineBuf[256];

	  if((fp = fopen("/htdocs/maintain_nh230.htm","r")) == NULL){
	return;
    }
    else{
	while(fgets(LineBuf,255,fp) != NULL)
	      fprintf(out,LineBuf);
	fclose(fp);
    }
    system("/bin/dd if=/dev/mtd_linux_cfg of=/tmp/config.tgz bs=1k count=32 >/dev/null 2>&1");
    //system("/bin/dd if=/dev/mtd_linux_cfg of=/tmp/config.tgz bs=1k count=32");
    //printf("Now!! Update latest config.tgz finished from flash memory!!\n");
/*
fprintf(out,"<HTML><HEAD>\n");
fprintf(out,"<TITLE>Gigabit SOHO NAS</TITLE>\n");
fprintf(out,"<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/default.css\">\n");
fprintf(out,"<SCRIPT language=JavaScript>\n");

fprintf(out,"function double_check(){\n");
fprintf(out,"	flag=confirm(\"Confirm: Are you sure you want to execute factory default value setting?\");\n");
fprintf(out,"	if(flag)\n");
fprintf(out,"	{\n");
fprintf(out,"		flag2=confirm(\"double confirm: Are you sure you want to execute factory default value setting?\"); \n");
fprintf(out,"		if(flag2)		{return true;}\n");
fprintf(out,"		else			{return false;}\n");
fprintf(out,"	}\n");
fprintf(out,"	return false;\n");
fprintf(out,"}\n");

fprintf(out,"function check_form(){\n");
fprintf(out,"	un = document.form1.ufile.value;\n");
fprintf(out,"	if (check_un()==0){\n");
fprintf(out,"			alert(\"Please input the file path!!\");\n");
fprintf(out,"			document.form1.ufile.focus();			\n");
fprintf(out,"			return false;\n");
fprintf(out,"	} else	 self.open('updatepop.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");

fprintf(out,"}\n");

fprintf(out,"function check_un(){\n");
fprintf(out,"	var ok=1;\n");
fprintf(out,"	var count = document.form1.ufile.length;\n");
fprintf(out,"	if (un==null||un=="")		ok=0;\n");
fprintf(out,"	return ok;\n");
fprintf(out,"}\n");

fprintf(out,"function check_form2(){\n");
fprintf(out,"	un = document.form2.ufile.value;\n");
fprintf(out,"	if (check_un2()==0){\n");
fprintf(out,"			alert(\"Please input the file path!!\");\n");
fprintf(out,"			document.form2.ufile.focus();			\n");
fprintf(out,"			return false;\n");
fprintf(out,"	} else	 self.open('updatepop1.htm','OK','alwaysRaised,resizable,scrollbars,width=275,height=120');\n");

fprintf(out,"}\n");

fprintf(out,"function check_un2(){\n");
fprintf(out,"	var ok=1;\n");
fprintf(out,"	var count = document.form2.ufile.length;\n");
fprintf(out,"	var chk1=un.lastIndexOf(\"z\");\n");
fprintf(out,"	var chk2=un.lastIndexOf(\"g\");\n");
fprintf(out,"	var chk3=un.lastIndexOf(\"t\");\n");
fprintf(out,"	var chk4=un.lastIndexOf(\".\");\n");
fprintf(out,"	var chk5=un.lastIndexOf(\"x\");\n");
fprintf(out,"	var chk6=un.lastIndexOf(\"t\");\n");
fprintf(out,"	var chk7=un.lastIndexOf(\"n\");\n");

fprintf(out,"	if(chk1 >= 0 && chk2 >= 0 && chk3 >= 0 && chk4 >= 0 && chk5 >= 0 && chk6 >= 0 && chk7 >= 0) ok=1;\n");
fprintf(out,"	else ok=0;\n");

fprintf(out,"	return ok;\n");
fprintf(out,"}\n");

fprintf(out,"function check_form3(){\n");
fprintf(out,"	un = document.form3.ufile3.value;\n");
fprintf(out,"	if (check_un3()==0){\n");
fprintf(out,"			alert(\"Please input the file path!!\");\n");
fprintf(out,"			document.form3.ufile3.focus();			\n");
fprintf(out,"			return false;\n");
fprintf(out,"	} \n");
fprintf(out,"	else{ \n");
fprintf(out,"	      return true;\n");
fprintf(out,"	 }     \n");
fprintf(out,"}\n");

fprintf(out,"function check_un3(){\n");
fprintf(out,"	var ok=1;\n");
fprintf(out,"	var count = document.form3.ufile3.length;\n");
fprintf(out,"	var chk1=un.lastIndexOf(\"z\");\n");
fprintf(out,"	var chk2=un.lastIndexOf(\"g\");\n");
fprintf(out,"	var chk3=un.lastIndexOf(\"t\");\n");
fprintf(out,"	var chk4=un.lastIndexOf(\".\");\n");
fprintf(out,"	var chk5=un.lastIndexOf(\"g\");\n");
fprintf(out,"	var chk6=un.lastIndexOf(\"i\");\n");
fprintf(out,"	var chk7=un.lastIndexOf(\"f\");\n");
fprintf(out,"	var chk8=un.lastIndexOf(\"n\");\n");
fprintf(out,"	     var chk9=un.lastIndexOf(\"o\");	    \n");
fprintf(out,"	     var chk10=un.lastIndexOf(\"c\");\n");

fprintf(out,"	if(chk1 >= 0 && chk2 >= 0 && chk3 >= 0 && chk4 >= 0 && chk5 >= 0 && chk6 >= 0 && chk7 >= 0 && chk8 >= 0 && chk9 >= 0 && chk10 >= 0) ok=1;\n");
fprintf(out,"	else ok=0;\n");

fprintf(out,"	return ok;\n");
fprintf(out,"}\n");

fprintf(out,"</SCRIPT>\n");
fprintf(out,"<META content=\"MSHTML 6.00.2600.0\" name=GENERATOR></HEAD>\n");
fprintf(out,"<BODY text=#000000 bgColor=#ffffff>\n");
fprintf(out,"<P><FONT class=style9>&nbsp;&nbsp;Maintenance</FONT></P>\n");
fprintf(out,"<TABLE cellSpacing=0 cellPadding=0 width=580 border=0>\n");
fprintf(out,"  <TBODY>\n");

fprintf(out,"<!-- Config Upload or Download  -->\n");
fprintf(out,"<TR bgColor=#ffffff>\n");
fprintf(out,"<TD height=10>&nbsp;</TD>\n");
fprintf(out,"<TH colSpan=3 height=10>\n");
fprintf(out,"<HR noShade>\n");
fprintf(out,"</TH></TR>\n");
fprintf(out,"<TR bgColor=#ffffff>\n");
fprintf(out,"<form name=\"form3\" action=\"Set_Conf.cgi\" method=post encType=multipart/form-data>\n");
fprintf(out,"<TD height=25>&nbsp;</TD>\n");
fprintf(out,"<TD colSpan=2 height=25 class=\"style4\">Config Upload or Download</TD>\n");
fprintf(out,"<TD height=25>\n");
fprintf(out,"<INPUT type=\"submit\" value=\"Load\" name=config_upload onclick=\"return check_form3()\">\n");
fprintf(out,"<INPUT type=file size=25 name=ufile3 value=\"\"><input type=\"hidden\" name=\"dot\" value=\"-------\"> </TD></TR>\n");
fprintf(out,"<TR><TD colSpan=4>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
system("/bin/dd if=/dev/mtd_linux_cfg of=/tmp/config.tgz bs=1k count=32");
printf("Now!! Update latest config.tgz from flash memory!!\n");
fprintf(out,"<INPUT type=\"button\" value=\"Save\" name=config_download onclick=\"location.href='/tmp/config.tgz';\"></TD></TD></TR>\n");
fprintf(out,"<TR bgColor=#ffffff><TD height=10>&nbsp;</TD>\n");
fprintf(out,"<TH align=left colSpan=3 height=10>\n");
fprintf(out,"<HR noShade></TH></TR></form>\n");
fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD height=25>&nbsp;</TD>\n");
fprintf(out,"<TD colSpan=2 height=25 class=\"style4\">Please use the above method to upload or download configuration to PC.<BR>\n");
fprintf(out,"<br>Note: When the upload or download is completed, it will return to the \"Home\" page.</TD></TR>\n");
fprintf(out,"<!-------------------------------->\n");
*/
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <form name=\"form1\" action=\"Firmware_upgrade.cgi\" method=post encType=multipart/form-data>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Firmware Upgrade</TD>\n");
fprintf(out,"	 <TD height=25>\n");
fprintf(out,"	 <INPUT type=submit value=\"Update\" name=B_download onclick=\"return check_form()\">\n");
//fprintf(out,"	   <!--<INPUT type=submit value=\"Update\" name=B_download>-->\n");
fprintf(out,"	 <INPUT type=file size=25 name=ufile value=""><input type=\"hidden\" name=\"dot\" value=\"-------\"> </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH align=left colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Be sure that the version to upgrade is appropriate.<BR>Please wait and ensure a safe operating environment.<BR><BR>\n");
fprintf(out,"	 Note: When the reboot is completed, it will return to the \"Home\" page.</TD></TR>\n");
/*
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD width=5 height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TH width=60 height=25>&nbsp;</TH>\n");
fprintf(out,"	 <TD width=215 height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD width=300 height=25>&nbsp;</TD></TR>\n");
*/
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <form method=\"GET\" action=\"Restart_System.cgi\">\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Restart the System</TD>\n");
fprintf(out,"	 <input type=\"hidden\" name=\"page\" value=\"restart\">\n");
fprintf(out,"	 <TD align=left height=25><INPUT type=submit value=\"Start Restarting\" name=B_reboot>\n");
fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <form name=fac_def method=\"GET\" action=\"Factory_default.cgi\" onsubmit=\"return double_check()\">\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">It may take several minutes.<BR>Please wait and ensure a safe operating environment.<BR><BR>\n");
fprintf(out,"	 Note: The system returns to \"Home\" page after rebooting.</TD></TR>\n");
/*
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=3 height=25>&nbsp;</TD></TR>\n");
*/
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Reset to Factory Default</TD>\n");
fprintf(out,"	 <TD align=left height=25>\n");
fprintf(out,"	 <INPUT type=submit value=\"Start Reseting\" name=B_default_set>\n");
fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">\n");
fprintf(out,"	 It may take several minutes. Current configuration will be lost.<br>\n");
fprintf(out,"	 Please wait and ensure a safe operating environment.<br><br>\n");
fprintf(out,"	Note: When the reset is completed, it will return to the \"Home\" page.\n");
fprintf(out,"	 </TD></TR>\n");

fprintf(out,"<!-- Character Set File  -->\n");
fprintf(out," <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <form name=\"form2\" action=\"Set_codeset.cgi\" method=post encType=multipart/form-data>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Character Set& UPNP & Printer</TD>\n");
fprintf(out,"	 <TD height=25>\n");

    if(hdcapacity==0)
     fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=ch_download disabled onclick=\"return check_form2()\">\n");
    else
    fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=ch_download onclick=\"return check_form2()\">\n");
    fprintf(out,"<INPUT type=file size=25 name=ufile value=\"\"><input type=\"hidden\" name=\"dot\" value=\"-------\"> </TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=10>&nbsp;</TD>\n");
    fprintf(out,"<TH align=left colSpan=3 height=10>\n");
      fprintf(out,"<HR noShade></TH></TR></form>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD height=25>&nbsp;</TD>\n");
    fprintf(out,"<TD colSpan=2 height=25 class=\"style4\">Please browse the \"ntx_libra.tgz\" file in the include CD. \n");
    fprintf(out,"The UPnP & Printer server function will be activated only after this file being appiled.<BR><BR>\n");
    //fprintf(out,"Note: When the character set is completed, it will return to the \"Home\" page.</TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD colSpan=3 height=25>&nbsp;</TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD colSpan=3 height=25>&nbsp;</TD></TR></TBODY></TABLE>\n");
fprintf(out,"<script language=\"JavaScript\"></script></BODY></HTML>\n");
}


#elif defined(NH221)


void Show_maintain( FILE *out){	//this sheet is moved in html: Show_maintain.htm

	FILE *fp;
	char LineBuf[256];

// if((fp = fopen("./maintain_nh221.htm","r")) == NULL){
	  if((fp = fopen("./Waitpage_h.1.htm","r")) == NULL){
	return;
    }
    else{
	while(fgets(LineBuf,255,fp) != NULL)
	      fprintf(out,LineBuf);
	fclose(fp);
    }
//    system("/bin/dd if=/dev/mtd3 of=/tmp/config.tgz bs=1k count=32 >/dev/null 2>&1");		bullsheet RAW
    show_directory("web_dir");// to make /var/run/usb.js for usb copy button default folder

/*
fprintf(out,"<META content=\"MSHTML 6.00.2600.0\" name=GENERATOR></HEAD>\n");
fprintf(out,"<BODY text=#000000 bgColor=#ffffff>\n");
fprintf(out,"<P><FONT class=style9>&nbsp;&nbsp;Maintenance</FONT></P>\n");
fprintf(out,"<TABLE cellSpacing=0 cellPadding=0 width=580 border=0>\n");
fprintf(out,"  <TBODY>\n");
*/
fprintf(out,"<!--   CODE PAGE	 -->\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
//fprintf(out,"	   <TD width=5 height=25>&nbsp;</TD>\n");
//fprintf(out,"	   <TH width=60 height=25>&nbsp;</TH>\n");
//fprintf(out,"	   <TD width=215 height=25>&nbsp;</TD>\n");
//fprintf(out,"	   <TD width=300 height=25>&nbsp;</TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <form method=\"GET\" action=\"Set_codeset1.cgi\">\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Code Page</TD>\n");
//fprintf(out,"	   <!--<input type=\"hidden\" name=\"page\" value=\"restart\">-->\n");
fprintf(out,"	 <TD align=left height=25>\n");
fprintf(out,"	 <select name=\"code_page\" size=\"1\">\n");
#ifdef DEBUG1
printf("Show_maintain;CONFIG.CODE_SET=%s\n",CONFIG.CODE_SET);
#endif
if(!strcmp(CONFIG.CODE_SET,"437-8859-1")){
fprintf(out,"	 <option value=\"437-8859-1\" selected>437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"737-8859-7")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\" selected>737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"775-8859-15")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\" selected>775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"775-8859-13")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\" selected>775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"850-8859-1")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\" selected>850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"850-8859-15")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\" selected>850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"1252-8859-15")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\" selected>1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"852-8859-2")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\" selected>852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"857-8859-9")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\" selected>857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"866-8859-5")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\" selected>866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"866-KOI8-R")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\" selected>866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
#ifdef NH221
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
#endif
}
else if(!strcmp(CONFIG.CODE_SET,"866-1251")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\" selected>866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
}
else if(!strcmp(CONFIG.CODE_SET,"932-sjis-")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\" selected>932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
}
#ifdef NH221
else if(!strcmp(CONFIG.CODE_SET,"936-8859-15-")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\" >866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\" selected>936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
}
else if(!strcmp(CONFIG.CODE_SET,"949-8859-15-")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\" selected>949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
}
else if(!strcmp(CONFIG.CODE_SET,"950-8859-15-")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\" selected>950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\">874 (Thai)</option>\n");
}
else if(!strcmp(CONFIG.CODE_SET,"874-8859-11-")){
fprintf(out,"	 <option value=\"437-8859-1\">437 (ISO 8859-1 United States, Canada)\n");
fprintf(out,"		<option value=\"737-8859-7\">737 (ISO 8859-7 Greek)\n");
fprintf(out,"		<option value=\"775-8859-15\">775 (ISO 8859-15 Estonian)\n");
fprintf(out,"		<option value=\"775-8859-13\">775 (ISO 8859-13 Baltic Rim)\n");
fprintf(out,"		<option value=\"850-8859-1\">850 (ISO 8859-1 Latin1, Western and West European)\n");
fprintf(out,"		<option value=\"850-8859-15\">850 (ISO 8859-15 Euro)\n");
fprintf(out,"		<option value=\"1252-8859-15\">1252 (ISO 8859-15 German)\n");
fprintf(out,"		<option value=\"852-8859-2\">852 (ISO 8859-2 Central/Eastern European)\n");
fprintf(out,"		<option value=\"857-8859-9\">857 (ISO 8859-9 Turkish)\n");
fprintf(out,"		<option value=\"866-8859-5\">866 (ISO 8859-5 Russian)\n");
fprintf(out,"		<option value=\"866-KOI8-R\">866 (KOI8 Russian)\n");
fprintf(out,"		<option value=\"866-1251\">866 (1251 Bulgarian, Belarussian)\n");
fprintf(out,"		<option value=\"932-sjis-\">932 (Shift-JIS Japanese)\n");
fprintf(out,"		<option value=\"936-8859-15-\">936 (Simplified Chinese GBK)</option>\n");
fprintf(out,"		<option value=\"949-8859-15-\">949 (Korean)</option>\n");
fprintf(out,"		<option value=\"950-8859-15-\">950 (Tranditional Chinese Big5)</option>\n");
fprintf(out,"		<option value=\"874-8859-11-\" selected>874 (Thai)</option>\n");
}
#endif
fprintf(out,"	 </select>\n");
fprintf(out,"	 <INPUT type=submit value=\"Apply\" name=codepage_apply>\n");

fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">To draw the option item with code number that instead of the ISO sequence code and country.<BR>Please wait and ensure a  safe operating environment.<BR><BR>\n");
fprintf(out,"	 Note: When the reset is completed, it will return to the \"Home\" page.</TD></TR>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <form name=\"form1\" action=\"Firmware_upgrade.cgi\" method=post encType=multipart/form-data>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Firmware Upgrade</TD>\n");
fprintf(out,"	 <TD height=25>\n");
fprintf(out,"	 <INPUT type=submit value=\"Update\" name=B_download onclick=\"return check_form()\">\n");
//fprintf(out,"	   <!--<INPUT type=submit value=\"Update\" name=B_download>-->\n");
fprintf(out,"	 <INPUT type=file size=25 name=ufile value=""><input type=\"hidden\" name=\"dot\" value=\"-------\"> </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH align=left colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Be sure that the version to upgrade is appropriate.<BR>Please wait and ensure a safe operating environment.<BR><BR>\n");
fprintf(out,"	 Note: When the reboot is completed, it will return to the \"Home\" page.</TD></TR>\n");
/*
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD width=5 height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TH width=60 height=25>&nbsp;</TH>\n");
fprintf(out,"	 <TD width=215 height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD width=300 height=25>&nbsp;</TD></TR>\n");
*/
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <form method=\"GET\" action=\"Restart_System.cgi\">\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Restart the System</TD>\n");
fprintf(out,"	 <input type=\"hidden\" name=\"page\" value=\"restart\">\n");
fprintf(out,"	 <TD align=left height=25><INPUT type=submit value=\"Start Restarting\" name=B_reboot>\n");
fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <form name=fac_def method=\"GET\" action=\"Factory_default.cgi\" onsubmit=\"return double_check()\">\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">It may take several minutes.<BR>Please wait and ensure a safe operating environment.<BR><BR>\n");
fprintf(out,"	 Note: When the reboot is completed, it will return to the \"Home\" page.</TD></TR>\n");
/*
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=3 height=25>&nbsp;</TD></TR>\n");
*/
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Reset to Factory Default</TD>\n");
fprintf(out,"	 <TD align=left height=25>\n");
fprintf(out,"	 <INPUT type=submit value=\"Start Reseting\" name=B_default_set>\n");
fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">\n");
fprintf(out,"	 It may take several minutes. Current configuration will be lost.<br>\n");
fprintf(out,"	 Please wait and ensure a safe operating environment.<br><br>\n");
fprintf(out,"	Note: When the reset is completed, it will return to the \"Home\" page.\n");
fprintf(out,"	 </TD></TR>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Syslog Infomation</TD>\n");
fprintf(out,"	 <TD align=left height=25>\n");
fprintf(out,"<input type=\"button\" value=\"Info\" onclick=\"location.replace('Show_syslog.cgi');\">");
fprintf(out,"	 </TD></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">\n");
fprintf(out,"	 It can display the USB device infomation when NAS will happen abnormally.\n");
fprintf(out,"	 </TD></TR>\n");

// andy 20050906 add the copy button default folder
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Copy Button default folder</TD>\n");
fprintf(out,"	 <TD align=left height=25>\n");
//fprintf(out,"<input type=\"button\" value=\"Info\" onclick=\"location.replace('Show_syslog.cgi');\">");
fprintf(out,"  <form name=\"usb_copy\" method=\"GET\" action=\"Set_usbcopy.cgi\">\n");
fprintf(out,"	 <input type=\"hidden\" name=\"usb_copy_dir\" value=\"%s\">\n",CONFIG.USB_COPY_FOLDER);
fprintf(out,"	 <script language=\"JavaScript\" src=\"usb.js\"></script>\n");

if(hdcapacity!=2)
	fprintf(out,"	 <INPUT type=submit value=\"Apply\" name=apply disabled >\n");
else
	fprintf(out,"	 <INPUT type=submit value=\"Apply\" name=apply>\n");

fprintf(out,"	  </TD></TR></form>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");

fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">\n");
fprintf(out,"	 When you press the copy button, the USB Disk data will copy to this folder.<br>\n");
fprintf(out,"	 ** Copy function is disabled when USB Disk is not present.\n");
fprintf(out,"	 </TD></TR>\n");
//end


fprintf(out,"<!-- Character Set File  -->\n");
fprintf(out," <TR bgColor=#ffffff>\n");
fprintf(out,"	 <TD height=10>&nbsp;</TD>\n");
fprintf(out,"	 <TH colSpan=3 height=10>\n");
fprintf(out,"	   <HR noShade>\n");
fprintf(out,"	 </TH></TR>\n");
fprintf(out,"  <TR bgColor=#ffffff>\n");
fprintf(out,"	 <form name=\"form2\" action=\"Set_codeset.cgi\" method=post encType=multipart/form-data>\n");
fprintf(out,"	 <TD height=25>&nbsp;</TD>\n");
fprintf(out,"	 <TD colSpan=2 height=25 class=\"style4\">Character Set& UPNP & Printer</TD>\n");
fprintf(out,"	 <TD height=25>\n");

    if(hdcapacity!=2)
     fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=ch_download disabled onclick=\"return check_form2()\">\n");
    else
    fprintf(out,"<INPUT type=\"submit\" value=\"Apply\" name=ch_download onclick=\"return check_form2()\">\n");
    fprintf(out,"<INPUT type=file size=25 name=ufile value=\"\"><input type=\"hidden\" name=\"dot\" value=\"-------\"> </TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=10>&nbsp;</TD>\n");
    fprintf(out,"<TH align=left colSpan=3 height=10>\n");
      fprintf(out,"<HR noShade></TH></TR></form>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD height=25>&nbsp;</TD>\n");
    fprintf(out,"<TD colSpan=2 height=25 class=\"style4\">Choose the arbitrary corresponsive file to perform character set.<BR>Please wait and ensure a safe operating environment.<BR><BR>\n");
    fprintf(out,"Note: When the character set is completed, it will return to the \"Home\" page.</TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD colSpan=3 height=25>&nbsp;</TD></TR>\n");
  fprintf(out,"<TR bgColor=#ffffff><TD height=25>&nbsp;</TD><TD colSpan=3 height=25>&nbsp;</TD></TR></TBODY></TABLE>\n");
fprintf(out,"<script language=\"JavaScript\"></script></BODY></HTML>\n");
}
#endif

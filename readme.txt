

HTML-part:
1. <!-- #include ... -->
par="size1:web_name1:name1:pattern1"	--collect all parameters
area="size1:web_name1"			--area(buffer) for wtiting, used by write_file, write_par. Will showed as par="" variable.
readcfg					--read all par="" from config file
				    (excluded, par="size1:web_name1::pattern1" with empty name1)
file="file1"				--include file1, if file1=something.inc - then it will be parsed as "HTML-Part"
	file="file.inc?par=..\"??_#parameter??"	//if not find - erease par=??_#parameter?? compliete
exec="command"				--execute some command, and print
cgi="CGI_name"				--execute some CGI
shell=""shell_script body""		--execute some shell_script and print (used "print"-function)
					    (don't use ??_#variable?? in shell_script - this is unsecure.
					    use ??_%variable?? or ??_&variable?? instead.)
					    for better security use remove_chars ""; or show_chars "";
write_par="par:value"			--write value to parameter. the same as (write_par  "var:value";) in cgi-script.
tbl_select="Table_Name"			--insert <select> in HTML-part   or IF
tbl_select="_Table_Name"		--insert simple list of elements in HTML-part

chtbl_stat="flag:Table_Name:parsing"	--change table_entries status	if flag=s (show), flag=n (not show), flag=t (toggle)
					    flag+1==: d - if dir exist, e - if dir not exist, f - if file exist 

2. case parameter
<!-- if="web_name1:value1/" -->		by value1 used function parsestr1(,value1)
Show for web_name1 = value1
<!-- if="web_name2:value2/" -->
Show for web_name2 = value2
    <!-- else -->
    Show if web_name2 is not value2
<!-- /if -->

3. internal variables
referer[128], short_referer, user_agent[128], ip[20], port[10], dns_name[128], etc_save[2]

4. show variables: ??variable??, ??_int.variable??, ??__env.variable??, 

    ??_int.variable??	-> internal variables (referer...)
	    referer		0
	    short_referer	0
	    user_agent		0,	not writeable
	    ip			20,	writeable
	    port		10
	    srv_ip		0	CONFIG.IP
	    srv_port		0	CONFIG.ADMIN_PORT
	    dns_name		0	used now
	    etc_save		2
	    buf			16384
	    file_name		0
    ??__env.variable??	-> enviroment variable
    ??_?file|expression?? -> in file this expression
    ??_#variable?? - show global command variable index.html?variable=5, writeable, used in Address line
    ??_##variable?? - show local command variable index.html?variable=5, writeable, used in HTML-files & scripts
			' '=%20, '='=%3D
    ??variable?? - show value[old] of variable
    ??_%variable?? - show new_value of variable
    ??_@variable?? - show variable of table with name "variable"
    ??_&variable?? - show fresh_value of variable (if changed=1 ->new_value, if changed=0 ->value[old])

5. Make(register) script:
    a: with MIME
<!-- CGI: Set_codeset.cgi
    //script body
END_CGI -->
    b: without MIME
<!-- CGI: _Set_codeset.cgi
    //script body
END_CGI -->


CGI script has:
print "print this text ??_&var?? {?condition?} [?prime_condition?] value=\"FFFFEF\" ?@-_Table@? ";	-print function
	?@-_Table@?	- output Table in list-form
	?@-Table@?	- output Table in <select>-form
	?@0var@?	- output 'var' formated (N-size-chars output), rest fill with ZEROs.

system "execute_this_cmd";
my_system "execute_this_cmd_and_show";
get_file "show_this_file"; or get_file "show_this_file" {}
				-if show_this_file=something.inc - then it will be parsed as "HTML-Part"
		"file.inc?par=??_#par1??"	-if not find - erease par=??_#par1?? complete,
						only writeable (!= 0) parameters will be linked!

save_file "_#par:file_name";	-used by post-web_encoded transfer
savecfg;			-save all parameters, which have changed=1 and saved=0 (all 'saved' flags will be 1)
					and set etc_save=1 automaticly
init;				-read internal parameters (admin_port,ip,admin_name,admin_passwd)
fill_all_cfg;			-fill all parameters, which are registered as par="size::" .
					If parameter is chenged - so changed=1 will set!
					---used global variables  index.cgi?variable=5----
fill_cfg "par1:par2:par3:par4";	-fill only that parameters, but they are must be registered as par="..." as well.
					and if pattern exist - when match that pattern.
					If parameter is chenged - so changed=1 will set!
					---used global variables  index.cgi?variable=5----
get_var "show_variable";	-the same as ??show_variable?? in html-part of file
boot_page "admin-reset.htm?var=??_%var??"	-script, which is used to reload this page (used print-function)
save_bfile "name:size_limit:format:file_name";	-used by post-bounded transfer
save_bfile "name:0:/[/*.torrent/:/path/to/*";	-used by post-bounded transfer, add to path a real file_name of upload_file
set "a=10";			-set a to 10 (each cgi-script has a,b,c, and d variables)
    set "a := _#val";		-assign digits from val to a
if_set "a==10" {}		-exec in braces if a == 10 (each cgi-script has a,b,c, and d variables) [now the same as set. 
				    diff is braces and comparision chars]
change_line "/etc/file:nameserver: nameserver ??var??"; or change_line "/etc/file:/*/?var?/:: nameserver ??_%var??"";
				-find nameserver line in /etc/file and change it to nameserver ??var??
if_changed "var" {}		-exec in braces if changed-flag for "var" is 1
not_changed "var" {}		-exec in braces if changed-flag for "var" is 0
if "_%var:0/" {}			-exec if exist and in braces if _%var(new_variable) is 0. Same as <!-- if="_%var:0" -->
not "_%var:0/" {}		-exec if exist and in braces if _%var(new_variable) is not 0
nnot "_%var:0/" {}		-exec if not exist or in braces if _%var(new_variable) is not 0
my_shell "mkdir ??_#var??";	-the same as shell="mkdir ??_&var??" for html-part (used print-function)
shell "mkdir ??_#var??"		-the same as my_shell, but without text-output

	/* write to variables */
clean_ip;			-set check_ip[0]=\0, release ip
do{} buf_if_eof;		- if not found in buffer
buf_parse_area "/[/*/]\n\n" {set "a=1";}	-parse string, set a to 1, if matched and return pointer to print ""; function.
write_file  "var:file_name";	-write file to parameter var.
write_par  "var:value";		-write value to parameter var. for example: "_etc_save:0" (used if /etc must be saved)
				value="some string\n\n ??var1??\t-this is a var1"

bind_par  "var";		-bind pointer to buf_parse_area and print functions to parameter var. If not found or non 
				writeable bind to NULL pointer.
write_system  "par:cmd";
cat_system  "par:cmd";		-execute cmd, write or cat info to parameter
***********************
remove_chars "chars::_#par1:_#par2:..";	remove all chars from par1, par2 ...Chars are:  \";<>|\\ \n	and so on.
show_chars "chars::_#par1:_#par2:..";	show all chars. invertion of remove_chars
***********************
load_file "show_this_file_from_parm"; or load_file "show_this_file_from_parm" {}
exit_cgi;					-exit the cgi without rest execution
chroot ""; (to default) or "/path/to/files/"; or chroot "" {}	-on success in braces execution
exist_file "_#par"{}				-jump if file not exist
chtbl_stat "flag:Table_Name:parsing";		-change table status: flag = [s:n:t:](one char or two chars
							(d-dir, e-dir not exist, f-file)),
						Table_Name=name_FromTableEntry, parsing=used parsestr1_()
write_ppar "to_par:from_par:parsestr";		-get form parameter something to parameter (via parsestr1)


6. Table entry
<!-- TABLE: Table_Name
//table body
END_TABLE -->
//Table_Name != ":\n

in Table Body can be:
#-> get_folder: /mnt

or :
#-> parse_file: /etc/fstab
while:/[/*/]\n
if:

or for existing:
#-> check_folders
/mnt/hd
/mnt/hd1
#-> end

or for NOT-existing:
#-> check_Nfolders
/mnt/hd
/mnt/hd1
#-> end

or for existing files:
#-> check_files
/mnt/README
/mnt/hd1
#-> end


need: 
#data
#fdisk
#ps
#df
#netstat
#tail
#ifconfig
#route
#mount
#ls
#echo
#mke2fs, grep, cut, expr, swapon, swapoff, fdisk
#top -n 1 -b
#cat	-not used yet. but usefull!
#pwd
#cryptpw	in	access-user.htm
#md5sum in admin-firmware


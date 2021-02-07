<!-- "path" is exist "path1", "path_org", "i", "file" are in use.  if "file" exist - runs _get_file.cgi -->

<!-- if="path:" -->
<!--#include write_par="path_origin:??path??" exchange="get_dir.cgi" exchange="_get_file.cgi" -->
<!-- CGI: get_dir.cgi
    get_file "./JavaReturn_.inc?name=Home%20Page&i=??i??";
    if "_#path_tbl_0:"{
	not "_@path_tbl:.." {
	write_par "path1:??path??";
	write_par "path:??path1??/??_@path_tbl??";
	}
	if "_@path_tbl:.." {
	write_ppar "path:path:/[/l/l/--/]";
	}
    }
    print "</table><table><tr><td>Path: ??path??</td></tr>";
if "path:/?path_origin/?/" {
    print "
?TABLE: path_tbl
#-> get_ofolder: ??path??
END_TABLE?
";
}
if "path:/?path_origin/?/!/"{
    print "
?TABLE: path_tbl
..
#-> get_ofolder: ??path??
END_TABLE?
";
}
//print "?table?:path_tbl:path_tbl::<tr><td><a href=\"get_dir.cgi?path_tbl_0=?#+\">?@@cd ??path?? 2>/dev/null; [ $? -eq 0 ] && ls -l \"?$\" @@?</a></td></tr>\n
print "?table?:path_tbl:path_tbl::<tr><td><a href=\"get_dir.cgi?path_tbl_0=?#+\">?$</a></td></tr>\n
</table>";
    print "
?TABLE: file_tbl
#-> get_ofiles: ??path??
END_TABLE?
<hr><table><tr><td>Files:</td></tr>";
if "file:" { print "?table?:file_tbl:file_tbl::<tr><td><a href=\"_get_file.cgi?file_tbl_0=?#+\">?$</a></td></tr>\n";}
nnot "file:" { print "?table?:file_tbl::<tr><td>?@@cd ??path?? 2>/dev/null; [ $? -eq 0 ] && ls -l \"?$\" 2>/dev/null @@?</td></tr>\n";}

print "</table><hr><tr><td><a href=\"??back??\">Back</a></td></tr></table></body></html>";
END_CGI -->
<!--#include cgi="get_dir.cgi" -->
<!-- if="file:" -->
<!-- CGI: _get_file.cgi
	nnot "_@file_tbl:/\ "{
	    print "HTTP/1.0 200 OK\nContent-type: text/html\nServer: thttpd/2.25b\nCache-Control: no-cache,no-store\nExpires: 0\n\n";
	    get_file "./JavaReturn_.inc?name=Home%20Page&i=??i??";
	    print "<pre>False folder ID: ??_#file_tbl_0??</pre></td></tr></table><a href=\"??back??\">Back</a></table></body></html>";
	    exit_cgi ;
	}

	if "_#file_tbl_0:"{
//	print "HTTP/1.0 200 OK\nContent-type: application/octet-stream\n Server: thttpd/2.25b\nExpires: 0\n\n";
	print "HTTP/1.0 200 OK\nContent-Disposition: attachment; filename=";
	print "??_@file_tbl??\n";
	print "Server: thttpd/2.25b\nExpires: 0\n\n";

	get_ffile "??path??/??_@file_tbl??";
	exit_cgi;
	}
END_CGI -->
<!-- /if -->
<!-- else -->
<!--#include file="head.inc?name=401%20Unauthorized" 
file="side.inc?i=??i??" -->
<table id="maintab" height=290 align="center"><tr>
<td align="center" valign="top"><br>
<table id="header" align="center"><tr><td width="602">401 Unauthorized</td></tr></table>
<br>
Access to this resource is denied
</td></tr><table>
</body></html>
<!-- /if -->

/*remove config.h.old, include/httpd_config.h.old*/

/*NAMEof parameter,	WEB_NAME in html pages,		SIZE of aray CONFIG.#NAME[size]*/

cfg_line(WEB_ROOT,	,256)
cfg_line(USERNAME,	supername, 20)			//   6. SuperUser name
cfg_line(PASSWORD,	,20)///*superpasswd*/, 20)			//   7. SuperUser Password
cfg_line(ADMIN_PORT,	admin_port, 6)			//  60. Admin Port Number

cfg_line(IP_SECURITY_MODE,ip_security_mode, 2)	//  26. Security Mode:0->Disable, 1->Allow, 2->Not Allow
cfg_line1(LEGAL_IP,	ip1,0,	20)			//  16. Legal IP1	!!!!must be changed- /etc/config must begin from LEGAL_IP0 .. _IP9!!!!
cfg_line1(LEGAL_IP,	ip2,1,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip3,2,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip4,3,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip5,4,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip6,5,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip7,6,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip8,7,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip9,8,	20)			//  16. Legal IP
cfg_line1(LEGAL_IP,	ip10,9,	20)			//  16. Legal IP
cfg_line(MAC_SECURITY_MODE,mac_security_mode, 2)	//  38. Security Mode:0->Disable, 1->Allow, 2->Not Allow
cfg_line1(LEGAL_MAC,	mac1,0, 30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac2,1,	30)			//  28. Legal MAC1	!!!!must be changed- the same for that!!!!
cfg_line1(LEGAL_MAC,	mac3,2,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac4,3,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac5,4,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac6,5,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac7,6,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac8,7,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac9,8,	30)			//  28. Legal MAC1
cfg_line1(LEGAL_MAC,	mac10,9,30)			//  28. Legal MAC1

cfg_line(DNS_PARSE,	, 256)			//dns name parsing


cfg_line(IP,		ip,	20)				//   0. WAN IP Address
cfg_line(NETMASK,	netmask,	20)			//   1. WAN Netmask
cfg_line(GATEWAY,	gateway,	20)			//   2. WAN Netmask
cfg_line(DNS_SERVER,	dns_server,	20)			//  43. DNS Server
cfg_line(HOSTNAME,	hostname,	17)			//   3. LAN Host Name
/*   cfg_line(WORKGROUP,	host_group, 17)			//   4. LAN Work Group
    cfg_line(HOSTNAME_FULL,hostname_full, 51)	//   5. LAN Host Name description
*/
    cfg_line(FTP_ENABLE,ftp_enable, 2)		//  14. FTP Enable/Disable
    cfg_line(FTP_PORT,ftp_port, 6)		//  11. FTP Port Number
    cfg_line(FTP_MAX,ftp_unit, 3)			//  10. FTP Maximum Unit
    cfg_line(FTP_TIMEOUT,ftp_time, 4)		//  12. FTP Timeout
    cfg_line(FTP_ANONYMITY,anonymity, 2)	//  13. FTP Anonymity


    cfg_line(DBEAR_PORT,dbear_port,	 6)	//new
    cfg_line(DBEAR_ENABLE,dbear_enable,	 2)	//new

    cfg_line(DHCP_ENABLE,dhcp_client, 2)		//  15. DHCP Client

    cfg_line(TIME_ZONE,	timezone, 25)		//  40. Time Zone	!!!IS IT NOT 'TZ'-?!!!
    cfg_line(TZ,	tz,		10)			//  44. Time Difference
    cfg_line(TIME_MODE,time_mode,	2)		//  41. Time Mode
    cfg_line(NTP_SERVER,ntp_server,	40)
    cfg_line(TIME_SERVER,time_server,	40)		//  42. Time Server
    cfg_line(TIME_VALUE,,		30)///*"current_time"*/, 30)	//not in original parse.c file and "current_time" must not be hier RAW

    cfg_line(NFS_ENABLE,,		2)	//new
    cfg_line(NMB_ENABLE,nmb_enable,	2)	//new

    cfg_line(CODE_SET,	code_set,	20)		//  45. Code Set
    cfg_line(TCONFIG_FOLDER,torrent_dir, 17)	//  46. Folder for Torrent Config
    cfg_line(TCONFIG_FILENAME,tconfig_name, 17)	//  47. Torrent Config Name
    cfg_line(TORRENT_USER,max_user,	3)		//  48. Max active torrent number
    cfg_line(TORRENT_ENABLE,torrent_enable, 2)	//  49. Enable Torrent

    cfg_line(WEB_ENABLE,web_enable,	2)		//  50. WEB server enable
    cfg_line(WEB_PORT,web_port,		6)		//  51. WEB server port
    cfg_line(WEB_FOLDER,web_dir,	17)		//  52. Folder WEB server pages

    cfg_line(USB_SHARE,	usb_share,	2)		//  61. USB disk as share folder
    cfg_line(USB_RECYCLE,recycle, 	2)		//  62. USB recycle bin
//======allen adds to declare saving USB variable======
//  cfg_line("PRINTER_ENABLE","enable_pserver",	CONFIG.SERVER, 2)		//  63. Printer server radio option
  cfg_line(PRINTER_ENABLE,enable_pserver, 2)		//  63. Printer server radio option
  //char SERVER2[2];
//=====================================================
//======allen adds to declare saving Jumbo variable======
  cfg_line(JUMBO,	Jumbo_mode, 2)			//  64. Jumbo mode radio option
//=======================================================
//======allen adds to saving checksmb adjugement parameter======
  cfg_line(INITBOOT,, 2)
//=======================================================
//=== allen adds to save URL ===//
  cfg_line(URL,, 256)
//==============================//
// === Sleep mode === //
  cfg_line(SLEEP_MODE,, 2)
// ================== //
// === usb copy folder === //
 cfg_line(USB_COPY_FOLDER,usb_copy_dir, 17)	//  67. FTP and SAMBA number setting for service number
// ================== //

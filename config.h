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
cfg_line(HOSTNAME,	hostname,	17)			//   3. LAN Host Name
cfg_line(WEB_LOGIN,	web_login,	128)			//   used for login

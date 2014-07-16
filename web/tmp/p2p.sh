#!/bin/sh

echo "1" > /proc/sys/vm/overcommit_memory
rm /mnt/mldonkey/incoming/incoming

[ ! -d /mnt/mldonkey ] && {
	[ -f /mnt/mldonkey.tgz ] && tar zxf /mnt/mldonkey.tgz -C /mnt			
}
		
[ -f /mnt/mldonkey_html.tgz ] && tar zxf /mnt/mldonkey_html.tgz -C /mnt/mldonkey/html_themes/ntx/ && rm -f /mnt/mldonkey_html.tgz

[ ! -d /mnt/Share/p2p ] && {
	mkdir -p /mnt/Share/p2p
	mkdir -p /mnt/Share/p2p/directories
	mkdir -p /mnt/Share/p2p/files
	mkdir -p /mnt/Share/p2p/torrents
}

if [ -L /mnt/Share/p2p/files ] ; then
	rm -f /mnt/Share/p2p/files
	rm -f /mnt/Share/p2p/directories
	rm -f /mnt/Share/p2p/torrents
	mkdir -p /mnt/Share/p2p/directories
	mkdir -p /mnt/Share/p2p/files
	mkdir -p /mnt/Share/p2p/torrents			
fi

[ -n "$2" ] && [ -n "$3" ] && {
	sed -i "/TORRENT_USER/s/.*/TORRENT_USER=\'$2,$3\'/" /etc/config
}


[ "$1" = "start" ] && {
	GO=`grep TORRENT_ENABLE /etc/config | cut -d"'" -f2`
	PID=`pidof mlnet`

	[ -z "$PID" ] && [ "$GO" -eq "1" ] && {

			DOWNLOADRATE=`grep TORRENT_USER /etc/config | cut -d"'" -f2 | cut -d, -f1`
			UPLOADRATE=`grep TORRENT_USER /etc/config | cut -d"'" -f2 | cut -d, -f2`			
			sed -i "/max_hard_download_rate =/c max_hard_download_rate = $DOWNLOADRATE" /mnt/mldonkey/downloads.ini
			sed -i "/max_hard_upload_rate =/c max_hard_upload_rate = $UPLOADRATE" /mnt/mldonkey/downloads.ini
			sed -i "/max_hard_download_rate_2 =/c max_hard_download_rate_2 = $DOWNLOADRATE" /mnt/mldonkey/downloads.ini
			sed -i "/max_hard_upload_rate_2 =/c max_hard_upload_rate_2 = $UPLOADRATE" /mnt/mldonkey/downloads.ini			
			rm -f /mnt/mldonkey/torrents/downloads/*

			
			if [ -d /mnt/mldonkey ] ; then				
				if [ -z "`pidof mlnet`" ] ; then
					rm -f /mnt/mldonkey/*.tmp
					rm -f /mnt/mldonkey/*.pid
					MLDONKEY_CHROOT=/mnt/mldonkey mlnet -allowed_ips 255.255.255.255 -run_as_useruid 32767 > /mnt/chlog &
					[ ! -d /mnt/Share/p2p ] && {
						mkdir -p /mnt/Share/p2p
						mkdir -p /mnt/Share/p2p/directories
						mkdir -p /mnt/Share/p2p/files
						mkdir -p /mnt/Share/p2p/torrents
					}
				else
					wget "http://127.0.0.1:4080/submit?q=save" -o /tmp/1.log
					wget "http://127.0.0.1:4080/submit?q=kill" -o /tmp/1.log
					sleep 3
					MLDONKEY_CHROOT=/mnt/mldonkey mlnet -allowed_ips 255.255.255.255 -run_as_useruid 32767 > /mnt/chlog &
					[ ! -d /mnt/Share/p2p ] && {
						mkdir -p /mnt/Share/p2p
						mkdir -p /mnt/Share/p2p/directories
						mkdir -p /mnt/Share/p2p/files
						mkdir -p /mnt/Share/p2p/torrents
					}							
				fi				
			else
				if [ -f /mnt/mldonkey.tgz ] ; then
					tar zxf /mnt/mldonkey.tgz -C /mnt
					
					if [ -z "`pidof mlnet`" ] ; then
						rm -f /mnt/mldonkey/*.tmp
						rm -f /mnt/mldonkey/*.pid				
						MLDONKEY_CHROOT=/mnt/mldonkey mlnet -allowed_ips 255.255.255.255 -run_as_useruid 32767 > /mnt/chlog &
						[ ! -d /mnt/Share/p2p ] && {
							mkdir -p /mnt/Share/p2p
							mkdir -p /mnt/Share/p2p/directories
							mkdir -p /mnt/Share/p2p/files
							mkdir -p /mnt/Share/p2p/torrents	
						}
					else
						wget "http://127.0.0.1:4080/submit?q=save" -o /tmp/1.log
						wget "http://127.0.0.1:4080/submit?q=kill" -o /tmp/1.log
						sleep 3
						MLDONKEY_CHROOT=/mnt/mldonkey mlnet -allowed_ips 255.255.255.255 -run_as_useruid 32767 > /mnt/chlog &
						[ ! -d /mnt/Share/p2p ] && {
							mkdir -p /mnt/Share/p2p
							mkdir -p /mnt/Share/p2p/directories
							mkdir -p /mnt/Share/p2p/files
							mkdir -p /mnt/Share/p2p/torrents
						}														
				    fi
				fi
			fi		  
	}
	
	
} || {
	wget "http://127.0.0.1:4080/submit?q=save" -o /tmp/1.log
	wget "http://127.0.0.1:4080/submit?q=kill" -o /tmp/1.log

}
#!/bin/sh
#############################################
# made by alex_raw
# usage: $1 sd.. or hd..
# $2 = 1 -mount, 0 -umount
# $3 = index of port
# $4 = serialnumber

for i in `cat known_usb | grep -v "#"`
    do
	if [ -n "$4" ]; then
	S=`echo $i | cut -d ":" -f 1 | grep $4 `
	    if [ "$S" == "$4" ]; then
		P=`echo $i | cut -d ":" -f 2 `
		M=`echo $i | cut -d ":" -f 3 `
		C=`echo $i | cut -d ":" -f 4 `
		if [ -z "$M" ]; then
		    . /etc/config
		    M=$BACKUP_PATH/storage
		fi
#		echo "Path:$P   mount:$M   cmd:$C"
		if [ "$P" == "*" ]; then
#		    echo ALL
		    j=0
		    for k in /dev/$1*
		    do
			#not use /dev/sdb, use only /dev/sdb1...
			if [ $j != 0 ]; then
			    if [ $2 == 1 ]; then
#				echo "$k  $j"
				mkdir -p "$M-part$j"
				mount $k "$M-part$j"
				[ $? != 0 ] && rm -rf "$M-part$j"
			    elif [ $2 == 0 ]; then
				umount -f $k
				rm -rf "$M-part$j"
			    fi
			fi
			j=$(($j+1))
		    done
		    $C
		else
		    if [ -e /dev/$1$P ]; then
			if [ $2 == 1 ]; then
#			echo "/dev/$1$P"
			mkdir -p "$M"
			mount /dev/$1$P "$M"
			[ $? != 0 ] && rm -rf "$M"
			elif [ $2 == 0 ]; then
			umount -f /dev/$1$P
			rm -rf "$M"
			fi
		    else
			echo "device /dev/$1$P not found"
		    fi
		    $C
		fi
	    fi
	fi

done

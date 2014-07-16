#!/bin/sh
#Format disk with EXT3
#
#comment out by RAW

# 20040324 ansel: add umount and reboot
echo "First I umount."

#killall inetd
#killall smbd
#killall pnmd
#killall nmbd
#killall ftpd
#killall lpd
#killall twonkymedia
#/etc/init.d/upnp.sh stop
#/etc/init.d/umount_usb.sh /proc/mounts
#wget "http://127.0.0.1:4080/submit?q=save" -o /tmp/1.log
#wget "http://127.0.0.1:4080/submit?q=kill" -o /tmp/1.log
#/etc/rc.d/itunes.sh stop
#/etc/rc.d/upnpav.sh stop
sync;sync;sync;sync;
##umount /mnt

#scsi_ctl /dev/sda sleep 10101010
#scsi_ctl $1 sleep 10101010
#sleep 5

disk=$1"1"
swap_prtition=$1"2"

#swapoff $swap_prtition >/dev/null 2>&1


[ "$2" -eq "0" ] && {
# don't need swap partition

#echo ",,83" | sfdisk $1;
echo "== CREATE the EXT3 PARTATION OK !!!! =="
echo "== FORMAT the "$disk"!!!! =="
#mke2fs -j -m 0 $disk;
echo "== OK !! =="
} || {

## Fresh new disk must do this
#fdisk $1 <<EOF
#w
#
#EOF


CYLINDERS=`sfdisk -l $1 | grep "$1": | cut -d, -f1 | cut -d' ' -f3`
C_BYTES=`sfdisk -l $1 | grep Units | cut -d, -f1 | cut -d' ' -f5`
C_KBYTES=`expr $C_BYTES / 1024`
#128MB=131072k=134217728bytes
#512MB=524288
SWAP_NUM=`expr 524288 / $C_KBYTES + 1`
FIRST_PART=`expr $CYLINDERS - $SWAP_NUM - 1`

#sfdisk $1 <<EOF
#0,$FIRST_PART,83
#,,82
#EOF
#echo "$CYLINDERS , $C_KBYTES , $SWAP_NUM , $FIRST_PART"

echo "== CREATE the EXT3 and SWAP PARTATION OK !!!! =="
echo "== FORMAT the "$disk"!!!! =="
#mke2fs -j -m 0 $disk
echo "== OK !! =="

}


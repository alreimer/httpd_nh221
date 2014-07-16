#!/bin/sh
disk=$1"1"

# 20040324 ansel: add umount and reboot

echo "First I umount."

killall inetd
killall smbd
killall pnmd
killall nmbd
killall ftpd
sync;sync;sync;sync;
#umount /mnt

x=0
while [ "$x" -ne 10 ]
 do                                                                               
     if umount -f /mnt
        then
          break;
     fi                                                                                
     sleep 3
     x=$(($x+1))
 done

sleep 3

echo ",,b" | sfdisk -D $1;
echo "== CREATE the FAT32 PARTATION OK !!!! =="
echo "== FORMAT the "$disk"!!!! =="
mkdosfs -s 32 -F 32 $disk ;
echo "== OK !! =="

#echo "Ready to reboot."
#reboot

#end

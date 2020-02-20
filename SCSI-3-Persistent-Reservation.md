## SCSI-3 Persist Reservation Command on Linux

Note: Linux sg3_utils packages are required.

To query all registrant keys for given device

	#sg_persist -i -k -d /dev/sdd

To query all reservations for given device

	#sg_persist -i -r -d /dev/sdd

To register the new *reservation key* 0x123abc

	#sg_persist -o -G -S 123abc -d /dev/sdd

To clear all registrants

	#sg_persist -o -C -K 123abc -d /dev/sdd

To reserve

	#sg_persist -o -R -K 123abc -T 5 -d /dev/sdd

To release

	#sg_persist -o -L -K 123abc -T 5 -d /dev/sdd

Common used reservation Types:  
5 - Write Exclusive, registrants only  
6 - Exclusive Access, registrants only 


This is copy of [this page][1]

[1]: http://aliuhui.blogspot.jp/2012/04/scsi-3-persist-reservation-command-on.html

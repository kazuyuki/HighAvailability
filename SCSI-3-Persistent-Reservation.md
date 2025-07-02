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

---

## SCSI-3 PR を用いた HAクラスタ用 共有ディスク排他制御

### 防御ノード (現用系) の処理
```
defender　{
	While(1) {
		if (自身の reservation key が登録されている) {
			if (自身が reserve (Exclusive Access) している) {
				何もしない
			} else {
				reserve する
				if (reserve 失敗) {
					自殺
				}
			}
		} else {
			reservation key を登録する
			reserve する
			if (reserve 失敗) {
				challanger になる or 自殺
			}
		}
		sleep 3	(攻撃側が7秒毎に clear を行ってくるから)
	}
}
```

### 攻撃ノード (待機系) の処理
```
atacker {
	while (1) {
		reservation key を登録する
		clear する
		sleep 7
		reserve する
		if (reserve 失敗) {
			何もしない
		} else {
			sleep 10 (wsfc がここで 10秒の間を空けているらしい。理由が判らない)
			defender になる
		}
	}
}
```

PROUT コマンドの type 引数に 3 (exclusive access) を用いると意図した挙動が得られた。つまり、待機系の reserve 成功が 現用系に read/write 不能をもたらした。

参照:  
https://support.microsoft.com/ja-jp/help/309186/how-the-cluster-service-reserves-a-disk-and-brings-a-disk-online

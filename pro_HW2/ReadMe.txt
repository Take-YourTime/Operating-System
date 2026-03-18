有兩個資料夾（1、2），分別代表 第一題 和 第二題 會需要用到的檔案

第三題所需要用到的檔案 clock.c 已經放在上述兩個檔案中

以下內容中，#代表命令列

==========

登入minix

連結共享資料夾
# monut -t hgfs none /mnt


-----第一題-----

進入共享資料夾
# cd /mnt/（共享資料夾名稱）/1

將共享資料夾第一題的資料複製到kernel
# cp proc.c /usr/src/minix/kernel
# cp proc.h /usr/src/minix/kernel
# cp system.c /usr/src/minix/kernel

編譯kernel
# cd /usr/src/minix
# make clean
# cd /usr/src/minix/kernel
# make clean
# make kernel
# make install
# cp /usr/sbin/kernel /boot/minix_latest/kernel
# reboot

（重新啟動後，選擇2）

登入並查看當前性能
# top


-----第二題-----

進入共享資料夾
# cd /mnt/（共享資料夾名稱）/2

將共享資料夾第二題的資料複製到kernel，注意此資料夾中的proc.c檔與第一題的不相同，其餘兩者內容相同
# cp proc.c /usr/src/minix/kernel
# cp proc.h /usr/src/minix/kernel
# cp system.c /usr/src/minix/kernel


編譯kernel
# cd /usr/src/minix
# make clean
# cd /usr/src/minix/kernel
# make clean
# make kernel
# make install
# cp /usr/sbin/kernel /boot/minix_latest/kernel
# reboot

（重新啟動後，選擇2）

登入並查看當前性能
# top


-----第三題-----

先使用window建立遠端連線
>>windows命令>> ssh root@(IP address)


...使用第二題資料...

進入minix共享資料夾
# cd /mnt/共享資料夾名稱/1

將共享資料夾第一題的資料以及clock.c檔複製到kernel
# cp proc.c /usr/src/minix/kernel
# cp proc.h /usr/src/minix/kernel
# cp system.c /usr/src/minix/kernel
# cp clock.c /usr/src/minix/kernel

編譯kernel
# cd /usr/src/minix
# make clean
# cd /usr/src/minix/kernel
# make clean
# make kernel
# make install
# cp /usr/sbin/kernel /boot/minix_latest/kernel
# reboot

（重新啟動後，選擇2）

登入，此時螢幕每個一段時間會輸出上一段時間內每個優先級的process數量
※注意此處一定要登入，不然輸出排版會跑掉


...使用第二題資料...

進入minix共享資料夾
# cd /mnt/共享資料夾名稱/2

將共享資料夾第一題的資料以及clock.c檔複製到kernel
# cp proc.c /usr/src/minix/kernel
# cp proc.h /usr/src/minix/kernel
# cp system.c /usr/src/minix/kernel
# cp clock.c /usr/src/minix/kernel

編譯kernel
# cd /usr/src/minix
# make clean
# cd /usr/src/minix/kernel
# make clean
# make kernel
# make install
# cp /usr/sbin/kernel /boot/minix_latest/kernel
# reboot

（重新啟動後，選擇2）

登入，此時螢幕每個一段時間會輸出上一段時間內每個優先級的process數量


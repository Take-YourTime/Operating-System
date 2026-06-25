[Introduction]

This shell is made by B113040045 Hsu Yu-Chang.

It can execute below commands.\n"
	
	exit: end program

	cd: change directory
	
	pwd: desplay current directory

	history: desplay all the used-commands, the second argument control how many commands will be desplay

	help: desplay the commands list

	NSYSU: introduce NSYSU

	rabbit: display a rabbit

The above function is as same as my shell in HW1.

Hoever if input the word which wasn't show above, it will output several time by child process.



[Testing]

# 編譯
clang shell.c -o shell

# 呼叫執行檔時，第二個變數表示會生成多少個child process；如果沒有輸入，會預設為1。
./shell 3

# shell基本功能
rabbit

NSYSU

pwd

# shared memory
abc123

77881122

999

12345 &

555 &

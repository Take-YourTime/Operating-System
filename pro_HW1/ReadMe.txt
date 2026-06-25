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

...and the other commands which can also execute on minix3.


It also can handle these shell operator:
	
	&

	>
	
	>>
	
	|


===========================================================================
[Testing Procedure]

ls

ls -1

ls -l -a

ls &

ls -l > foo

ls >> foo

sort < testfile.txt
(output will be)
	a
	a
	b
	b
	c
	c

ls -l | sort

pwd

cd ..

rabbit

NSYSU

history 5

history

exit
# Mini_Shell_using_C++

The command that has to be typed from command prompt to execute your program:
```
gcc shell.c
./a.out
```

to exit from the minishell type
```
exit
```

The following things are implemented in the minishell and there respective commands:
### Firstly all external commands are executed some are:
	man
	which
	chsh
	whereis
	passwd
	date
	cal
	clear
	sleep
	apropos
	shutdown
	ls
	cat
	more
	less
	touch
	cp
	mv
	rm
	find
	script
	mkdir
	rmdir
	chmod
	grep
	wc
etc.
For using this use simply as we using in linux terminal with all the parameters or flags as required.

### Some internal command are also implemented in the minishell like:

* echo=>
 It can be used in the similar way as used the terminal. Various cases are handled like $ , " "    or simple string. Also the spaces are also handled like it is handled in the terminal.
 try:     echo $USER
	  echo abcd
	  echo "hello pranshu"
	  echo     "hello pranshu"   $HOME    abcd    $USER  "hi ih"    ji
or any general combination of these
 	  
* cd=>
Various implementations are also implemented.
try:     cd <directory path>
         cd <dir1>/<dir2>/<dir3>/
         cd ~
         cd ~/
         cd ..
         cd .
         cd -
         cd

* printenv=>
Basically five environment as told were implemented like PATH,HOME,USER,SHELL,TERM.
try:     printenv
         printenv HOME
         printenv PATH USER TERM

* setenv=>
Set the environment variable.
try: setenv USER = kandoi
     setenv PS1 = %

* history=>
Shows the history.
can try: history

* help=>
shows basic help.
try: help

* pwd=>
shows current working directory.
try: pwd

* exit=>
exit the minishell.
try: exit
     quit
     X

* $VAR=>
this is also implemented which gives the environment variable.
try: $USER
     $PATH
     
### Single piping is also implemented 
which can work with all compatible internal and external commands like pwd, cat, grep and all other.
try: pwd | wc -c
     pwd | grep h

### input output redirection is also implemented
The various cases handled are:
	<
	>
	>>
	< >
	< >>
	
### Basic on the implementation.
For making the assignment firstly input is taking line by line.
then it is parsed using various parser and argumnents are obtained.
the the arguments are passed to execute function which executes the required command.
thus execute works like first it check the command on the basis of the internal command if not found then searches in the external commmand if found executes if not found then returns error.
For achieving various system calls are used.


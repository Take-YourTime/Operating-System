#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 10
#define MAX_HISTORY_SIZE 256

//	use in "history"
char* history[MAX_HISTORY_SIZE];
int history_count = 0;

int flag_back, flag_out, flag_add, flag_in, flag_pipe;
int fd1, fd2;	// file descriptor
char file_out[MAX_COMMAND_LENGTH], file_in[MAX_COMMAND_LENGTH];


void add_to_history(const char* command)
{
	if(history_count < MAX_HISTORY_SIZE)
	{
		/* history[history_count++] = strdup(command); */
		history[history_count] = (char*)malloc(strlen(command) + 1);
        strcpy(history[history_count++], command);
	}
	else
	{
		free(history[0]);
		//	move forward all the history commands
		for(int i = 0; i < MAX_HISTORY_SIZE - 1; i++)
			history[i] = history[i+1];
		
		history[MAX_HISTORY_SIZE - 1] = (char*)malloc(strlen(command) + 1);
        strcpy(history[MAX_HISTORY_SIZE - 1], command);
	}
}


void print_history(int n)
{
	int move = (n > history_count) ? history_count : n;
	
	for(int i = history_count - move; i < history_count; i++)
		printf(" %d %s\n", i, history[i]);
}


void execute_command(char** arguments, int command_count, int* status, int pipe_count)
{
	int pipe_index = 0;
	char* pipe_arguments[ pipe_count ][ MAX_COMMAND_LENGTH ];
	
	//	detect &
	if( !strcmp(arguments[command_count - 1], "&") )
	{
		flag_back = 1;
		arguments[command_count - 1] = NULL;
		command_count--;
	}
	
	//	detect >, >>, <, |
	for(int i = 0, pipe_copy_index = 0; i < command_count; i++)
	{
		if(!strcmp(arguments[i], ">"))
		{
			flag_out = 1;
			strcpy(file_out, arguments[i + 1]);
			
			arguments[command_count - 2] = NULL;
			command_count -= 2;
			i--;
		}
		else if(!strcmp(arguments[i], ">>"))
		{
			flag_add = 1;
			strcpy(file_out, arguments[i + 1]);
			
			arguments[command_count - 2] = NULL;
			command_count -= 2;
			i--;
		}
		else if(!strcmp(arguments[i], "<"))
		{
			flag_in = 1;
			strcpy(file_in, arguments[i + 1]);
			
			arguments[command_count - 2] = NULL;
			command_count -= 2;
			i--;
		}
		else if(!strcmp(arguments[i], "|"))
		{
			flag_pipe = 1;
			
			//	store the command which is in front of "|" into arguments2
			for(int j = 0; j < i; j++, pipe_copy_index++)
				pipe_arguments[pipe_index][j] = arguments[pipe_copy_index];
			pipe_arguments[pipe_index][i] = NULL;
			pipe_copy_index = i + 1;
			
			pipe_index++;
			if(pipe_index == pipe_count)
			{
				//	reset arguments, and store the command which behind "|"
				for(int j = 0; j < command_count - i - 1; j++)
					arguments[j] = arguments[j + i + 1];
				arguments[command_count - i - 1] = NULL;
			}
		}
	}
	
	// 分叉第一個子進程
   	pid_t pid1 = fork();
    

    if (pid1 < 0)
    {
        // fork error
        perror("fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    {
    	//	>, ouput the command result to a file. 
    	//		if the file exists, rewrite it; if not exists, create a new file.
    	if(flag_out)
    	{
    		fd1 = open(file_out, O_RDWR | O_CREAT | O_TRUNC, 0644);
    		if(fd1 < 0)
    		{
    			printf("> open %s error!\n", file_out);
    			exit(EXIT_FAILURE);
    		}
    		dup2(fd1, STDOUT_FILENO);	// STDOUT_FILENO = 1, this is equal to dup2(fd1, 1)
    	}
    	//	>>, add the command result to a file
    	//		If the file exists, add the result to the end; if not exists, create a new file.
    	if(flag_add)
    	{
    		fd1 = open(file_out, O_RDWR | O_CREAT | O_APPEND, 0644);
    		if(fd1 < 0)
    		{
    			printf(">> open %s error!\n", file_out);
    			exit(EXIT_FAILURE);
    		}
    		dup2(fd1, STDOUT_FILENO);
    	}

    	//	<, input the command from the file
    	if(flag_in)
    	{
    		fd1 = open(file_in, O_RDONLY);
    		if(fd1 < 0)
    		{
    			printf("< open %s error!\n", file_in);
    			exit(EXIT_FAILURE);
    		}
    		dup2(fd1, 0);
    	}
    	
    	//	|
    	if(flag_pipe)
    	{
    		int status2;
    		pid_t pid2[pipe_count];
    		
    		for(int i = 0; i < pipe_count; i++)
    		{
    			pid2[i] = fork();
				if(pid2[i] < 0)
				{
					perror("fork2 error\n");
					exit(EXIT_FAILURE);
				}
				else if(pid2[i] == 0)
				{
					fd2 = open("/tmp/tempfile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(fd2, STDOUT_FILENO);
					execvp(pipe_arguments[i][0], pipe_arguments[i]);
					exit(EXIT_SUCCESS);
				}
				else
				{
					waitpid(pid2[i], &status2, 0);
					fd2 = open("/tmp/tempfile", O_RDONLY);
					dup2(fd2, 0);
				}
				
				remove("/tmp/tempfile");
    		}
				
    	}
		
    	execvp(arguments[0], arguments);
    	exit(EXIT_SUCCESS);
    }
    else
    {
    	//	parent process
    	if(flag_back)
    	{
    		printf("[process ID %d]\n", pid1);
    	}
    	else
    	{
    		waitpid(pid1, status, 0);  // 等待第一個子進程完成
    	}
    }
}


int main(int argc, char **argv, char **envp)
{
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS];
    char *token;
    int status;
    
	printf("Input \"help\" to get the shell guide\n");
	
    while (1)
    {
    	//	reset variable
    	flag_back = flag_out = flag_add = flag_in = flag_pipe = 0;
    	fd1 = fd2 = 0;
    
    	
        printf("minix3-shell> ");
        
        //	read a line form "stdin"
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        //	remove '\n' in the command
        if ((strlen(command) > 0) && (command[strlen(command) - 1] == '\n'))
        {
            command[strlen(command) - 1] = '\0';
        }
	
		//	record the command
		add_to_history(command);
	
        //	解析命令行
        token = strtok(command, " ");
        
        //	the number of words of this command
        int command_count = 0;
        
        //	character index
        int i = 0;
        int pipe_count = 0;
        
        //	seperate the command into arguments
        while (token != NULL && i < MAX_ARGUMENTS - 1)
        {
            arguments[i] = token;
            if( !strcmp(arguments[i], "|") )
            	pipe_count++;
            token = strtok(NULL, " ");
            command_count++;
            i++;
        }
        arguments[i] = NULL;


	    /* own can handle command*/
        if ( !strcmp(arguments[0], "exit") )
        {
	        //	end program
        	if(command_count == 1)
        	{
        		exit(EXIT_SUCCESS);
            	break;
            }
        }
        else if( !strcmp(arguments[0], "cd") )
        {
        	/* cd */
	        //	change directory
        	if(command_count != 1)
        	{
        		if( chdir(arguments[1]) == -1)
        			perror("cd: Worng directory!\n");
			}
		}
		else if ( !strcmp(arguments[0], "pwd") )
		{
			/* pwd */
			// show the current directory
        	char cwd[1024];
		    if (getcwd(cwd, sizeof(cwd)) != NULL) 
		        printf("%s\n", cwd);
		    else
		        perror("getcwd");
   		}
		else if( !strcmp(arguments[0], "\000") )
		{
			continue;
		}
		else if( !strcmp(arguments[0], "history") )
		{
			/* history */
			//	show the history commands
			if(command_count != 1)
			{
				//	show the above n commands on the screen
				int n = atoi(arguments[1]);
				print_history(n);
			}
			else
				print_history(MAX_HISTORY_SIZE);
		}
		else if( !strcmp(arguments[0], "help") )
		{
			printf("This shell is made by B113040045 Hsu Yu-Chang.\n");
			printf("It can execute below commands.\n");
			printf("\texit\n");
			printf("\tcd\n");
			printf("\tpwd\n");
			printf("\thistory\n");
			printf("\thelp\n");
			printf("\tNSYSU\n");
			printf("\trabbit\n");
			printf("\t...and the other commands which can also execute on minix3.\n");
		}
		else if( !strcmp(arguments[0], "NSYSU") )
		{
			printf("It's a famous college located in Sizihwan Kaohsiung,and also known as Never Support You Strongly University.\n");
		}
		else if( !strcmp(arguments[0], "rabbit") )
		{
			printf("  {\\ __ /}\n");
			printf("\n");
			printf("  ( o - o)\n");
			printf("\n");
			printf("c /> O  <\\\n");
		}
		else
		{
			/*	handled by system */
			execute_command(arguments, command_count, &status, pipe_count);
		}
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 10
#define MAX_HISTORY_SIZE 256
#define MAX_PROCS 1024

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE 4096

char* history[MAX_HISTORY_SIZE];
int history_count = 0;
int flag_back;

/* shared memory variable */
int shm_fd;
char* shared_dir;
const char *message = "Hello, shared memory!";
const char *pathname = "/tmp/example_file";

// Information for each process
struct ProcessInfo {
    int exit_status;
    int is_valid;
};

// Array to hold process information
struct ProcessInfo process_info[MAX_PROCS];

// Initialize process information
void init_process_info()
{
    for (int i = 0; i < MAX_PROCS; i++)
	{
        process_info[i].exit_status = -1;
        process_info[i].is_valid = 0;
    }
}

// Wait for a process to exit and return its exit status
int Waitpid(pid_t pid)
{
    if (pid < 0 || pid >= MAX_PROCS || !process_info[pid].is_valid)
	{
        return -1;  // Invalid process ID or process does not exist
    }
	
	waitpid(pid, &(process_info[pid].exit_status), 0);
	

    return process_info[pid].exit_status;
}

// Fork a new process
pid_t ProcessFork(void)
{
    pid_t pid = fork();
    if (pid < 0)
	{
        return -1;  // Error forking
    }
	else if (pid == 0)
	{
        return 0;   // Child process returns 0
    }
	else
	{
        return pid;  // Parent process returns child PID
    }
}

// Exit the current process with a given exit status
void Exit(int exit_status)
{
    pid_t pid = getpid();
    
    if (pid >= MAX_PROCS)
	{
        exit(EXIT_FAILURE);  // This should not happen
    }

    process_info[pid].exit_status = exit_status;
    process_info[pid].is_valid = 1;
}

// Spawn a new process with program name and argument
pid_t Spawn(const char *str, int arg) {
	pid_t pid;
	
	printf("Message '%s' written to shared memory\n", str);
	
	for(int i = 0; i < arg; i++)
	{
		pid = ProcessFork();
	    
	    if (pid < 0)
		{
	        return -1;  // Failed to create new process
	    }
	    else if (pid == 0)
		{	/* Child process */
	        printf("\tReading from shared memory: %s\n", (char *)shared_dir);
	        exit(EXIT_SUCCESS);  // Ensure child exits after reading
	    }
		else
		{  	/* Parent process */
			// Write to shared memory
			snprintf(shared_dir, SHM_SIZE, "%s", str);
	        if (!flag_back)
			{
	            Exit(Waitpid(pid));	// Wait for child if running in foreground
			}
			
	        // Clear process info for reuse
	        process_info[pid].exit_status = -1;
	        process_info[pid].is_valid = 0;
	    }
	}
	
    return pid;
}

// Add command to history
void add_to_history(const char* command)
{
    if (history_count < MAX_HISTORY_SIZE)
	{
        history[history_count] = strdup(command);
        history_count++;
    }
	else
	{
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++)
		{
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY_SIZE - 1] = strdup(command);
    }
}

// Print history commands
void print_history(int n) {
    int move = (n > history_count) ? history_count : n;
    for (int i = history_count - move; i < history_count; i++)
	{
        printf(" %d %s\n", i, history[i]);
    }
}

int main(int argc, char **argv)
{
	//	讀取執行程式時的參數
	int child_process_quantity;
	if(argc > 1)
		child_process_quantity = atoi(argv[1]);
	else
		child_process_quantity = 1;
	printf("Child process quantity: %d\n", child_process_quantity);
	
	/*------------------------------------------------------------------*/
	/* create shared memory */
	
	int shmid;	//	store the return value from shmget()
	key_t key;
	
	key = ftok(".", 'a');
	if(key == -1)
	{
		perror("ftok() fause\n");
		exit(EXIT_FAILURE);
	}
	
	//	create shared memory
	if ( (shmid = shmget (key, SHM_SIZE, 0666|IPC_CREAT)) == -1)
	{
		perror("shmget: shmget failed");
		exit(EXIT_FAILURE);
	}
 	
 	
    // Map the shared memory segment to the address space of the process
    shared_dir = (char*)shmat(shmid, (void *)0, 0);
	if(shared_dir == (char*)-1)
	{
        perror("shared_dir fause\n");
        exit(EXIT_FAILURE);
    }
	
	/*------------------------------------------------------------------*/
	
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS] = {NULL};
    char *token;
    
    init_process_info();
    printf("Input \"help\" to get the shell guide\n");
    
    while (1) {
        printf("minix3-shell> ");
        // clean shared memory
		if (shared_dir != NULL)
		{
			memset(shared_dir, 0, SHM_SIZE);
		}


        fgets(command, MAX_COMMAND_LENGTH, stdin);
        if ((strlen(command) > 0) && (command[strlen(command) - 1] == '\n'))
		{
            command[strlen(command) - 1] = '\0';
        }
        
        add_to_history(command);
        
        token = strtok(command, " ");
        int command_count = 0;
        int i = 0;
        
        while (token != NULL && i < MAX_ARGUMENTS - 1)
		{
            arguments[i] = token;
            token = strtok(NULL, " ");
            command_count++;
            i++;
        }
        arguments[i] = NULL;
        
        
        if( !strcmp(arguments[command_count - 1], "&") )
		{
			flag_back = 1;
			arguments[command_count - 1] = NULL;
			command_count--;
		}
		else
		{
			flag_back = 0;
		}
			
        if (!strcmp(arguments[0], "exit"))
		{
            if (command_count == 1)
			{   
				if(shmdt(shared_dir) == -1)	// Detach the shared memory segment
					perror("detach false\n");
				if(shmctl(shmid, IPC_RMID, NULL) == -1)	// Remove the shared memory segment
					perror("remove shared memory segment false\n");
				break;
 			}
        }
		else if (!strcmp(arguments[0], "cd"))
		{
            if (command_count != 1)
			{
                if (chdir(arguments[1]) == -1)
				{
                    perror("cd: Wrong directory!\n");
                }
            }
        }
		else if (!strcmp(arguments[0], "pwd"))
		{
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
			{
                printf("%s\n", cwd);
            }
			else
			{
                perror("getcwd");
            }
        }
		else if (!strcmp(arguments[0], "history"))
		{
            if (command_count != 1)
			{
                int n = atoi(arguments[1]);
                print_history(n);
            }
			else
			{
                print_history(MAX_HISTORY_SIZE);
            }
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
            int a = Spawn(arguments[0], child_process_quantity);
        }
    }

    return 0;
}

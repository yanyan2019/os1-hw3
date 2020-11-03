#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

void ssh_interface(struct command*);
void parseData(struct command *, char *);
void buildin_cmd(struct command *);
void other_cmd(struct command *);
int* expansion_var(struct command *);

//ssh interface
void ssh_interface(struct command * cmd){
	int checkFore;
	size_t str_len;
	size_t n_chars;
	char* str = (char*)malloc(sizeof(char)*MAX_SIZE);

	printf("& smallssh\n");
	while(1){
		cmd->num_args = 0;
		printf(": ");
	
		// get line to read command line
		memset(str,'\0', sizeof(str));

		n_chars = getline(&str, &str_len, stdin);

//		printf("length:%i of string: %s\n", strlen(str), str);	
//		printf("str[0]: %c\n", str[0]);

		// check for empty or comment command line
		if(str[0] != '#'&& strlen(str) > 1){
			parseData(cmd,str);
		
			// check for built in command
			buildin_cmd(cmd);			

			// check for foreground or background
			if(strncmp(cmd->cmdline[cmd->num_args-1], "&", 1) == 0){
				//background	
				
			}else{
				// foreground
			//	other_cmd(cmd);
			}					
		}
		printf("\n");
	}
}

// parse data and blank line handling
void parseData(struct command * cmd, char * str){
//	printf("in parse data function:\n");
	int i= 0;
	int * match_index;
	char *t_ptr;
	char *token = strtok_r(str, " \n", &t_ptr);
//	strcpy(cmd->name, token);
//	printf("str name: %s\n", &t_ptr);	
	
	// extract command from input var to a stuct command	
	while(token != NULL){
//		printf("token lenght: %i\n", strlen(token));
//   		strcpy(cmd->cmdline[i], token);
		memset(cmd->cmdline[i], '\0', sizeof(cmd->cmdline[i]));
		strcpy(cmd->cmdline[i], token);
		cmd->num_args++; 
   		token = strtok_r(NULL, " \n", &t_ptr);
//		cmd->num_args++;
		i++;
	}
//	printCmd(cmd);
//	match_index = expansion_var(cmd);
	
}

// Provide expansion for the variable $$
int* expansion_var(struct command * cmd){
	int i,j, k, count;
	int * match_index = (int *)malloc(sizeof(int)*MAX_LINE);
	// loop though command
	for(i = 0; i < MAX_LINE; i++){
	//	printf("size: %i, macro %i\n", strlen(cmd->cmdline[0]), MAX_LINE);
		// check for $$
		for(j = 0; j < strlen(cmd->cmdline[i]); j++){
//				printf("check $$\n");
//				printf("%c", cmd->cmdline[i][j+1]);
			
			// check for executive $$				
			if(cmd->cmdline[i][j] == '$' && cmd->cmdline[i][j+1] == '$'){
				printf("first match at %i and %i \n", j, j+1);
						
				// save location
				match_index[k] = j;
				match_index[k+1] = j+1;	
				// skip to next element
				j = j +1;
				k++;		
			}
		}
	}
	return match_index;	
}

// Build in 3 commands
void buildin_cmd(struct command* cmd){
//	printf("cmd0: %s %i \n", cmd->cmdline[0], strcmp(cmd->cmdline[0], "exit"));

	char token[MAX_SIZE];
	char dir[MAX_SIZE];
	memset(dir, '\0', sizeof(dir));
	
//	printf("cmdline->line: [%c]\n", cmd->cmdline[0][3]);
	// conditions to check which command
	if(strcmp(cmd->cmdline[0], "exit") == 0){
//		printf("exit found\n");
		exit(0);
	}else if(strcmp(cmd->cmdline[0], "cd") == 0){
		// HOME
		if(cmd->num_args == 1){
			chdir(getenv("HOME"));

		// Any path
		}else if(cmd->num_args > 1){
			strcpy(dir, "./");
			strcat(dir, cmd->cmdline[1]);
			chdir(dir);
		}
			// print result
			getcwd(dir, sizeof(dir));
			printf("%s\n", dir);
	}else if(strcmp(cmd->cmdline[0] , "status") == 0){
//		printf("status\n");
		
	// call non build in commands
	}else{
		//printf("print command %s %i\n", cmd->cmdline[0], strlen(cmd->cmdline[0]));	
		other_cmd(cmd);		
	}
}

// Execute other commands by creating new processes using a function from the exec family of functions, this functionis based on exploration: monitoring child process
void other_cmd(struct command * cmd){
//	printf("other command function\n");
	pid_t pid;
	int childStatus, i;
	

	pid = fork();
	if(pid == -1){
		perror("fork() failed!");
		exit(1);
	}else if(pid == 0){
		// execute commands
//		printf("num of argus: %i\n", cmd->num_args);
		cmd->cmdline[cmd->num_args] = NULL;
//		memset(cmd->cmdline[cmd->num_args], '\0', sizeof(cmd->cmdline[cmd->num_args]));
		if(cmd->cmdline[cmd->num_args] == NULL){
			//printf("print command %s %i\n", cmd->cmdline[0], strlen(cmd->cmdline[0]));
			execvp(cmd->cmdline[0], cmd->cmdline);
		}
		perror("execvp");
		return;
//		exit(1);
	}else{
		//parent process
		pid_t childPid = waitpid(pid, &childStatus, 0);
		return;
//		exit(0);
	}		

}

// Support input and output redirection

// Support running commands in foreground and background processes

// Implement custom handlers for 2 signals, SIGINT and SIGTSTP

int main(){
	
	struct command * cmd;
	cmd = init_cmd();

	// ssh interface
	ssh_interface(cmd);
	
	free_cmd(cmd);
	return 0;
}

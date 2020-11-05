#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "command.h"
#include <fcntl.h>

//prototypes
void ssh_interface(struct command*);
void parseData(struct command *, char *);
void buildin_cmd(struct command *);
void other_cmd(struct command *);
int* expansion_var(struct command *);
int in_out_redirect(struct command *);

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

		// check for empty or comment command line
		if(str[0] != '#'&& strlen(str) > 1){
			parseData(cmd,str);
		
			// check for built in command
			buildin_cmd(cmd);			

			// check for foreground or background
			if(strncmp(cmd->cmdline[cmd->num_args-1], "&", 1) == 0){
				//background	
				printf("background\n");
			}else{
				// foreground
				//other_cmd(md);
			}					
		}
//		printf("\n");
	}
}

// parse data and blank line handling
void parseData(struct command * cmd, char * str){
//	printf("in parse data function:\n");
	int i= 0;
	int * match_index;
	char *t_ptr;
	char *token = strtok_r(str, " \n", &t_ptr);
	
	// extract command from input var to a stuct command	
	while(token != NULL){

		memset(cmd->cmdline[i], '\0', sizeof(cmd->cmdline[i]));
		strcpy(cmd->cmdline[i], token);
		cmd->num_args++; 
   		token = strtok_r(NULL, " \n", &t_ptr);
		i++;
	}
//	printCmd(cmd);
	
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

	char token[MAX_SIZE];
	char dir[MAX_SIZE];
	memset(dir, '\0', sizeof(dir));
	
	// conditions to check which command
	if(strcmp(cmd->cmdline[0], "exit") == 0){

		exit(0);

	}else if(strcmp(cmd->cmdline[0], "cd") == 0){
		// HOME path
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

	}else{
		// check for non-build in commands
		other_cmd(cmd);		
	}
}

// Execute other commands by creating new processes using a function from the exec family of functions, this functionis based on exploration: monitoring child process
void other_cmd(struct command * cmd){
//printf("other command function\n");
	pid_t pid;
	int childStatus, i;
	int check;	
	char* temp[2];

	pid = fork();
	

	if(pid == -1){
		perror("fork() failed!");
		exit(1);
	}else if(pid == 0){ // child process
		// check >
		check = in_out_redirect(cmd);
		//printf("check after inout: %i\n", check);
	        fflush(stdout);	
		if(check != -1){

			printf("found < or >\n");	
			temp[0]= (char*)malloc(sizeof(char)*MAX_SIZE);
			temp[1] = (char*)malloc (sizeof(char)*MAX_SIZE);	

			strcpy(temp[0], cmd->cmdline[check]);
			temp[1] = NULL;
			printf("temp :%s \n", cmd->cmdline[check]);
			fflush(stdout);
    			execlp(cmd->cmdline[check],cmd->cmdline[check], NULL);
//		}			
			perror("execlp");	
			exit(1);
		}else if(check == -1) {	
//			printf("after checking in out redirect fcuntion:\n");
			cmd->cmdline[cmd->num_args] = NULL;
			if(cmd->cmdline[cmd->num_args] == NULL){
				execvp(cmd->cmdline[0],cmd->cmdline);
			}
		

		perror("execvp");
		exit(1);
		}
	}else{
		//parent process
		pid_t childPid = waitpid(pid, &childStatus, 0);
	}		

}

// Support input and output redirection
int in_out_redirect(struct command * cmd){
	int i = 0, check = 0, index = -1;
	int out_fd, in_fd;
	int in, out;
//	printf("in in_out function,check:%i\n", check);
	// search for input/output symbol
	while(i < cmd->num_args){
		check = 0;
		if(strcmp(cmd->cmdline[i], "<") == 0 ){
         		out = i - 1;
          		in = i + 1;
			check = 1;
    		}else if (strcmp(cmd->cmdline[i], ">") == 0){
          		out = i + 1;
          		in = i - 1;
			check = 1;
    		}
		if(check == 1){
		//	printf("match found!\n");
			out_fd = open(cmd->cmdline[out], O_WRONLY | O_CREAT | O_TRUNC, 0777);
			printf("file_name: %s\n", cmd->cmdline[out]);
			fflush(stdout);
			if(out_fd == -1){
				printf("Can not open %s\n", cmd->cmdline[out]);
				fflush(stdout);
				exit(1);
			}
      			int result = dup2(out_fd, 1);
			if (result == -1) { 
				perror("target dup2()"); 
				exit(2); 
 			}
			in_fd = open(cmd->cmdline[in], O_RDONLY, 0777);
			if(in_fd == -1){
				if(cmd->num_args <= 3){
					index = in;
					break;
					//printf("new command index: %i\n", in);
					//fflush(stdin);
				}
           			perror("Can not open in file");
				exit(1);
			}
			else{
				result = dup2(in_fd, 0);
				if (result == -1) { 
    					perror("source dup2()"); 
    					exit(2); 
				}
			}
		}
		i++;
	}
	printf("index: %i\n");
	if(out_fd != -1){
		fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	}
	if(in_fd != -1){
		fcntl(in_fd, F_SETFD, FD_CLOEXEC);
		
	}	

	return index;	
		
}
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

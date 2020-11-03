//command.h
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 512
#define MAX_SIZE 2048

// struct for command line
struct command{
 // char  name[MAX_SIZE];
	char * cmdline[MAX_LINE];
  int num_args;           // total number of arguments including the first commamd
	char* inFile;          // name of the input file
	char* outFile;        // name of the outout file
	int input_rdrect;    // 0 is not redirect, 1 is redirect
	int output_rdrect;   // 0 is not redirect, 1 is redirect
	int foreOrBack;     // foreground = 0, background =1
};

// allocate memory to command line
struct command * init_cmd(){
	struct command * cmd = malloc(sizeof(struct command));
	int i;
//  memset(cmd->name, '\0', sizeof(cmd->name));
//  cmd->cmdline = (char*)malloc(sizeof(char)* MAX_LINE);
	for(i = 0; i < MAX_LINE; i++){
		cmd->cmdline[i]  = (char*)malloc(sizeof(char) * MAX_SIZE);
  //strcpy(cmd->cmdline[i],"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  //:wq printf("cmdline %i :%s\n",i+1, cmd->cmdline[i]);
   
	}
  cmd->num_args = 0;
	cmd->inFile = malloc(sizeof(char) * MAX_SIZE);
	cmd->outFile = malloc(sizeof(char) * MAX_SIZE);
	cmd->input_rdrect = 0;
	cmd->output_rdrect = 0;
	cmd->foreOrBack = 0;
	return cmd;
}

// print command
void printCmd(struct command * cmd){
  int i;
  printf("cmdline: \n");
 // printf("name: %s\n",cmd->name);
  for(i = 0; i < MAX_LINE; i++){
    printf("%i: %s\n",i+1,cmd->cmdline[i]);
  }
  printf("num_args: %i\n", cmd->num_args);
 // printf("",inFile);
}

// free memory of struct
void free_cmd(struct command * cmd){
	int i;
 
 if(cmd != NULL){
	for(i = 0; i < MAX_LINE; i++){
		free(cmd->cmdline[i]);
	}
  //free(cmd->name);
	free(cmd->inFile);
	free(cmd->outFile);
	free(cmd);
 }
}

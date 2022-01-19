#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include "alias.h"

size_t sizeVar = 512; // variable to store size 
int numArgument = 0; // variable to store number of arguments
char userPrompt[50] = "MYNEWSHELL"; // variable to store default prompt line
char *myhistoryarr[512]; // varibale to store list of commands from history
int myhistory_count=0; // variable to store the number of commands from history 
pid_t pid; // variable to store the process ID
struct sigaction act; 

char *readCmdline(); // 
char **lineParsing(char *commandBlock); // function declaration to parse the command line by removing any kind of whitespaces
int comdExecution(char **finalCommand, char **separateComd); // function declaration to execute different types of build-in commands and default commands
int redirection(char **finalCommand); // function declaration to perfom the redirection command
void totalProcess(char *commandLine); // function declaration to separate different commands and perform the total userinput commands
int path(char **command); // function declaration to perform the path command 
int myhistory(); // function declaration to print out the history
int delete_history();// function declaration to delete the history
//int exitprogram(char **command); 
int alias(char aliasComd[256], int idx);// Execute alias command
int changeDirectory(char dir[32]); //Execute cd command



int main(int argc, char **argv)
{	

	if (argc > 2) // displaying error message if there is more than 2 arguments passed
	{
		perror("Error !!! Invalid number of arguments");
		exit(0);
	}

	char *commandLine = malloc(sizeVar * sizeof(char)); // character array to store the user entered command line
	if (commandLine==NULL)
	{
		printf("Memory Allocation failed.\n");
		exit(1); // error handling for the memory allocation error 
	}

	while(1) 
	{ 
		tcsetpgrp(fileno(stdin), getpgrp());
		act.sa_handler = SIG_IGN;
		assert(sigaction(SIGINT, &act, NULL) == 0);
		assert(sigaction(SIGQUIT, &act, NULL) == 0);
		if(argc == 2) // if only two arguments passed in main
		{
			// BATCH MODE 
			FILE *fptr;
			fptr = fopen(argv[1], "r");
			if (fptr == NULL)
			{	printf("The batch file was unable to open.\n");
				exit(0);
			}
			while(!feof(fptr))
			{	
				getline(&commandLine, &sizeVar, fptr);

				if(commandLine == NULL) {
					printf("You have not entered any command. Please try again.\n");
					return 1;
				}
				totalProcess(commandLine);

			}
			fclose(fptr);
			break;
		}  
		else  //if no paramters is passed in main, the program runs interactive mode
		{	// INTERACTIVE MODE

			printf("\n%s> ",userPrompt);  
			// taking input as the command line
			int h = 0;
			h=getline(&commandLine, &sizeVar, stdin);
			//removing newline(\n) from the input
			commandLine[strcspn(commandLine, "\n")] = 0;
			//check to see of the entered command is an alias
			for(int i = 0; i<256; i++){
				char *p;
				p = malloc (sizeof (char) * 15);
				if (p == NULL){
					printf("Error! Memory allocation failed. \n");}
				strcpy(p, commandLine);
				if(strcmp(p, aliasname[i]) == 0){
					executeAliasCmd(i);
					p = NULL;
				}
			}
			//ask for user prompt
			while (h ==1)
			{
				printf("%s> ",userPrompt);
				h=getline(&commandLine, &sizeVar, stdin);   
			}

			//required for my history
			if (myhistory_count==20)
			{
				myhistory_count=0;
			}

			//calling totalProcess and flushing from buffer
			myhistoryarr[myhistory_count++] = strdup(commandLine);
			fflush(stdout); 
			totalProcess(commandLine);
		}
	}	

	//reset path
	unsetenv("PATH");

	return 0;
}

//go through each comand one by one and get it ready for parsing
void totalProcess(char *commandLine) 
{
	char **commands;
	int num_of_Cmnd = 0;
	char *temp;
	char **separateComd;
	separateComd = malloc(sizeVar * sizeof(char*));
	if (separateComd==NULL)
	{
		exit(1); // error handling for allocation error
	}

	temp = strdup(commandLine);

	char *var;
	var = strtok(temp, ";");
	while (var != NULL)
	{
		separateComd[num_of_Cmnd] = var;
		if(strncmp(separateComd[num_of_Cmnd], "alias", 5)==0){
			char *p;
			p = malloc(sizeof(char) * 256);
			strcpy(p, separateComd[num_of_Cmnd]);
			alias(p, index1);
			p = NULL;
		}
		separateComd = realloc(separateComd, sizeVar * sizeof(char*));
		var = strtok(NULL, ";");
		num_of_Cmnd++;
	}

	for(int i=0; i<num_of_Cmnd; i++)
	{
		if(( strcmp(separateComd[i], " exit ") == 0))
		{	
			i++;						// making sure to skip exit and run exit at the end. 	
			commands = lineParsing(separateComd[i]);
			comdExecution(commands, separateComd);
			free(commandLine);
			free(commands);
			exit(0);
		}
		else
		{
			commands = lineParsing(separateComd[i]);
			comdExecution(commands, separateComd);
			free(commandLine);
			free(commands);
		}
	}
	return;     
}	

//parsing input to call appropriate function
char **lineParsing(char *commandBlock)
{
	char space[] = {" \t\r\n\a"};     
	char **finalCommand = malloc(sizeVar * sizeof(char*));
	if (finalCommand==NULL)
	{
		printf("Memory Allocation failed.\n"); // error handling for memory allocation error
		exit(1);
	}
	char *tempTok;
	int count = 0;

	tempTok = strtok(commandBlock, space);
	while(tempTok != NULL)
	{
		finalCommand[count] = tempTok;
		count++;
		finalCommand = realloc(finalCommand, sizeVar * sizeof(char*));
		tempTok = strtok(NULL, space);
	}
	finalCommand[count] = NULL;
	numArgument = count;

	return finalCommand;
}

//execute command
int comdExecution(char **finalCommand, char **separateComd)
{
	//if exit is found
	if( strcmp(finalCommand[0],"exit") == 0)
	{
		exit(-1);
	}
	//check for cd, exit, path, myhistory  
	if (strcmp(finalCommand[0],"path") == 0)
	{
		return path(finalCommand);
	} else if(strcmp(finalCommand[0],"customize") == 0)
	{
		printf("You chose to customize the command prompt term.\n Please enter the new prompt: ");
		char tempChar[50], x;
		scanf("%s", tempChar); 
		scanf("%c", &x);
		strcpy(userPrompt,tempChar);

		return 1;
	}

//For myHistory Command
	else if(strcmp(finalCommand[0],"myhistory")==0)
	{ 
		if(finalCommand[1] == NULL)
		{
			myhistory();
		}
		else if (strcmp(finalCommand[1],"-c")==0)
		{
			delete_history();
		}

		else if( strcmp(finalCommand[1],"-r")==0)
		{
			char n;
			strcpy(&n, finalCommand[2]);
			int t = n-'0';


			printf("[%d] %s\n",t, myhistoryarr[t-1]);

		}
	}
//for cd command
	else if (strcmp(finalCommand[0],"cd")==0)
	{

		if(finalCommand[1] == NULL){
			changeDirectory("/home");
		}
		else {

			changeDirectory(finalCommand[1]);
		}
	}

	//redirectiom
	redirection(finalCommand);

	return 1;
} 

//redirection function defination
int redirection(char **finalCommand) 
{
	// checking for redirection signs
	int lessVar =0, greaterVar = 0; 
	for(int i=0; i <numArgument; i++)
	{
		if(  strcmp(finalCommand[i], "<") == 0)
			lessVar++;
	}

	for(int i=0; i <numArgument; i++)
	{
		if(  strcmp(finalCommand[i], ">") == 0)
			greaterVar++;
	}

	pid_t pid;
	//if any redirection sign is found
	if(greaterVar > 0 || lessVar > 0)
	{
		int fd;
		pid = fork();
		if(pid == 0) 
		{
			if(greaterVar > 0 &&lessVar == 0 ) //if ">" is found
			{
				fd = open(finalCommand[2], O_CREAT | O_TRUNC | O_WRONLY, 0600);

				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
			else if(greaterVar == 0 &&lessVar > 0) //if "<" is found
			{
				fd = open(finalCommand[2], O_RDONLY, 0600);

				dup2(fd, STDIN_FILENO);
				close(fd);
			} else if(greaterVar > 0 && lessVar > 0) //if both "<" & ">" is found
			{
				fd = open(finalCommand[2], O_RDONLY, 0600);
				dup2(fd, STDIN_FILENO);
				close(fd);

				fd = open(finalCommand[2], O_CREAT | O_TRUNC | O_WRONLY, 0600);

				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
			finalCommand[1] = NULL;
			if(execvp(finalCommand[0], finalCommand) == -1) //error handling
			{
				perror("child not executing");
			}

		}
		waitpid(pid,NULL,0); 
	} 
	else
	{

		pid = fork();
		if (pid==-1)
		{
			perror("fork");
			exit(EXIT_FAILURE);                         
		}
		else if(pid == 0) //child
		{
			setpgrp();
			tcsetpgrp(fileno(stdin), getpgid(pid));
			act.sa_handler = SIG_DFL;
			assert(sigaction(SIGINT, &act, NULL) == 0);
			assert(sigaction(SIGQUIT, &act, NULL) == 0);
			execvp(finalCommand[0], finalCommand);
			printf("execlp failed\n");
			exit(EXIT_FAILURE);

			exit(1);
		}

		else //if (pid > 0) 
		{
			setpgid(pid, pid);
			signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(fileno(stdin), getpgid(pid));
			wait(NULL);

			int random;
			waitpid(pid, &random, 0); 
			while (!WIFEXITED(random) && !WIFSIGNALED(random))
			{
				waitpid(pid, &random, 0);
			}
		}
		
	}
	return 1;

}

int path(char **command) // fucntion to print out the path command 
{

	char *delLine; // variable to store the term to be deleted
	if(command[1] == NULL) // if just path command is passed without any "+" or "-"
	{
		printf("PATH = %s\n", getenv("PATH")); 
	}

	else if(strcmp(command[1], "+") == 0){ // appending userinput term to path
		strcat(getenv("PATH"), ":"); // appending colon at the end of the path 
		strcat(getenv("PATH"), command[2]); // appending the added term to path
		setenv("PATH", getenv("PATH"), 1);
	}

	else if(strcmp(command[1], "-") == 0){
		delLine = strstr(getenv("PATH"), command[2]);//removing user input form path
		delLine--;
		strncpy(delLine, "", 1);
		setenv("PATH", getenv("PATH"), 1);
	}
	else{

		printf("PATH = %s\n", getenv("PATH"));//printing path
	}
	return 1;
}


//function defination for myhistory()
int myhistory()
{

	printf("\nDisplaying Recent Histories...\n");
	for(int i=0; i < myhistory_count; i++)
	{
		printf("[%d] %s",(i+1), myhistoryarr[i]);
	}
	return 1;
}


//for deleting history
int delete_history()
{

	for(int i=0; i < myhistory_count; i++)
	{
		myhistoryarr[i]=NULL;
	}
	//myhistory_count-=2;
	printf("Successfully deleted %d histories.\n", (myhistory_count-1));
	myhistory_count=0;
	return 1;
}

//for changing history
int changeDirectory(char dir[32]){
	char *d;
	d = malloc(sizeof(char)*32);
	if(d == NULL){
		printf("Error. Memory Allocation failed. \n");
	}
	printf("Previous directory: %s\n ", getcwd(d, 32));
	chdir(dir);
	printf("Directory after cd command: %s\n", getcwd(d, 32));
	d= NULL;


	return 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
//getAliascommand and getAliasname goes through the user input returning alias_name and alias_cmd if the user enters alias alias_name = 'alias_cmd'
char* getAliascommand(char str[256]);
char *getAliasname(char str[256]);
//function to print error message
void inputErr(); 
//executes alias command
void executeAliasCmd(int index1);
//delete alias from alias -r command
int deletealias(int index1, int maxindx); 
char aliasname[256][15]; //array to store alias_name 
char aliascommand[256][15]; //array to store alias_cmd 
//delimiter for strtok(). 
int index1 = 0; 
//delimiter
char delimit[]="='";
int alias(char aliasComd[256], int idx)
{
	int a = 0;   //index for arrays to store alias_name and alias_cmd
	char str[256]; //store input
	char* cmdptr; //ptr to store value returned by getAliascommand(); 
	char *nameptr; //ptr to store value returned by getAliasname()
	char *cmd[32]; //for strtok() function to store string tokens
	char *deleteptr; //pointer to store the value of alias to be deleted
	int cmdindx; //index of alias to be deleted 

	//copy the command to str
	strcpy(str, aliasComd);
	//boolean value for checking user input
	int booln1 = 0, booln2 = 0, foundbool = 0;
	
	//if user inputs alias -c then all the aliases will be deleted
	if(strcmp(str, "alias -c")==0){
		for(int i = 0; i<index1; i++ ){
			strcpy(aliasname[i], "NULL"); 
			strcpy(aliascommand[i], "NULL"); 
		}
		printf("All aliases deleted. \n");
		index1 = 0;
		return 1;  
	}

	//if alias -r aliasname is entered, this function finds aliasname and deletes ot
	if(strncmp(str, "alias -r ", 9)==0){
		strcpy(delimit, " "); 
		deleteptr = malloc (sizeof (char) * 15);
		if(deleteptr == NULL){
			printf("Error. Memory allocation failed. \n"); 
		}
		deleteptr = getAliascommand(str); 
		printf("%s \n", deleteptr); 
		for(int c=0; c<index1; c++){
			if(strcmp(aliasname[c], deleteptr )==0){
				foundbool = 1; 
				cmdindx = c; 
				index1 = deletealias(cmdindx, index1); 
				printf("The alias has been deleted.\n");
				deleteptr = NULL; 
				strcpy(delimit, "='");
				return 1; 
			}
		}  
		if (foundbool==0){
			printf("The alias you entered does not exist. \n");
		}
	} 

	//if only alias is entered, this function will output every aliases
	if((strlen(str) == 5) && (strcmp(str, "alias") == 0)){
		if (index1 == 0){
			printf("User has not created any aliases. Create atleast one alias before calling $alias command. \n"); 
			return 1; 
		}
		for (int c=0; c<index1; c++){
			printf("%d. alias name: %s    alias command: %s. \n", (c+1), aliasname [c] ,aliascommand[c]);
			if (c==index1-1){
				return 1; 
			}
		}  
	}

	//if input is not correct
	if(strncmp(str, "alias ", 6) != 0){
		inputErr(); 
		return 1;

	}

	//checking for = in the input
	int indx = strlen(str); 
	for(int i = 0; i<indx; i++){
		if(str[i] =='='){
			booln1 = 1; 
		}
	}

	int noQuotation = 0; //keeping track of number of single quotation marks
	for(int i = 0; i<indx; i++){
		if(str[i] == '\''){
			noQuotation++; 
		}
	}
	if(noQuotation == 2){
		booln2 = 1; 
	}

	//Print error message if input is not valid
	if (booln1 == 0 || booln2 == 0 ){
		inputErr(); 
	}
	//store alias command in an array if the command is not previously entered
	cmdptr = malloc (sizeof (char) * 15);
	if (cmdptr == NULL){
		printf("Error. Memory allocation failed. \n");
	}
	cmdptr = getAliascommand(str);
	for(int i =0; i<index1; i++){
		if (strcmp(aliascommand[i], cmdptr) == 0){
			printf("%s already exists. \n", cmdptr);
			printf("%d. alias name: %s    alias command: %s. \n", (i+1), aliasname [i] ,aliascommand[i]);
			return 1;
		} 
	}
	strcpy(aliascommand[index1], cmdptr); 
	
	//store alias name in an array if the name is not previously entered
	nameptr = malloc (sizeof (char) * 15);
	if (nameptr == NULL){
		printf("Error. Memory allocation failed. \n");
	}
	nameptr = getAliasname(str);
	for(int i =0; i<index1; i++){
		if (strcmp(aliasname[i], nameptr) == 0){
			printf("%s already exists. \n", nameptr);
			printf("%d. alias name: %s    alias command: %s. \n", (i+1), aliasname [i] ,aliascommand[i]);
			return 1; 
		} 
	}
	strcpy(aliasname[index1], nameptr); 
	
	//increase index
	index1++;  

	//deallocate pointer
	cmdptr = NULL; 
	nameptr = NULL;
       	return 1; 	
}
//function that returns alias command
char* getAliascommand(char str[256]){
	char *aliascmd; 
	char *cmd[32]; 
	int i = 0; 
	aliascmd = malloc (sizeof (char) * 15);
	if (aliascmd == NULL){
		printf("Error. Memory allocation failed. \n");
	}
	char* token = strtok(str, delimit);
	while (token != NULL) {
		cmd[i] = token;
		i++;
		token = strtok(NULL, delimit);
	}
	aliascmd = cmd[i-1]; 
	return aliascmd;  
}

//function that returns alias name
char *getAliasname(char str[256]){
	char *aliasname; 
	char *cmd[32];
	char *cmd2[32];  
	int i = 0, j =0; 
	aliasname = malloc (sizeof (char) * 15);
	if(aliasname == NULL){
		printf("Error. Memory allocation failed. \n");
	}
	char* token = strtok(str, delimit);
	while (token != NULL) {
		cmd[i] = token;
		i++;
		token = strtok(NULL, delimit);
	}

	char *token2 = strtok(cmd[0], " ");
	while (token2 != NULL) {
		cmd2[j] = token2; 
		j++;
		token2 = strtok(NULL, " ");
	}
	aliasname = cmd2[j-1];
	if(strcmp(aliasname, "alias")==0){
		inputErr();  
	}
	return aliasname;  
} 

//outputs correct format of input required 
void inputErr(){
	printf("Error! Look at the accepted input formats listed below: \n");
	printf("1. Use alias alias_name = 'command' if you want to enter a new alias. \n");
	printf("2. Use alias -r alias_name if you want to delete an alias. \n");
	printf("3. Use alias -c if you want to delete all aliases. \n");
	printf("4. Use alias if you want to list all the aliases.\n");
	printf("Note: Space before or after a command is not accepted. \n");
}
int deletealias(int index1, int maxindx){
	for(int i = index1; i<=maxindx-1; i++ ){
		if (i == maxindx){
			strcpy(aliasname[i], "NULL");  
			strcpy(aliascommand[i], "NULL");  
		}
		else{
			strcpy(aliasname[i], aliasname[i+1]);  
			strcpy(aliascommand[i], aliascommand[i+1]);
		}
	}
	return (maxindx-1);
}

//execute command thats set by user
void executeAliasCmd(int index1){
	char *str3; 
	char *cmd[15];
	int a = 0; 
	str3 = malloc(sizeof (char) * 15);
	if (str3 == NULL){ 
		printf("Error! Memory allocation failed.\n");
	}
	strcpy(str3, aliascommand[index1]);
	char *token3 = strtok(str3, " ");
	while (token3 != NULL) {
		cmd[a] = token3;

		a++;
		token3 = strtok(NULL, " ");
	}
	cmd[a] = NULL;
	
	//deallocating pointer
	str3 = NULL; 

	pid_t pid;
	pid=fork();
	//child process
	if (pid == 0){

		execvp(cmd[0], cmd);
		printf("%s command in alias '%s' is invalid.\n", cmd[0], aliasname[index1]);
	}

	else  //parent process
	{
		wait( (int *)0 );
	}
}

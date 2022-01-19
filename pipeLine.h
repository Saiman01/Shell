/*
 * PipeLine.h
 *
 *  Created on: April 25, 2020
 *      Author: Mukesh Yadav
 *    CSCE 3600 
 *  Shell extended with PipeLining 
 *  It mainly connects the standard output of command1 with the 
 *  input of command 2 to the standard input of command 3.
 */
//Library functions called
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>

// function to get the command number
int get_number_of_commands(char **cmd)
{
    int len = 0;
    for (len = 0; cmd[len] != NULL; len++)
        ;
    return len;
}

/*
pipe handler
this function is called from the child proccess code
returns 0 if no pipes, 1 if we handled pipes
*/
int pipes_handler(char **cmd)
{
    int cnt = 0;
    for (int i = 0; cmd[i] != NULL; i++)
    {
        if (strstr(cmd[i], "|") != NULL)
        {
            cnt++;
        }
    }
    if (cnt == 0)
    {
        // no pipes found, so do nothing
        return 0;
    }
    /*--- pipes found
    split the commands to pass  them to the pipe
   */

    char ***pipes_cmd = (char ***)malloc(sizeof(char **) * (cnt + 1));
    if (pipes_cmd == NULL)
    {
        fprintf(stderr, "Error Malloc, exiting\n");
        exit(0);
    }

    pipes_cmd[0] = cmd;
    int n = 1;
    int len = get_number_of_commands(cmd);
    for (int i = 1; i < len; i++)
    {
        // this means this is a spliting point
        if (cmd[i][0] == '|')
        {
            // cut the line into 2 lines
            pipes_cmd[n] = cmd + i + 1;
            cmd[i] = NULL;
        }
    }

    //-----------
    // run commands through pipes
    int fd[2];
    pid_t pid;
    int fdd = 0;
    for (;*pipes_cmd != NULL;)
    {
        pipe(fd);

        if ((pid = fork()) == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {

            dup2(fdd, 0);
            if (*(pipes_cmd + 1) != NULL)
            {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            // run the excec comman
            if (execvp(*pipes_cmd[0], *pipes_cmd) < 0)
            {
                fprintf(stderr, "The command %s does not exist or could not be executed.\n", *pipes_cmd[0]);
            }
            exit(1);
        }
        else
        {
            wait(NULL);
            close(fd[1]);
            fdd = fd[0];
            pipes_cmd++;
        }
    }
     return 1;
}
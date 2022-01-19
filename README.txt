Major Project 2
Group 6

Members and responsibilities: 
1. Saiman Sigdel-Responsible for (1)cd commands and (4)Alias Support
2. Nabin Bhatta-Responsible for (4)myhistory command and (3)Signal Control
3. Mukesh Yadav-Responsible for (2)exit command and (2)Pipelining
4. Kshitiz Thapa-Responsible for (3)path command and (1)Redirection

Roles of each members: 
1. Saiman Sigdel - check for bugs and make sure the program runs smoothly 
2. Kshitiz Thapa - make sure the code is oraganized, readable and well commented
3. Nabin Bhatta - make sure the team deadline is met by everyone in the group
4. Mukesh Yadav - keep track of progress made by the team and keep track of whats left to do

In this project, we have implemented the shell engine as the group component where shell should
read the line from standard input (i.e. interactive mode) or file (i.e. batch mode). After that, 
it parses the line with command and arguments, executes the commands with arguments, and then prompts
for more inputs from the user.

1. Interactive Mode will display a prompt and allow the user to type in a comand at the prompt.
   
2. Batch Mode is slightly different than that of interactive mode where we do not have to display any 
   prompt, but we should echo each line that it reads from the batch file back to the user before 
   executing it.

The project is divided in to two portions where group members are responsible for their individual portions
as well the group effort is required to complete the organization and proper arrangement of all the individual 
portions. It is also the responsibilities of the group to test the code with different inputs and make sure 
that everything is arranged and displays the correct output.

Design of The Project:
Project is divided into 3 files.
    1. pipeLine.h: It is designed for handling the pipelining. 

    2. alias.h: It is designed for handling the alias commands.
 
    3. newshell.c: It is designed for connecting everything together.


When the user compile the program using makefile then it prompts MYNEWSHELL> .Here, users can ask the system to run the 
various commands. After reading the reading an arbitrary length line from the std in or from the file, it parses that line 
into tokens. If there is any ; or \n which is an empty line problem, the program deals with these problems as well.
The most challanging parts are MyHistory and Alias where our group members have to struggle a lot to find the correct version. 

Direction to Compile and Run the project:
To compile: make
To Run: ./newshell
To Clean: make clean
Any known bugs/errors:
1. Alias command and myhistory cannot be combined with other commands.
2. Execvp failed message is shown for any command that is not executed by execvp. For example if we write alias ll = 'ls -al', alias
command is executed but execvp command still runs on the input showing execvp failed.
3. (Mukesh Yadav)I wrote the code required to for pipeline command in pipeline.h but I could not add that in newshell.c . 

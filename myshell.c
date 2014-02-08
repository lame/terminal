/*MyShell.c*/
/*Project 1 by Ryan Kuhl*/
/*FSUID: RKK09C*/
/*COP4610*/
/*01/22/2014*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define MAX_LEN 1024
#define MAX_LINE 80

void date();
void cd(char* commands[], int);
void echo(char* commands[], int);
void ls(char* commands[], int);
void cat(char* commands[], int);
void more(char* commands[], int);
void history(char* commands[], int);

int main(int argc, char* argv[])
{	
	const char* whitespace = " \n\r\f\t\v";
	char* commands[9];
	char maxLine = MAX_LINE;
	char str[MAX_LINE];
	char cwd[MAX_LEN];
	char* token;
	char* usr;

	while(1){
		//printf("$");
		usr = (char*)getenv("USER");
		if(usr!=NULL && getcwd(cwd, sizeof(cwd)) !=NULL)
		{
			int n = 0;
			printf("%s@myshell:%s>",usr,cwd);
			fgets(str, 81, stdin);
			token = strtok(str, whitespace);
			//commands[0] = token;							//redundant
			//printf("Token: %s\n", token);					//For deubgging purposes
			if(token == NULL) continue;
			while(token != NULL)
			{
				commands[n] = token;
				//printf("%s\n", commands[n]);
				token = strtok(NULL, whitespace);
			 	++n;
			}
			printf("N number is: %d\n", n);
            		if(strcmp(commands[0],"exit") == 0)
				return 0;
			else if(strcmp(commands[0],"cd") == 0)
				cd(commands, n);
			else if(strcmp(commands[0], "echo") == 0)
				echo(commands, n);
			else if(strcmp(commands[0], "date") == 0)
				date();
			else if(strcmp(commands[0], "ls") == 0)
				ls(commands, n);
			else if(strcmp(commands[0], "cat") == 0)
				cat(commands, n);
			else if(strcmp(commands[0], "more") == 0)
				more(commands, n);
			else if(strcmp(commands[0], "history") == 0)
				history(commands, n);
			else
			{
				printf("Unexpected command\n");
				continue;
			}					              
		}
		else
			return 1;
	}
}

void date()
{
	char* curr_time;	
	time_t sysTime;
	sysTime = time(NULL);
	curr_time = ctime(&sysTime);

	(void)printf("%s\n", curr_time);
};
void cd(char* commands[], int count)
{	
	int ret;
	int n = 1;
	
	if(commands[1] == NULL)
		chdir("/home");
	else if(commands[n][0] != '/'){
		if(strcmp(commands[1], "..") == 0)
			chdir(commands[1]);
		else
		{
			char blank[MAX_LINE] = "/";
			strcat(blank, commands[1]);
			commands[1] = blank;
			chdir(commands[1]);
			printf("Commands[1]: %s\n", commands[1]);
		}
	} 
	else
		ret = chdir(commands[1]);
	if(count > 2)
		printf("Error: Too many arguments");
	//else if(token == NULL)
	//{
	//	ret = chdir("/home");
	//}	*/
};

void echo(char* commands[], int count)
{
	int n = 1;
	char* envOut;
	//printf("commands[1]+1: %s\n", commands[1] +1);

	for(n; n<count; ++n)
	{
		if(commands[n] == NULL)
			break;
		if(commands[n][0] == '$')
		{
			//printf("commands[%d]+1: %s\n",n,commands[n] +1);
			envOut = (char*)getenv(commands[n]+1);
			printf("%s ", envOut);

		}
		else
			printf("%s ", commands[n]);
	}
	printf("\n");
};
void ls(char* commands[], int count)
{
	const char* colon = ":";
	const char* PATH = "PATH";
	char* token;
	char buffer[1000];

	strcpy(buffer, (char*)getenv(PATH));
	printf("buffer is: %s", buffer);
	//tokenize buffer on colon, search dirs for command, create child process
};
void cat(char* commands[], int count)
{

};
void more(char* commands[], int count)
{

};
void history(char* commands[], int count)
{

};

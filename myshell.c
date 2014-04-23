/********************************************
    
    Original code by Matt Porter
    Edited by Dannielle Anderson
    COP4610
    Project 1 Example Solution
    Spring 2014
    
    A simple UNIX shell that supports some
    built-in commands, external commands, and
    file redirection.

*********************************************/

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*

    Controls how many arguments and bytes per command
    that the shell will accept.
*/
#define MAX_COMMAND_ARGS 9
#define MAX_COMMAND_SIZE 80
#define MAX_HISTORY_ENTRIES 100
#define MAX_NUM 100

/* Structure to hold command information. */

    typedef struct
    {
      int   argc;
      int   amp;
      char* argv[MAX_COMMAND_ARGS + 1];
      char* outputFile;
      char* inputFile;
      char* history[MAX_HISTORY_ENTRIES + 1];
      size_t size;

    } Command;

    typedef struct
    {
      int JID;
      int PID; 
      int argc;
      char* argv[MAX_COMMAND_ARGS + 1];
      int Done;
      int Kill;
    }Jobs;

    int SIZE = 1;
    Jobs jobArray[MAX_NUM];

/* Prints the command prompt. */

    void PrintPrompt()
    {
      char* buf = get_current_dir_name();
      printf("%s@myshell:%s>", getenv("USER"), buf);
      free(buf);
    }

/*
    Decides if the supplied argument is a regular file.
    Returns 1 for success, 0 for failure.
*/

    int IsFile(const char* file)
    {

      struct stat info;

      return (!stat(file, &info) && S_ISREG(info.st_mode));

    }

/* Maps stdout and / or stdin to a file. */

    void FileRedirect(const Command* command)
    {

  /* First, we check for a file to map the standard out to. */

      if (command->outputFile)
      {

        int out = open(command->outputFile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

        if (out == -1)
        {

          printf("%s: No such file or directory.\n", command->outputFile);
          exit(EXIT_FAILURE);

        }

        dup2(out, 1);
        close(out);

      }

  /* Next, we check for a file to map the standard in to. */

      if (command->inputFile)
      {

        int in = open(command->inputFile, O_RDONLY);

        if (in == -1)
        {

          printf("%s: No such file or directory.\n", command->inputFile);
          exit(EXIT_FAILURE);

        }

        dup2(in, 0);
        close(in);

      }

    }

/* Changes the current working directory. */

    void Cd(const Command* command)
    {

      char* cwd;

      if (command->argc > 2)
        printf("cd: Too many arguments.\n");

      else
      {

        if (command->argc == 1)
          cwd = getenv("HOME");

        else
          cwd = command->argv[1];

        if (chdir(cwd))
          printf("%s: No such file or directory.\n", cwd);

      }

    }

/* Writes to the screen what the user inputs. */

    void Echo(const Command* command)
    {

      for (int  i = 1; i < command->argc; ++i)
        printf("%s ", command->argv[i]);

      printf("\n");

    }

/* Executes an external command. */

    void ExecuteExternal(const char* fullPath, const Command* command)
    {

  /* First, we create a new child process. */
      Jobs jerb;
      pid_t pid = fork();

  /* Process creation failure. */
      if(!command->amp){
        if (pid < 0)
          fprintf(stderr, "* ExecuteExternal(): fork() failed.\n");

      /* Child process. */

        else if (pid == 0)
        {

          /* Redirect if necessary. */

          FileRedirect(command);

          /* Finally, execute the external command. */

          execv(fullPath, command->argv);
          fprintf(stderr, "* ExecuteExternal(): unable to run command.\n");
          exit(EXIT_FAILURE);

        }

      /* Parent process simply waits. */

        else
          waitpid(pid, 0, 0);
      }
      else{
       if (pid < 0)
        fprintf(stderr, "* ExecuteExternal(): fork() failed.\n");

      if(pid == 0)
      {
              /* Redirect if necessary. */
        FileRedirect(command);
              /* Finally, execute the external command. */
        execv(fullPath, command->argv);
        fprintf(stderr, "* ExecuteExternal(): unable to run command.\n");
        exit(EXIT_FAILURE);
      }
    }
    printf("[%d] %d\n", SIZE, pid);
    ++SIZE;
    jerb.JID = SIZE;
    jerb.PID = (int)pid;
    jerb.argc = command->argc;
    jerb.Done = 0;
    jerb.Kill = 0;

    for(int i = 0; i < MAX_COMMAND_ARGS; ++i)
    {
      if(command->argv[i] != NULL)
      {
        jerb.argv[i] = strdup(command->argv[i]);
      }
      else
        break;
    }

    jobArray[SIZE] = jerb;    //add information to jobArray
  }

  void FindExternal(const Command* command)
  {

    char* paths;
    char* pathsCopy;
    char* fullPath;
    char* tokenPtr;

  /* If the command has a / in it, run it directly. */

    if (strchr(command->argv[0], '/'))
    {

      if (IsFile(command->argv[0]))
        ExecuteExternal(command->argv[0], command);

      else
        printf("%s: Command not found.\n", command->argv[0]);

    }

  /* Otherwise, search for the command. */

    else
    {

      /* First, get the paths and make a copy of them. */

      paths = getenv("PATH");

      if (paths)
        pathsCopy = (char*)malloc(strlen(paths) + 1);

      else
      {

        fprintf(stderr, "* FindExternal(): unable to get paths.\n");
        return;

      }

      if (pathsCopy)
        strcpy(pathsCopy, paths);

      else
      {

        fprintf(stderr, "* FindExternal(): unable to copy paths.\n");
        return;

      }

      /* Second, tokenize the path, assuming a : delimiter. */

      tokenPtr = strtok(pathsCopy, ":");

      while (tokenPtr)
      {

          /* Make room for the path, command, null character, and slash. */

        fullPath = (char*)malloc(strlen(tokenPtr) + strlen(command->argv[0]) + 2);

        if (!fullPath)
        {

          fprintf(stderr, "* FindExternal(): unable to create full path.\n");
          return;

        }

          /* Create full path to command. */

        strcpy(fullPath, tokenPtr);
        strcat(fullPath, "/");
        strcat(fullPath, command->argv[0]);

          /* If the path exists, run the command. */

        if (IsFile(fullPath))
        {

          ExecuteExternal(fullPath, command);
          free(fullPath);
          free(pathsCopy);
          return;

        }

        free(fullPath);
        tokenPtr = strtok(0, ":");

      }

      free(pathsCopy);

      /* Command isn't found in any path if we get here. */

      printf("%s: Command not found.\n", command->argv[0]);

    }

  }

  void PrintJobs()
  {
    for(int i = 1; i < SIZE+1; ++i)
    {
    //Add extra for loop for arg[1]
    //if(jobArray[i]->argv[])
      if(jobArray[i].JID > 0)
        printf("[%d] %d %s\n", jobArray[i].JID, jobArray[i].PID, jobArray[i].argv[0]);
    }
    for(int i = 1; i < SIZE+1; ++i)
    {
      if(jobArray[i].Done == 1 || jobArray[i].Kill == 1)
      {
        for(int j = i; j < SIZE; ++j)
        {
          Jobs temp;
          temp = jobArray[i+1];
          jobArray[i] = temp;
        }
      } 
      --SIZE;

    }
  }

/*

    Separates command tokens based on whitespace and sets up
    the command structure.  Returns 1 for success, 0 for failure.

*/

    int ParseCommand(char* buffer, Command* command)
    {

      char* tokens[MAX_COMMAND_ARGS];
      char* whitespace = " \n\r\f\t\v";
      int tokenNum = 0;

  /* Initialize the command. */
      command->amp = 0;
      command->argc = 0;
      command->outputFile = NULL;
      command->inputFile = NULL;

  /* Next, tokenize the command based on whitespace. */

      tokens[tokenNum] = strtok(buffer, whitespace);

      while (tokens[tokenNum])
      {

        ++tokenNum;

        if (tokenNum == MAX_COMMAND_ARGS)
          break;

        tokens[tokenNum] = strtok(0, whitespace);

      }

  /* Now, setup the command structure. */

      for (int i = 0; i < tokenNum; ++i)
      {

      /*

            First, look for special characters indicating
            redirect files or environment variables.

      */

            if (!strcmp(tokens[i], ">"))
              command->outputFile = tokens[++i];

            else if (!strcmp(tokens[i], "<"))
              command->inputFile = tokens[++i];

            else if(!strcmp(tokens[i], "&"))
              command->amp = 1;

            else if (tokens[i][0] == '$')
            {

              char* var = getenv((const char*)&tokens[i][1]);

              if (!var)
              {

                printf("%s: Undefined variable.\n", tokens[i]);
                return 0;

              }

              else
              {

                command->argv[command->argc] = var;
                ++(command->argc);

              }

            }

      /* Otherwise, assume a normal command argument. */

            else
            {

              command->argv[command->argc] = tokens[i];
              ++(command->argc);

            }

          }

          command->argv[command->argc] = 0;

          return 1;

        }
/*  
    Saves the commands to command history
*/
    void History(const Command* cmd)
    {
      for (size_t i = 0; i < (cmd->size); ++i)
      { 
        printf("%s\n", (*cmd).history[i]);
      }
    }
/*
    Runs the specified command.
    Returns 1 for success, 0 for exit.
*/
    int RunCommand(const Command* command)
    {

      if (command->argc != 0)
      {

        if (!strcmp(command->argv[0], "cd"))
          Cd(command);

        else if (!strcmp(command->argv[0], "echo"))
          Echo(command);

        else if (!strcmp(command->argv[0], "history"))
          History(command);

        else if(!strcmp(command->argv[0], "Kill"))
        {
          for(int i = 1; i <SIZE+1; ++i){
            if(atoi(command->argv[1]) == jobArray[i].PID);
            {
              if(jobArray[i].argv[1] != NULL)
                printf("Terminated %s %s\n", jobArray[i].argv[0], jobArray[i].argv[1]);
              else
                printf("Terminated %s\n", jobArray[i].argv[0]);
            }
          }
          FindExternal(command);
        }

        else if(!strcmp(command->argv[0], "jobs"))
          PrintJobs();

        else if (!strcmp(command->argv[0], "exit"))
          return 0;

        else
          FindExternal(command);

      }

      return 1;

    }

/* Begins the main program loop. */

    int main()
    {

      char buffer[MAX_COMMAND_SIZE + 1];
      Command command;
      command.size = 0;
      pid_t id;

      while (1)
      {

      /* First, prompt the user to enter a command. */

        PrintPrompt();

        if (!fgets(buffer, MAX_COMMAND_SIZE, stdin))
        {

          fprintf(stderr, "* fgets(): unable to gather command input.\n");
          continue;

        } 

        buffer[strlen(buffer) - 1] = 0;

  /* Then store the command to history */

        command.history[command.size++] = strdup(buffer);

  /* Next, setup the command by parsing through the buffer. */

        if (!ParseCommand(buffer, &command))
          continue;

  /* Finally, run the command. */

        if (!RunCommand(&command))
          return 0;
        do{
         id = waitpid(-1, NULL, WNOHANG);
         if (id > 0)
           for(int i = 1; i < SIZE+1; ++i)
           {
            if(id == jobArray[i].PID)
            {
              jobArray[i].Done = 1;
            // if(jobArray[i].argv[1] != NULL)
            //     printf("[%d] Done %s %s\n", jobArray[i].JID, jobArray[i].argv[0], jobArray[i].argv[1] );
            // else
            //   printf("[%d] Done %s\n", jobArray[i].JID, jobArray[i].argv[0]);
            }
          }
          if(id > 0)
            printf("[%d] Done\n", id);
        } while(id > 0);

      }

      return 0;

    }

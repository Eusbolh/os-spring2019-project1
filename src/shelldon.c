/*
 * shelldon interface program

KUSIS ID: PARTNER NAME: Alper Toygar
KUSIS ID: PARTNER NAME: Cüneyt Emre Yavuz

 */

#include "birdakika.h"
#include "codesearch.h"
#include "consts.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_HISTORY    1000

#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))  // Given array x, returns the number of elements in the array

int parseCommand(char inputBuffer[], char *args[],int *background);

void concatArray(char* dest, char** arr, int beginIndex);

int main(void)
{
  char inputBuffer[MAX_LINE]; 	       /* buffer to hold the command entered */
  int background;             	       /* equals 1 if a command is followed by '&' */
  char *args[MAX_LINE/2 + 1];	         /* command line (of 80) has max of 40 arguments */
  pid_t child;            		         /* process id of the child process */
  int status;           		           /* result from execv system call*/
  int shouldrun = 1;
  char history[MAX_HISTORY][MAX_LINE];
  int history_index = 0;

  int oldestchildPID = -1;             /* pid of the process called with 'oldestchild' command */
  int oldestchildMode = -1;            /* function mode for 'oldestchild' command (case 0: first time, case 1: repetition, case 2: new pid) */
		
  while (shouldrun){            		   /* Program terminates normally inside setup */
    background = 0;
		
    shouldrun = parseCommand(inputBuffer,args,&background);       /* get next command */
		
    /*
    if (strncmp(inputBuffer, "exit", 4) == 0)
      shouldrun = 0;                    // Exiting from shelldon
    */

    if (shouldrun) {

      //creating the command from the arguments
      char command[MAX_LINE];
      concatArray(command, args, 0);

      strcpy(history[history_index], command);
      history_index++;      

      if (strncmp(inputBuffer, "exit", 4) == 0)
        shouldrun = 0;

      if(strncmp("cd", command, strlen("cd")) == 0){
        char dir[MAX_LINE];
        concatArray(dir, args, 1);
        chdir(dir);
        continue;
      }

      // oldestchild Command
      if (args[1] != NULL && strncmp("oldestchild", command, 11) == 0) {
        // The case which 'oldestchild' functions is called first time
        int argPID = atoi(args[1]);
        if (oldestchildPID == -1) {
          oldestchildPID = argPID;
          oldestchildMode = 0;
        }
        // Successive call case - PID is same with the last call
        else if (oldestchildPID == argPID) {
          oldestchildMode = 1;
        }
        // Successive call case - PID is different than last call
        else {
          oldestchildPID = argPID;
          oldestchildMode = 2;
        }
      }

      //list for the execv command
      char* comm[4] = {"/bin/bash", "-c", command, NULL};
      pid_t childPID;
      if((childPID = fork()) == -1) {
        perror("Fork Error!\n");
      }
      else if (childPID == 0) {
        if (strncmp(inputBuffer, "exit", 4) == 0) {
          if (oldestchildPID != -1) {
            // unload the kernel module
            char unload_comm[MAX_LINE];
            sprintf(unload_comm, "%s %s", "sudo rmmod ", KERNEL_MODULE);
            char* kern_unload_comm[4] = {"/bin/bash", "-c", unload_comm, NULL};
            execv("/bin/bash", kern_unload_comm);
          }
          break;
        }
        else if(strcmp("history", command) == 0){
          for(int i = 0; i < history_index; i++)
          {
            printf("%s\n", history[i]);
          }
          
          exit(1);          
        }else if (args[1] != NULL && strncmp("codesearch", command, 10) == 0)
        {
          if (strcmp(args[1], "-r") == 0) {
            codesearch(args[2], -1, "");
          }else if (args[2] != NULL && strcmp(args[2], "-f") == 0) {
            codesearch(args[1], 1, args[3]);
          }else{
            codesearch(args[1], 0, "");
          }
        }else if (strncmp("birdakika", command, 9) == 0)
        {
          crontab(command);
        } else if (args[1] != NULL && strncmp("oldestchild", command, 11) == 0) {
          // The case which 'oldestchild' functions is called first time
          if (oldestchildMode == 0) {
            printf("'oldestchild' command is called first time. Kernel module will be loaded with PID: %s.\n", args[1]);
            
            // load the kernel module with argPID
            char load_comm[MAX_LINE];
            sprintf(load_comm, "%s%s%s%s", "sudo insmod out/", KERNEL_MODULE, ".ko processID=", args[1]);
            // printf("%s\n", load_comm);
            char* kern_load_comm[4] = {"/bin/bash", "-c", load_comm, NULL};
            execv("/bin/bash", kern_load_comm);
          }
          // Successive call case - PID is same with the last call
          else if (oldestchildMode == 1) {
            printf("Kernel module is already loaded with PID: %s.\n", args[1]);
          }
          // Successive call case - PID is different than last call
          else if (oldestchildMode == 2) {
            printf("Kernel module is loaded with another pid. It is being removed and kernel module will be loaded again with PID: %s.\n", args[1]);
            
            // unload the kernel module
            char unload_comm[MAX_LINE];
            sprintf(unload_comm, "%s %s", "sudo rmmod ", KERNEL_MODULE);

            // load the kernel module with argPID
            char load_comm[MAX_LINE];
            sprintf(load_comm, "%s%s%s%s", "sudo insmod out/", KERNEL_MODULE, ".ko processID=", args[1]);
            // printf("%s\n", load_comm);
            char master_comm[MAX_LINE];
            sprintf(master_comm, "%s && %s", unload_comm, load_comm);
            char* kern_master_comm[4] = {"/bin/bash", "-c", master_comm, NULL};
            execv("/bin/bash", kern_master_comm);
          }
          else {
            // code shouldnt be reached here
          }
        } else
        {
          //Runs the command if not history
          execv("/bin/bash", comm);
        }

        return 0;
        
      }
      else {

        //If not in background waits execution else doesnt wait and continues
        if(!background){
          wait(NULL);

          if (strncmp("codesearch", command, 10) == 0)
          {
            FILE *fptr;
            if((fptr = fopen(TEMP_FILE_NAME, "r")) == NULL){
                perror("Error opening answer file\n"); 
                return -1;
            }
            int c;
            while((c = getc(fptr)) != EOF){
                printf("%c", c);
            }
            fclose(fptr);
            remove(TEMP_FILE_NAME);
          }
        }
      }

      

      
    }
  }
  return 0;
}

/**
 * The concatArray function bellow takes a destionation string and a string array and 
 * concatenates the elements starting from the beginIndex.
 */
void concatArray(char* dest, char** arr, int beginIndex){
        char answer[MAX_LINE];
        strcpy(answer, arr[beginIndex]);
        int counter = beginIndex + 1;
        while(arr[counter] != NULL){
          char* temp = answer;
          sprintf(answer, "%s %s", temp, arr[counter]);
          counter++;
        }
        strcpy(dest, answer);
}


/** 
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings. 
 */

int parseCommand(char inputBuffer[], char *args[],int *background)
{
  int length,		                       /* # of characters in the command line */
    i,		                             /* loop index for accessing inputBuffer array */
    start,		                         /* index where beginning of next command parameter is */
    ct,	                               /* index of where to place the next parameter into args[] */
    command_number;	                   /* index of requested command number */
  
  ct = 0;

  /* read what the user enters on the command line */
  do {
    printf("\033[1;32m");
    printf("shelldon> ");
    printf("\033[0m");
    fflush(stdout);
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE); 
  }
  while (inputBuffer[0] == '\n');      /* swallow newline characters */

  /**
   *  0 is the system predefined file descriptor for stdin (standard input),
   *  which is the user's screen in this case. inputBuffer by itself is the
   *  same as &inputBuffer[0], i.e. the starting address of where to store
   *  the command that is read, and length holds the number of characters
   *  read in. inputBuffer is not a null terminated C-string. 
   */    
  start = -1;
  if (length == 0)
    exit(0);                           /* ^d was entered, end of user command stream */
  
  /** 
   * the <control><d> signal interrupted the read system call 
   * if the process is in the read() system call, read returns -1
   * However, if this occurs, errno is set to EINTR. We can check this  value
   * and disregard the -1 value 
   */

  if ( (length < 0) && (errno != EINTR) ) {
    perror("error reading the command");
    exit(-1);                          /* terminate with error code of -1 */
  }
  
  /**
   * Parse the contents of inputBuffer
   */
  
  for (i=0;i<length;i++) { 
    /* examine every character in the inputBuffer */
    
    switch (inputBuffer[i]){
      case ' ':
      case '\t' :                      /* argument separators */
        if(start != -1){
          args[ct] = &inputBuffer[start];    /* set up pointer */
          ct++;
        }
        inputBuffer[i] = '\0';         /* add a null char; make a C string */
        start = -1;
        break;
        
      case '\n':                       /* should be the final char examined */
        if (start != -1){
          args[ct] = &inputBuffer[start];     
          ct++;
        }
        inputBuffer[i] = '\0';
        args[ct] = NULL;               /* no more arguments to this command */
        break;
  
      default :                        /* some other character */
        if (start == -1)
          start = i;
        if (inputBuffer[i] == '&') {
          *background  = 1;
          inputBuffer[i-1] = '\0';
        }
    }                                  /* end of switch */
  }                                    /* end of for */
  
  /**
   * If we get &, don't enter it in the args array
   */
  
  if (*background)
    args[--ct] = NULL;
  
  args[ct] = NULL;                     /* just in case the input line was > 80 */
  
  return 1;
    
}                                      /* end of parseCommand routine */


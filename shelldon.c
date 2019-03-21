/*
 * shelldon interface program

KUSIS ID: PARTNER NAME: Alper Toygar
KUSIS ID: PARTNER NAME: Cüneyt Emre Yavuz

 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define MAX_LINE       80              /* 80 chars per line, per command, should be enough. */
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
	
  int i, upper;
		
  while (shouldrun){            		   /* Program terminates normally inside setup */
    background = 0;
		
    shouldrun = parseCommand(inputBuffer,args,&background);       /* get next command */
		
    if (strncmp(inputBuffer, "exit", 4) == 0)
      shouldrun = 0;                    /* Exiting from shelldon*/

    if (shouldrun) {
      /*
        After reading user input, the steps are 
        (1) Fork a child process using fork()
        (2) the child process will invoke execv()
        (3) if command included &, parent will invoke wait()
       */

      /*
      char* command = args[0];
      int argumentCounter = 1;
      printf("Command: %s\n", command);
      while(args[argumentCounter] != NULL) {
        printf("Argument %d: %s\n", argumentCounter, args[argumentCounter]);
        argumentCounter++;
      }
      */

      //creating the command from the arguments
      char command[MAX_LINE];
      concatArray(command, args, 0);
      
      strcpy(history[history_index], command);
      history_index++;
      
      if(strncmp("cd", command, strlen("cd")) == 0){
        char dir[MAX_LINE];
        concatArray(dir, args, 1);
        chdir(dir);
        continue;
      }

      //list for the execv command
      char* comm[4] = {"/bin/bash", "-c", command, NULL};

      pid_t childPID;
      if((childPID = fork()) == -1) {
        perror("Fork Error!\n");
      }
      else if (childPID == 0) {  
        
        if(strcmp("history", command) == 0){
          for(int i = 0; i < history_index; i++)
          {
            printf("%s\n", history[i]);
          }
          
          exit(1);
          
        }else
        {
        //Runs the command if not history
        execv("/bin/bash", comm);
        }
        
      }
      else {
        //If not in background waits execution else doesnt wait and continues
        if(!background){
          wait(NULL);
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


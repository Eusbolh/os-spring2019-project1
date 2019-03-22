/**
 * 
 * Take the line
 * split the time
 * put in format      dakika saat * * * play komutu filename
 * 
 *
 * /
 */

#include "birdakika.h"
#include "consts.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void crontab(char* command){
    FILE *fptr;
    if((fptr = fopen(CRON_FILE_NAME, "w")) == NULL){
        perror("Error opening file\n"); 
        return;
    }
    int min, hour;
    char song_path[MAX_LINE];
    char real_song_path[MAX_LINE];
    char c[MAX_LINE];
    char dot;

    sscanf(command, "%s %d%c%d %[^\t\n]", c, &hour, &dot, &min, song_path);
    if (strncmp("/", song_path, 1) == 0 || strncmp("~", song_path, 1) == 0) {
        strcpy(real_song_path, song_path);
    }
    else {
        char cwd[MAX_LINE];
        getcwd(cwd, MAX_LINE);
        sprintf(real_song_path, "%s/%s", cwd, song_path);
    }
    
    fprintf(fptr, "%d %d * * * timeout 60 /usr/bin/mpg321 \"%s\"\n", min, hour, real_song_path);
    char cron_command[MAX_LINE];
    sprintf(cron_command, "%s %s", "crontab", CRON_FILE_NAME);
    char* comm[4] = {"/bin/bash", "-c", cron_command, NULL};
    
    fclose(fptr);    

    pid_t childPID;
    if((childPID = fork()) == -1) {
        perror("Fork Error!\n");
    }
    else if (childPID == 0) { 
        execv("/bin/bash", comm);
    }else{
        wait(NULL);
        remove(CRON_FILE_NAME);
    }


}

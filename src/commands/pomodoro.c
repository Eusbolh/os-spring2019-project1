#include "consts.h"
#include "pomodoro.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void pomodoro(int i){
    FILE *fptr;
    if((fptr = fopen(CRON_FILE_NAME, "a")) == NULL){
        perror("Error opening file\n"); 
        return;
    }
    time_t now;

    time(&now);
    struct tm *local = localtime(&now);
    int min = local->tm_min;
    int hour = local->tm_hour;
    int day = local->tm_wday;
    int month = local->tm_mon+1;

    for(int j = 1; j <= i; j++)
    {
        min += 25;
        if(min >= 60){
            min -= 60;
            hour++;
        }
        if(hour >= 24){
            hour = 0;
            day++;
        }
        
        if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
            if(day > 31){
                day = 1;
                month++;
            }
        }else if (month == 2)
        {
            if(day > 28){
                day = 1;
                month++;
            }
        }else{
            if(day > 30){
                day = 1;
                month++;
            }
        }

        if(month > 12){
            month = 1;
        }     

        fprintf(fptr,"%d %d * %d %d timeout 5 /usr/bin/mpg321 \"/home/alpertoygar/Projects/Comp304/Project1/os-spring2019-project1/res/Panic At The Disco ‒ High Hopes (Lyrics).mp3\"\n", min, hour, month, day);

        if(j % 4 == 0){
            min += 15;
        }else{
            min += 5;
        }
        if(min >= 60){
            min -= 60;
            hour++;
        }
        if(hour >= 24){
            hour = 0;
            day++;
        }
        
        if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
            if(day > 31){
                day = 1;
                month++;
            }
        }else if (month == 2)
        {
            if(day > 28){
                day = 1;
                month++;
            }
        }else{
            if(day > 30){
                day = 1;
                month++;
            }
        }

        if(month > 12){
            month = 1;
        }        
        fprintf(fptr,"%d %d * %d %d timeout 5 /usr/bin/mpg321 \"/home/alpertoygar/Projects/Comp304/Project1/os-spring2019-project1/res/Panic At The Disco ‒ High Hopes (Lyrics).mp3\"\n", min, hour, month, day);
        
    }
    fclose(fptr);

    char cron_command[MAX_LINE];
    sprintf(cron_command, "%s %s", "crontab", CRON_FILE_NAME);
    char* comm[4] = {"/bin/bash", "-c", cron_command, NULL};
    execv("/bin/bash", comm);
}

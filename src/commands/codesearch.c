#include "codesearch.h"
#include "consts.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void codesearch_files_only(char* str);
void codesearch_recursive(char* dir_name, char* keyword);
void codesearch_one_file(char* file_name, char* keyword);


int codesearch(char* keyword, int mode, char* file_name){
    if(mode == 0){
        codesearch_files_only(keyword);
        return 1;
    }else if (mode < 0)
    {
        codesearch_recursive(".", keyword);
        return 1;
    }else{
        codesearch_one_file(file_name, keyword);
        return 1;
    }
}

void codesearch_files_only(char* keyword){    

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while((dir = readdir(d)) != NULL){                
            if(strcmp(dir->d_name, TEMP_FILE_NAME) != 0){
                char path[MAX_PATH_LENGTH];
                sprintf(path, "./%s", dir->d_name);
                codesearch_one_file(path, keyword); 
            }  
        }
        closedir(d);        
    }
}

void codesearch_recursive(char* dir_name, char* keyword){
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d) {
        while((dir = readdir(d)) != NULL){
            if(dir->d_type == DT_DIR && strncmp(dir->d_name, ".", 1) != 0){
                codesearch_recursive(dir->d_name, keyword);
            }else{     
                if(strcmp(dir->d_name, TEMP_FILE_NAME) != 0){
                    char path[MAX_PATH_LENGTH];
                    sprintf(path, "%s/%s", dir_name, dir->d_name);
                    codesearch_one_file(path, keyword); 
                }               
            }
        }
    closedir(d);        
    }
}

void codesearch_one_file(char* file_name, char* keyword){
    FILE *fptr;
    if((fptr = fopen(file_name, "r")) == NULL){
        perror("Error opening file\n"); 
        return;
    }
    FILE *ansptr;
    if((ansptr = fopen(TEMP_FILE_NAME, "a")) == NULL){
        perror("Error opening file\n"); 
        fclose(fptr);
        return;
    }
    size_t len = 0;
    char* line = NULL;
    size_t read;
    int num = 1;

    while ((read = getline(&line, &len, fptr)) != -1) {
        if(strstr(line, keyword)){
            fprintf(ansptr, "%d: %s -> %s", num, file_name, line);
        }
        num++;
    }

    fclose(fptr);   
    fclose(ansptr); 
}


/*
Adarsh Kurian
CS337
Program 7
KurianShell 
11/29/16
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#define MAX_PATH_LENGTH 10000

static char * path;
static char * directive;
static char ** args;
static char * stringIn;

void readDirective(char * str, char * command, char ** args);

void multiFile(char** args);

void sigHandle(int g){
    printf("\nExiting...\n");
    fflush(stdout);
    free(directive);
    free(args);
    free(stringIn);
    exit(0);
}

int main(int argc, char ** argv){
    if(argc == 2){
        execlp("sh","sh", argv[1],NULL);
        return 0;
    }

    printf("\nKurian Shell v3.0 [built: Wed Feb 01 02:01:34 2017]\n\n");

    signal(SIGINT, sigHandle);

    path = (char *)malloc(sizeof(char) * MAX_PATH_LENGTH);
    directive = (char *)malloc(sizeof(char) * 10000);
    args = (char **)malloc(sizeof(char *) * 10000);
    stringIn = (char *)malloc(sizeof(char) * 10000);

    getcwd(path, MAX_PATH_LENGTH);
    printf("\n%s ?", path);
    fflush(stdout);

    while(1){
        if(read(fileno(stdin), stringIn, 10000) <= 1){
            fflush(stdin);
            getcwd(path, MAX_PATH_LENGTH);
            printf("\n%s ?", path);
            fflush(stdout);
            continue;
        }
        fflush(stdin);
        if(stringIn[0] == ' '){
            getcwd(path, MAX_PATH_LENGTH);
            printf("\n%s ?", path);
            fflush(stdout);
            continue;
        }

        readDirective(stringIn, directive, args);
        if(strcmp(directive, "exit") == 0){
            free(directive);
            free(args);
            free(stringIn);
            break;
        }
        else if(strcmp(directive, "multi") == 0){
            multiFile(args);
        }
        else if(strcmp(directive, "cd") == 0){
            if(args[1]!= NULL){
                if(chdir(args[1]) == -1){
                    printf("\n%s : no such directory\n", args[1]);
                }
            }
            else{
                printf("\ninsufficent arguments for cd: cd <path>\n");
                fflush(stdout);
            }
        }
        else{
            pid_t pid;
            pid = fork();
            
            if(pid == 0){
                execvp(directive, args);
                exit(0);
            }
            else if(pid > 0){
                int status;
                wait(&status);
            }
            else{
                printf("\nBad fork. Exiting\n");
                return 0;
            }
        }
        getcwd(path, MAX_PATH_LENGTH);
        printf("\n%s ?", path);
        fflush(stdout);
        free(directive);
        free(args);
        free(stringIn);
        directive = (char *)malloc(sizeof(char) * 10000);
        args = (char **)malloc(sizeof(char *) * 10000);
        stringIn = (char *)malloc(sizeof(char) * 10000);
    }
    return 0;
}

void readDirective(char * str, char * command, char ** args){
   char * token;

   token = strtok(str, " \n");
   strcpy(command, token);

   args[0] = token;
   int i = 1;
   while(token != NULL){
      token = strtok(NULL, " \n");
      args[i] = token;
      ++i;
   }
}

void multiFile(char ** args){
    if(args[1] != NULL){
        FILE * fin;
        fin = fopen(args[1], "rb");
        if(fin == NULL){
            printf("\nCould not open file\n");
            fflush(stdout);
            return;
        }
        char line[256];
        while (fgets(line, sizeof(line), fin) != 0){
            if(strlen(line) <= 2){
                break;
            }
            printf("\nExecuting: %s\n", line);
            char * directive2 = (char *)malloc(sizeof(char) * 10000);
            char ** args2 = (char **)malloc(sizeof(char *) * 10000);
            readDirective(line, directive2, args2);

            if(strcmp(directive2, "exit") == 0){
                free(directive2);
                free(args2);
                break;
            }
            else if(strcmp(directive2, "cd") == 0){
                if(args2[1]!= NULL){
                    if(chdir(args[1]) == -1){
                        printf("\n%s : no such directory\n", args[1]);
                    }
                }
                else{
                    printf("\ninsufficent arguments for cd: cd <path>\n");
                    fflush(stdout);
                }
            }
            else{
                pid_t pid;
                pid = fork();
            
                if(pid == 0){
                    execvp(directive2, args2);
                    exit(0);
                }       
                else if(pid > 0){
                    int status;
                    wait(&status);
                }
                else{
                    printf("\nBad fork. Exiting\n");
                    return;
                }
            }
            free(directive2);
            free(args2);
        }
        
        fclose(fin);
    }
    else{
        printf("\ninsufficent arguments for multi: multi <path/file_name>\n");
        fflush(stdout);
    }
    return;
}
//
//  main.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "command.h"

int running = 1;
struct Command command;
char* path;
char* history[10];
int history_count = 0;

int main(int argc, const char * argv[]) {
    while (running) {
        path = getcwd(path, 500);
        write(STDOUT_FILENO,"prompt $ ", 15);
        
        char text[1024];
        ssize_t characters_count = read(STDIN_FILENO, text, 1024);
        compile_command(text, characters_count, &command);
        
        int pid = fork();
        
        //child
        if (!pid) {
            dup2(command.in_fd, STDIN_FILENO);
            dup2(command.out_fd, STDOUT_FILENO);
            
            if (command.built_in) {
                if (execvp(command.tokens[0], (char *const *) command.tokens) == -1) {
                    printf("Unknown comand \n");
                }
            }
            else if (command.type == hist) {
                for (int i = 0; i < 10; i++) {
                    if (history[i] == NULL) {
                        break;
                    }
                    printf("%d: ", i + 1);
                    printf("%s \n", history[i]);
                }
            }
            exit(0);
        }
        else {
            wait(NULL);
        }
        
        if (command.type == unknown) {
            printf("Unkonwn comand \n");
        }
    }
    
}

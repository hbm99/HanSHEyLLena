//
//  main.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include <sys/wait.h>
#include "command.h"

int running = 1;
char* path;
struct Command* compiled_command;
struct History* history;

int main(int argc, const char * argv[]) {
    history = malloc(sizeof(struct History*));
    history->count = 0;
    history->start_index = 0;
    while (running) {
        path = getcwd(path, 500);
        write(STDOUT_FILENO,"prompt $ ", 9);
        
        char _text[1024];
        ssize_t characters_count = read(STDIN_FILENO, _text, 1024);
        
        char text[characters_count];
        for (int i = 0; i < characters_count; i++) {
            if (_text[i] == '#') {
                characters_count = i;
                break;
            }
            else {
                text[i] = _text[i];
            }
        }
        
        compiled_command = compile_command(text, characters_count, history);
        
        int pid = fork();
        
        //child
        if (!pid) {
            dup2(compiled_command->in_fd, STDIN_FILENO);
            dup2(compiled_command->out_fd, STDOUT_FILENO);
            
            if (command->type == quit) {
                running = 0;
            }
            else {
                if (compiled_command->built_in) {
                    if (execvp(compiled_command->tokens[0], (char *const *) compiled_command->tokens) == -1) {
                        printf("Unknown comand \n");
                    }
                }
                else if (compiled_command->type == hist) {
                    for (int i = 0; i < 10; i++) {
                        if (history->record[i] == NULL) {
                            break;
                        }
                        printf("%d: ", i + 1);
                        printf("%s \n", history->record[i]);
                    }
                }
                exit(0);
            }
        }
        else {
            wait(NULL);
        }
        
        if (compiled_command->type == unknown) {
            printf("Unkonwn comand \n");
        }
    }
    
}

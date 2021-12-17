//
//  main.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include <sys/wait.h>
#include <signal.h>
#include "command.h"

int running = 1;
char* original_path;
struct History* history;

int main(int argc, const char * argv[]) {
    original_path = getcwd(original_path, 500);
    history = malloc(sizeof(struct History*));
    history->count = 0;
    history->start_index = 0;
    while (running) {
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
        
        compile_command(text, characters_count, history);
        
        int pid = fork();
        
        //child
        if (!pid) {
            dup2(command.in_fd, STDIN_FILENO);
            dup2(command.out_fd, STDOUT_FILENO);
            
            if (command.type == quit) {
                running = 0;
                kill(getppid(), SIGKILL);
            }
            else {
                if (command.built_in) {
                    if (execvp(command.tokens[0], (char *const *) command.tokens) == -1) {
                        printf("Unknown comand \n");
                    }
                }
                else if (command.type == hist) {
                    
                    //Imprimiendo history.txt

                    // Declare the file pointer
                    FILE *filePointer;
                    // Declare the variable for the data to be read from file
                    char line[50] = {0};
                    // Open the existing file history.txt using fopen()
                    // in read mode using "r" attribute
                    filePointer = fopen(strcat(original_path, "/history.txt"), "r");
                    // Check if this filePointer is null
                    // which maybe if the file does not exist
                    if (filePointer == NULL)
                        printf("History file failed to open.\n");
                    else
                    {
                        int line_count = 0;
                        // Read the line from the file
                        // using fgets() method
                        while(fgets(line, 50, filePointer) != NULL)
                        {
                            // Print the line
                            printf("%d. %s", ++line_count, line);

                            if (line[strlen(line) - 1] != '\n')
                                printf("\n");
                        }
                        // Closing the file using fclose()
                        fclose(filePointer);
                    }
                }
                exit(0);
            }
        }
        else {
            wait(NULL);
        }
        if (command.type == unknown) {
            printf("Unkonwn comand \n");
        }
    }
    return 0;
}

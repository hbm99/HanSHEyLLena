//
//  main.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include <sys/wait.h>
#include <signal.h>
#include "command.h"
#include <signal.h>

int running = 1;
int commands_counter;
struct History history;
struct Command* command_list;
int signal_counter = 1;

void  INThandler(int sig)
{
   
        signal(sig, SIG_IGN);
        if (signal_counter == 2)
        {
            pid_t iPid = getpid(); 
            kill(iPid, SIGKILL);
        }
        else 
        {
            signal_counter++;
            signal(SIGINT, INThandler);
        }      
    
}

//Initializing history
void init_history() {

    history.count = 0;
    history.start_index = 0;
    history.txt_path = (char *)malloc(500);
    history.txt_path = strcat(original_path, "/history.txt");
    
    txtPointer = fopen(history.txt_path, "r");
    
    size_t length;
    for(int i = 0; i < 10; i++)                                                          
    {
        length = 50;
        getline(&history.record[i], &length, txtPointer);
        if (strcmp(history.record[i], "") == 0)
            break;
        history.count++;           
    }
    fclose(txtPointer);
}

char** parse_line() {
        
    char _text[1024];
    ssize_t characters_count = read(STDIN_FILENO, _text, 1024);
    
    // checking if there is a comment (#) or pipes (|) in the line
    char text[characters_count];
    int pipes_counter = 0;
    for (int i = 0; i < characters_count; i++) {
        if (_text[i] == '|') {
            pipes_counter++;
        }
        if (_text[i] == '#') {
            characters_count = i;
            break;
        }
        else {
            text[i] = _text[i];
        }
    }

    char** list_commands_texts = (char**)malloc((pipes_counter + 1) * sizeof(char*));
    for (int i = 0; i < pipes_counter + 1; i++)
        list_commands_texts[i] = (char*)malloc(characters_count);

    int list_commands_texts_index = 0;
    char* accum = (char*)malloc(characters_count);
    for (int i = 0, j = 0; i < characters_count; i++, j++)
    {
        if (text[i] == '|')
        {
            list_commands_texts[list_commands_texts_index] = strcat(accum, "\n");
            j = -1;
            accum = (char*)malloc(characters_count);
            list_commands_texts_index++;
            continue;
        }
        accum[j] = text[i];
    }
    list_commands_texts[list_commands_texts_index] = accum;

    commands_counter = pipes_counter + 1;

    return list_commands_texts;
}

void fill_command_list(char** list_commands_texts) {

    command_list = (struct Command*)malloc(commands_counter * sizeof(struct Command));
    for (int i = 0; i < commands_counter; i++)
    {
        compile_command(list_commands_texts[i], strlen(list_commands_texts[i]), &history);
        command_list[i] = command;
    }
}

int** init_multipipe() {
    
    int** pipeline;
    pipeline = (int**)malloc((commands_counter - 1) * sizeof(int*));
    for (int i = 0; i < commands_counter - 1; i++) {
        pipeline[i] = (int *) malloc(2 * sizeof(int));
        pipe(pipeline[i]);
    }
    return pipeline;
}

int main(int argc, const char * argv[]) {

    signal(SIGINT, INThandler);

    original_path = getcwd(original_path, 500);
    
    init_history();

    while (running) {

        write(STDOUT_FILENO, "prompt $ ", 9);
        
        char** list_commands_texts = parse_line();

        fill_command_list(list_commands_texts);

        int** pipeline = init_multipipe();
        int pipes_counter = commands_counter - 1;

        int command_index = 0;
        for (int i = 0; i < commands_counter; i++)
        {
            struct Command current_command = command_list[i];

            int pid = fork();
         
            switch (pid)
            {
            case 0:
                if (pipes_counter > 0)
                {
                    if (i == 0)
                    {
                        close(pipeline[i][0]);
                        dup2(pipeline[i][1], 1);
                    }
                    else if (i == commands_counter - 1)
                    {
                        close(pipeline[i - 1][1]);
                        dup2(pipeline[i - 1][0], 0);
                        close(pipeline[i - 1][0]);
                    }
                    else
                    {
                        close(pipeline[i][0]);
                        close(pipeline[i - 1][1]);
                        dup2(pipeline[i - 1][0], 0);
                        dup2(pipeline[i][1], 1);
                    }
                }

                dup2(current_command.in_fd, STDIN_FILENO);
                dup2(current_command.out_fd, STDOUT_FILENO);

                if (current_command.type == quit) {
                    running = 0;
                    kill(getppid(), SIGKILL);
                }
                else {
                    if (current_command.built_in) {
                        if (execvp(current_command.tokens[0], (char *const *) current_command.tokens) == -1) {
                            printf("Unknown command \n");
                        }
                    }
                    else if (current_command.type == hist) {
                        
                        char line[50] = {0};
                        
                        txtPointer = fopen(history.txt_path, "r");
                        
                        if (txtPointer == NULL)
                            printf("History file failed to open.\n");
                        else
                        {
                            int line_count = 0;
                            
                            while(fgets(line, 50, txtPointer) != NULL)
                            {
                                printf("%d. %s", ++line_count, line);

                                if (line[strlen(line) - 1] != '\n')
                                    printf("\n");
                            }
                        }
                        fclose(txtPointer);
                    }
                    exit(0);
                }
                break;
            
            default:
                if (i < pipes_counter)
                {
                    close(pipeline[i][1]);
                }
                break;
            }
            if (current_command.type == unknown) {
                printf("Unknown command \n");
            }
            command_index = i;
        }
        for (int i = 0; i <= command_index; i++)
        {
            wait(NULL);
        }
    }

    return 0;
}

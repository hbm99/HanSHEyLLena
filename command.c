//
//  command.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include "command.h"

void init_command(struct Command *command) {
    command->in_fd = STDIN_FILENO;
    command->out_fd = STDOUT_FILENO;
    command->text = "";
    command->type = normal;
    command->built_in = 1;
    command->tokens = calloc(20, sizeof(char*));
    command->text = malloc(300);
}

void tokenize(char *text, size_t characters_count, struct Command *command) {
    strncpy(command->text, text, characters_count - 1);
    
    char* text_clone = malloc(strlen(command->text));
    strcpy(text_clone, command->text);
    char* token = strtok(text_clone, " ");
    command->tokens[0] = token;
    
    for (int i = 1; i < 20; ++i) {
        token = strtok(NULL, " ");
        command->tokens[i] = token;
        if (token == NULL)
            break;
    }
}

void parse_command(struct Command *command) {
    for (int i = 0; i < 20; ++i) {
        char* token = command->tokens[i];
        if (token == NULL)
            break;
        else if(strncmp(token, "<", 2) == 0){
            command->type = cin;
            int file = open(command->tokens[i + 1], O_RDONLY);
            command->in_fd = file;
        }
        else if(strncmp(token, ">>", 2) == 0){
            command->type = ccout;
            int file = open(command->tokens[i + 1], O_APPEND);
            command->out_fd = file;
        }
        else if(strncmp(token, ">", 1) == 0){
            command->type = cout;
            command->tokens[i] = NULL;
            int file = open(command->tokens[i + 1], O_WRONLY, O_TRUNC);
            command->out_fd = file;
        }
        else if(strncmp(token, "cd", 2) == 0){
            command->type = cd;
            command->built_in = 0;
            chdir(command->tokens[i + 1]);
            break;
        }
        else if(strncmp(token, "history", 7) == 0){
            command->type = hist;
            command->built_in = 0;
        }
//        else if(strncmp(token,"!",1) == 0){
//            char* val = strtok(token, "!");
//            if (val == NULL){
//                compile_command(history[history_count - 1], strlen(history[history_count - 1]) + 1);
//                return;
//            }
//            for (int j = history_count; j > 0; --j) {
//                if(strncmp(history[j - 1],val,strlen(val)) == 0){
//                    compile_comand(history[j - 1], strlen(history[j - 1]) + 1);
//                    return;
//                }
//            }
//            long line = strtol(val,NULL,10);
//            if(line > 0 && line <= history_count) {
//                compile_comand(history[line - 1], strlen(history[line - 1]) + 1);
//                return;
//            }
//            comand.type = unknown;
//            comand.built_in = false;
//            return;
//        }
    }
//    try_save_comand();
}

void compile_command(char *text, size_t characters_count, struct Command *command) {
    init_command(command);
    tokenize(text, characters_count, command);
    parse_command(command);
}

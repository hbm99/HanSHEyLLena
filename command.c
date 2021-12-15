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
    
}

void compile_command(char *text, size_t characters_count, struct Command *command) {
    init_command(command);
    tokenize(text, characters_count, command);
    parse_command(command);
}

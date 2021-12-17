//
//  command.c
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#include "command.h"

void save_command(struct History* history) {
    if (command.text[0] == ' ') {
        return;
    }
    
    history->record[(history->start_index + history->count) % 10] = command.text;
    
    if (history->count == 10 )
        history->start_index = (history->start_index + 1) % 10;
    else
        history->count++;
}

void init_command() {
    command.in_fd = STDIN_FILENO;
    command.out_fd = STDOUT_FILENO;
    command.text = "";
    command.type = normal;
    command.built_in = 1;
    command.tokens = calloc(20, sizeof(char*));
    command.text = malloc(1024);
}

void tokenize(char *text, size_t characters_count) {
    strncpy(command.text, text, characters_count - 1);
    
    char* text_clone = malloc(strlen(command.text));
    strcpy(text_clone, command.text);
    char* token = strtok(text_clone, " ");
    command.tokens[0] = token;
    
    for (int i = 1; i < 20; ++i) {
        token = strtok(NULL, " ");
        command.tokens[i] = token;
        if (token == NULL)
            break;
    }
}

void parse_command(struct History* history) {
    for (int i = 0; i < 20; ++i) {
        char* token = command.tokens[i];
        if (token == NULL)
            break;
        else if (strncmp(token, "<", 1) == 0) {
            command.type = cin;
            int file = open(command.tokens[i + 1], O_RDONLY);
            command.in_fd = file;
        }
        else if (strncmp(token, ">>", 2) == 0) {
            command.type = ccout;
            int file = open(command.tokens[i + 1], O_APPEND);
            command.out_fd = file;
        }
        else if (strncmp(token, ">", 1) == 0) {
            command.type = cout;
            command.tokens[i] = NULL;
            int file = open(command.tokens[i + 1], O_WRONLY, O_TRUNC);
            command.out_fd = file;
        }
        else if (strncmp(token, "cd", 2) == 0) {
            command.type = cd;
            command.built_in = 0;
            chdir(command.tokens[i + 1]);
            break;
        }
        else if (strncmp(token, "history", 7) == 0) {
            command.type = hist;
            command.built_in = 0;
        }
        else if (strncmp(token, "again", 5) == 0) {
            char* val = command.tokens[i + 1];
            if ((long)val > 0 && (long)val <= history->count) {
                compile_command(history->record[(history->start_index + (long)val - 1) % 10], strlen(history->record[(history->start_index + (long)val - 1) % 10]) + 1, history);
                return;
            }
            command.type = unknown;
            command.built_in = 0;
            return;
        }
        else if (strncmp(token, "exit", 4) == 0) {
            command.type = quit;
        }
    }
    save_command(history);
}

void compile_command(char *text, size_t characters_count, struct History* history) {
    init_command();
    tokenize(text, characters_count);
    parse_command(history);
}

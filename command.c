#include "command.h"

char* concat(char* str1, char* str2) {
    char* concatenation = (char*)malloc(1024);
    int index = 0;
    while(str1[index] != '\0')
    {
        concatenation[index] = str1[index];
        index++;
    }
    int index_str2 = 0;
    while(str2[index_str2] != '\0')
    {
        concatenation[index] = str2[index_str2];
        index++;
        index_str2++;
    }
    concatenation[index] = '\0';
    return concatenation;
}

void save_command(struct History* history) {
    
    if (command.text[0] == ' ') {
        return;
    }

    history->record[(history->start_index + history->count) % 10] = command.text;
    
    if (history->count == 10) {

        history->start_index = (history->start_index + 1) % 10;

        txtPointer = fopen(history->txt_path, "w+");
        fclose(txtPointer);
        
        for (int i = 0; i < 10; i++)
        {
            char* dataToBeWritten = history->record[(history->start_index + i) % 10];   
            txtPointer = fopen(history->txt_path, "a");

            if (txtPointer == NULL) {
                printf("History file failed to open.");
            }
            else {

                if (strlen(dataToBeWritten) > 0) {
                    fputs(dataToBeWritten, txtPointer);
                    if (dataToBeWritten[strlen(dataToBeWritten) - 1] != '\n') {
                        fputs("\n", txtPointer);
                    }       
                }
                fclose(txtPointer);
            }
        }
    }
    else {

        char* dataToBeWritten = history->record[(history->start_index + history->count) % 10];   

        txtPointer = fopen(history->txt_path, "a");

        if (txtPointer == NULL) {
            printf("History file failed to open.");
        }
        else {

            if (strlen(dataToBeWritten) > 0) {
               
               fputs(dataToBeWritten, txtPointer);
               fputs("\n", txtPointer);
            }
            
            fclose(txtPointer);
        }
        history->count++;
    }
        
}

void init_command() {
    command.in_fd = STDIN_FILENO;
    command.out_fd = STDOUT_FILENO;
    command.type = normal;
    command.built_in = 0;
    command.tokens = (char**)calloc(20, sizeof(char*));
    command.text = (char*)malloc(1024);
}

void tokenize(char *text, size_t characters_count) {
    strncpy(command.text, text, characters_count - 1);
    
    char* text_clone = (char*)malloc(strlen(command.text));
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
        else if (strlen(token) == 1 && strncmp(token, "<", 1) == 0) {
            command.type = cin;
            int file = open(command.tokens[i + 1], O_RDONLY);
            command.in_fd = file;
        }
        else if (strlen(token) == 2 && strncmp(token, ">>", 2) == 0) {
            command.type = ccout;
            int file = open(command.tokens[i + 1], O_APPEND |  O_CREAT , 0644 );
            command.out_fd = file;
        }
        else if (strlen(token) == 1 && strncmp(token, ">", 1) == 0) {
            command.type = cout;
            command.tokens[i] = NULL;
            int file = open(command.tokens[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
            command.out_fd = file;
        }
        else if (strlen(token) == 2 && strncmp(token, "cd", 2) == 0) {
            command.type = cd;
            command.built_in = 1;
            chdir(command.tokens[i + 1]);
            break;
        }
        else if (strlen(token) == 7 && strncmp(token, "history", 7) == 0) {
            command.type = hist;
            command.built_in = 1;
        }
        else if (strlen(token) == 5 && strncmp(token, "again", 5) == 0) {
            if (command.tokens[i + 1] != NULL)
            {
                int value = atoi(command.tokens[i + 1]);
                if (value > 0 && value <= history->count) {

                    char* line = history->record[(history->start_index + value - 1) % 10];
                    char* no_eol_line = (char*)malloc(sizeof(line));
                    for (int i = 0; i < strlen(line); i++)
                    {
                        if (line[i] == '\n')
                        {
                            break;
                        }
                        no_eol_line[i] = line[i];
                    }
                    compile_command(no_eol_line, strlen(no_eol_line) + 1, history);
                    return;
                }
            }
            command.type = unknown;
            command.built_in = 1;
            return;
        }
        else if (strlen(token) == 4 && strncmp(token, "exit", 4) == 0) {
            command.built_in = 1;
            command.type = quit;
        }
        else if (strlen(token) == 4 && strncmp(token, "help", 4) == 0)
        {
            command.built_in = 1;
            command.type = help;
        }
    }
    save_command(history);
}

void compile_command(char* text, size_t characters_count, struct History* history) {
    init_command();
    tokenize(text, characters_count);
    parse_command(history);
}

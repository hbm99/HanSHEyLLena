//
//  command.h
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#ifndef command_h
#define command_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {normal, unknown, cd, cin, cout, ccout, hist, quit};

struct Command{
    int type;
    int in_fd;
    int out_fd;
    int built_in;
    char** tokens;
    char* text;
};

void init_command(struct Command *command);

void tokenize(char *text, size_t characters_count, struct Command *command);

void parse_command(struct Command *command);

void compile_command(char *text, size_t characters_count, struct Command *command);

#endif /* command_h */

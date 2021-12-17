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
#include <sys/fcntl.h>
#include "history.h"

struct Command command;

enum {normal, unknown, cd, cin, cout, ccout, hist, quit};

struct Command{
    int type;
    int in_fd;
    int out_fd;
    int built_in;
    char** tokens;
    char* text;
};

void save_command(struct History* history);

void init_command(void);

void tokenize(char *text, size_t characters_count);

void parse_command(struct History* history);

void compile_command(char *text, size_t characters_count, struct History* history);

#endif /* command_h */

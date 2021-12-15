//
//  command.h
//  hanSHeyLLena
//
//  Created by Hansel Blanco on 9/12/21.
//

#ifndef command_h
#define command_h

#include <stdio.h>

enum {normal, unknown, cd, cin, cout, ccout, hist, quit};

struct Command{
    int type;
    int in_fd;
    int out_fd;
    int built_in;
    char** tokens;
    char* text;
};

void init_command(void);

void tokenize(char *text, size_t characters_count);

void parse_command(void);

void compile_command(char *text, size_t characters_count);

#endif /* command_h */

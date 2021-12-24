#include <sys/wait.h>
#include <signal.h>
#include "command.h"
#include <signal.h>

int running = 1;
int commands_counter;
struct History history;
struct Command* command_list;
int signal_counter;
char* original_path;
pid_t parent_id;
int prompt_ready = 0; 


void init_history(void);
char** parse_line(void);
int** init_multipipe(void);
void free_memory(void);
void print_prompt(void);

void free_memory() {
    free(command_list);
    free(original_path);
}

void print_prompt() {

    write(STDOUT_FILENO, "prompt $ ", 9);
    prompt_ready= 1;

}

void INThandler_initial(int signal) {
  
    write(STDIN_FILENO, "\n", 1);
    print_prompt();

}

void  INThandler(int sig)
{
    pid_t iPid = getpid(); 

    if (parent_id == iPid){

        write(STDIN_FILENO, "\n", 1);
        print_prompt();

    }

    if (signal_counter >= 1 && parent_id != iPid)
    {  

        kill(iPid, SIGKILL);

    }
    else {

        signal(sig, SIG_IGN);      
        signal_counter++;
        signal(sig, INThandler);

    }      
    
}

//Initializing history
void init_history() {

    history.count = 0;
    history.start_index = 0;
    history.txt_path = (char *)malloc(500);
    char* path_to_history = (char*)malloc(sizeof(original_path) + 1000);
    strcpy(path_to_history, original_path);
    history.txt_path = strcat(path_to_history, "/history.txt");
    
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

    signal(SIGINT, INThandler_initial);

    prompt_ready = 0;

    original_path = getcwd(original_path, 500);
    
    init_history();

    while (running) {

        if (!prompt_ready) print_prompt();
        
        char** list_commands_texts = parse_line();

        fill_command_list(list_commands_texts);

        int** pipeline = init_multipipe();

        int pipes_counter = commands_counter - 1;

        int command_index = 0;
        for (int i = 0; i < commands_counter; i++)
        {
            struct Command current_command = command_list[i];

            signal_counter = 0;

            int pid = fork();

            signal(SIGINT, INThandler);

            prompt_ready = 0;

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

                if (!current_command.built_in) {
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
                else if (current_command.type == help)
                {
                    char* help_type = "";
                    if (command.tokens[i + 1] != NULL)
                        help_type = command.tokens[i + 1];
                    else
                        help_type = "help";
                    
                    char* temp_txt = ".txt";
                    char* path_with_help = "/help/";
                    char* help_full_path = concat(original_path, concat(path_with_help, concat(help_type, temp_txt)));
                    
                    char str[1000];
                    txtPointer = fopen(help_full_path, "r");
                    if (txtPointer == NULL)
                        printf("Some help file failed to open.\n");
                    else
                    {
                        while(1) 
                        {
                            fgets(str, 999, txtPointer);
                            if (feof(txtPointer))
                            {
                                break;
                            }
                            printf("%s", str);
                        }
                    }
                    fclose(txtPointer);
                }
                else if (current_command.type == quit) {
                    running = 0;
                    kill(getppid(), SIGKILL);
                }
                exit(0);
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
            parent_id = getpid(); 
            wait(NULL);
         
        }
    }
    free_memory();
    return 0;
}

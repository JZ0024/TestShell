#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <readline/readline.h> 
#include <readline/history.h> 

#define MAXWORDS 1000
#define MAXCOMS 100
#define clear() printf("\033[H\033[J")

void init_shell(){
    char *user;

    clear();
    printf("\n\n********** THIS IS A TEST SHELL **********\n\n");

    user = getenv("USER");
    printf("\n USER: @%s\n", user);
    sleep(1);
    clear();
}

int get_input(char *in){
    char *buffer;
    buffer = readline("\ntstsh$$ ");
    if(strlen(buffer)){
        add_history(buffer);
        strcpy(in, buffer);
        return 0;
    }

    return 1;
}

void print_dir(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDirectory: %s\n", cwd);
}

void execute(char **commands){
    pid_t pid = fork();

    switch(pid){
        case -1:
            perror("Error: ");
            break;

        case 0:
            if(execvp(commands[0], commands) < 0){
                perror("Error: ");
            }

            exit(0);

        default:
            wait(NULL);
            break;
    }

    return;
}

void execute_pipe(char **commands, char **pipe_commands){
    int pipefd[2];
    pid_t p1, p2;

    if(pipe(pipefd) < 0){
        perror("Error: ");
        return;
    }

    p1 = fork();
    switch(p1){
        case -1:
            perror("Fork error: ");
            break;

        case 0:
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
    
            if(execvp(commands[0], commands) < 0){
                perror("Pipe error: ");
                exit(0);
            }
            break;

        default:
            p2 = fork();
            switch(p2){
                case -1:
                    perror("Parent fork error: ");
                    break;

                case 0:
                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                    
                    if(execvp(pipe_commands[0], pipe_commands) < 0){
                        perror("Parent error: ");
                        exit(0);
                    }
                    break;

                    default:
                        wait(NULL);
                        wait(NULL);
                        break;
                }
        }

        return;
}

void print_man(){
    puts("\n*** TEST SHELL MANUAL ***"
        "\nCreated by JZ0024"
        "\n--This is a test shell. Use with caution."
        "\nCommands:"
        "\n   For UNIX commands, look them up yourself, scrub"
        "\n   cd     Change directories"
        "\n   ls     List directories"
        "\n   exit   Exit shell"
        "\n\nSupported Features:"
        "\n   Piping"
        "\n   Improper spacing");
        return;
}

int execute_my_commands(char **commands){
    int i, switch_args = 0;
    const char* command_list[3];
    char* user;
    
    command_list[0] = "exit";
    command_list[1] = "cd";
    command_list[2] = "help";
    
    for(i = 0; i < 3; i++){
        if(!strcmp(commands[0], command_list[i])){
            break;
        }
    }

    switch(i){
        case 0:
            printf("\nExiting...\n");
            exit(0);
    
        case 1:
            chdir(commands[1]);
            return 1;

        case 2:
            print_man();
            return 1;

        default:
            user = getenv("USER");
            printf("\nHello %s. When you were out partying, I was studying C "
                "and command line shells...\nType \"help\" if you're lost\n", user);
            break;
    }

    return 0;
}

int parse_pipe(char *str, char** result){
    int i;
    
    for(i = 0; i < 2; i++){
        result[i] = strsep(&str, "|");
    
        if(!result[i]){
            break;
        }
    }

    return result[1] ? 1 : 0;
}

void parse_space(char* str, char** result){
    int i;

    for(i = 0; i < MAXCOMS; i++){
        result[i] = strsep(&str, " ");
        
        if(!result[i]){
            break;
        }

        if(!strlen(result[i])){
            i--;
        }
    }
}

int process_string(char* str, char** result, char** pipe_result){
    char* piped_str[2];
    int piped = 0;

    piped = parse_pipe(str, pipe_result);

    if(piped){
        parse_space(piped_str[0], result);
        parse_space(piped_str[1], pipe_result);
    }else{
        parse_space(str, result);
    }

    if(execute_my_commands(result)){
        return 0;
    }

    return 1 + piped;
}

int main(){
    char input_str[MAXWORDS], *parsed_args[MAXCOMS];
    char* parsed_piped_args[MAXCOMS];
    int exec_flag = 0;
    
    init_shell();

    while(1){
        print_dir();

        if(get_input(input_str)){
            continue;
        }

        exec_flag = process_string(input_str, parsed_args, parsed_piped_args);

        if(exec_flag == 1){
            execute(parsed_args);
        }

        if(exec_flag == 2){
            execute_pipe(parsed_args, parsed_piped_args);
        }

    }

    return 0;
}

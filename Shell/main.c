#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

#define RL_BUFFER_SIZE 1024
#define TOK_BUFFER_SIZE 64
#define TOK_DELIMITERS " \t\r\n\a"

void shell_loop();
char* read_line();
char** tokenize(char* line);
int execute(char** command);
int launch(char** command);


int main(void) {
    shell_loop();

    return EXIT_SUCCESS;
}

void shell_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = read_line();
        args = tokenize(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
}

char* read_line() {
    int buffer_size = RL_BUFFER_SIZE;
    int buffer_index = 0;
    char* buffer = malloc(buffer_size * sizeof(char));
    int c;

    if (!buffer) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[buffer_index] = '\0';
            return buffer;
        }
        else {
            buffer[buffer_index++] = c;
        }

        if (buffer_index >= buffer_size) {
            buffer_size += RL_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char** tokenize(char *line) {
    int token_index = 0, buffer_size = TOK_BUFFER_SIZE;
    char* token;
    char** tokens = malloc(sizeof(char*) * TOK_BUFFER_SIZE);

    if (!tokens) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIMITERS);
    while (token) {
        tokens[token_index++] = token;

        if (token_index >= TOK_BUFFER_SIZE) {
            buffer_size += TOK_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size);

            if (!tokens) {
                fprintf(stderr, "realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIMITERS);
    }

    tokens[token_index] = NULL;
    return tokens;
}

int execute(char **args)
{
    if (args[0] == NULL) {
        return 1;
    }

    return launch(args);
}

int launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("fork");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("fork");
    }
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}




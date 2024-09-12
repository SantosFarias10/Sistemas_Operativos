#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#include "obfuscated.h"

static void show_prompt(void) {
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *home = getenv("HOME");
        if (home != NULL && strncmp(cwd, home, strlen(home)) == 0) {
            printf("\033[36m[~%s]\033[0m\033[32mmybash>\033[0m ", cwd + strlen(home));
        } else {
            printf("\033[36m%s/mybash> \033[0m", cwd);
        }
    }
    fflush(stdout);
}

static void show_group_name(void) {
    printf("\n");
    printf("\033[35m ___       __   ________  ___       ________  ___      ___ _______   ________  ________           ________ ________  ________      \n\033[0m ");
    printf("\033[35m|\\  \\     |\\  \\|\\   __  \\|\\  \\     |\\   __  \\|\\  \\    /  /|\\  ___ \\ |\\   __  \\|\\   ____\\         |\\   ____\\_____  \\|\\   ____\\     \n\033[0m ");
    printf("\033[35m\\ \\  \\    \\ \\  \\ \\  \\|\\  \\ \\  \\    \\ \\  \\|\\  \\ \\  \\  /  / | \\   __/|\\ \\  \\|\\  \\ \\  \\___|_        \\ \\  \\___|____|\\ /\\ \\  \\___|     \n\033[0m ");
    printf("\033[35m \\ \\  \\  __\\ \\  \\ \\  \\\\\\  \\ \\  \\    \\ \\  \\\\\\  \\ \\  \\/  / /   \\ \\  \\_|/_\\ \\   _  _\\ \\_____  \\        \\ \\  \\  ___   \\|\\  \\ \\  \\____    \n\033[0m ");
    printf("\033[35m  \\ \\  \\|\\__\\_\\  \\ \\  \\\\\\  \\ \\  \\____\\ \\  \\\\\\  \\ \\    / /     \\ \\  \\_|\\ \\ \\  \\\\  \\|____|\\  \\        \\ \\  \\|\\  \\ __\\_\\  \\ \\  ___  \\  \n\033[0m ");
    printf("\033[35m   \\ \\____________\\ \\_______\\ \\_______\\ \\_______\\ \\__/ /       \\ \\_______\\ \\__\\\\ _\\ ____\\_\\  \\        \\ \\_______\\\\_______\\ \\_______\\ \n\033[0m ");
    printf("\033[35m    \\|____________|\\|_______|\\|_______|\\|_______|\\|__|/         \\|_______|\\|__|\\|__|\\_________\\        \\|_______|\\_______|\\|_______| \n\033[0m ");
    printf("\033[35m                                                                                   \\|_________|                                       \n\033[0m ");
    printf("\n");
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input = parser_new(stdin);

    show_group_name();

    while (!parser_at_eof(input)) {
        //ping_pong_loop(NULL);
        show_prompt();

        pipe = parse_pipeline(input);

        if (pipe != NULL) {
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        }

    }
    parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;
}


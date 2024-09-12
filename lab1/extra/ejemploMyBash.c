#include <stdio.h>
#include <unistd.h>

int main(void) {
    char *cmd = "cat";
    char *argv[3];
    argv[0] = cmd;
    argv[1] = "ejemploMyBash.c";
    argv[2] = NULL;

    execvp(cmd, argv);
    printf("Algo salio mal...");
    return 1;
}
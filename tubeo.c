/*  INF3173 - TP2
 *  Session : été 2022
 *  
 *  IDENTIFICATION.
 *
 *      Nom : Salcedo
 *      Prénom : Renzo Arturo
 *      Code permanent : SALR02089408
 *      Groupe : 40
 */
 
int main(int argc, char ** argv) {
    int status;
    int compteur = 0;
    int argument_n =  1 << 20;
    int nb_commandes = 1;
    int pid_enfant;
    int index_position = 0;

    for (int k = 1; k < argc; k++)
        if (strcmp(argv[k], ":") == 0)
            command_count++;

    command_count++;
    char ***cmd = calloc(command_count + 1, sizeof(char **));

    int command = 0;
    int first_arg = 1;
    if (argv[1][0] == '-')
        first_arg = 3;
    int p[2];
    pid_t pid;
    int fd_in = 0;
    int status = 0;

    while (*cmd != NULL) {
        pipe(p);
        if ((pid = fork()) == -1) {
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            dup2(fd_in, 0); 
            if (*(cmd + 1) != NULL)
                dup2(p[1], 1);
            close(p[0]);
            execvp(*(cmd)[0], *cmd);
            _Exit(127);
        } else {
//            if (compteur == *argv - '0') {
//                b_read = splice(fd_in, NULL, STDOUT_FILENO, NULL, 200, NULL);
//                printf("%d", b_read);
//            }
            close(p[1]);
            fd_in = p[0]; 
            cmd++;
        }
    }
    return 0;
}

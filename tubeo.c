#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define READ_END     0
#define WRITE_END    1

void modifier_argv(char **argv, int argc, int *nb_commandes) {
    for (int i = 0; i < argc; ++i)
        if (argv[i][0] == ':') {
        argv[i] = NULL;
        ++*nb_commandes;
    }
}

void valider_arg_n(char *const *argv, int *compteur, int *argument_n) {
    if (argv[1][0] == '-') {
        *argument_n = ((argv[1][3]) - '0') - 1;
        *compteur = 2;
    }
}

int main(int argc, char *argv[]) {
    int status;
    int compteur = 0;
    int argument_n =  1 << 20;
    int nb_commandes = 1;
    int pid_enfant;
    int index_position = 0;
    modifier_argv(argv, argc, &nb_commandes);
    valider_arg_n(argv, &compteur, &argument_n);
    int index[nb_commandes];
    index[index_position++] = ++compteur;
    for (int r = 0; r < argc; ++r) {
        if (argv[r] == NULL)
            index[index_position++] = r + 1;
    }
    int pipe_precedent, pf_file_descriptor[2];
    pipe_precedent = STDIN_FILENO;
    ssize_t octets_lus = 0;

    for (int i = 0; i < nb_commandes; i++) {
        pipe(pf_file_descriptor);
        if ((pid_enfant = fork()) == 0) {
            if (pipe_precedent != STDIN_FILENO) {
                dup2(pipe_precedent, STDIN_FILENO);  // redirect precedent vers stdin
                close(pipe_precedent);
            }
            if (i < nb_commandes - 1 || i == argument_n) {         // redirect pdf[1] vers stdout
                dup2(pf_file_descriptor[1], STDOUT_FILENO);
                close(pf_file_descriptor[1]);
            }
            execvp(argv[index[i]], argv + (index[i]));
            _Exit(127);
        }
        wait(&status);
        close(pipe_precedent); // Ferme read end du pipe precent (pas necessaire dans parent)
        close(pf_file_descriptor[1]); // Ferme write du pipe courrant (pas necessaire dans parent)
        pipe_precedent = pf_file_descriptor[0]; // Sauvegarde read end du peipe courrant pour utiliser dans la prochaine iteration
        if (i == argument_n) {
            //pf_file_descriptor[0] = probe(pf_file_descriptor[0], pi);
            int file_descriptor_splice[2];
            pipe(file_descriptor_splice);
            octets_lus = splice(pf_file_descriptor[0], NULL, file_descriptor_splice[1], NULL, 200, SPLICE_F_MOVE);
            close(file_descriptor_splice[1]);
            pipe_precedent = file_descriptor_splice[0];
            printf("%ld", octets_lus);
        }
    }
    close(pf_file_descriptor[0]);
    close(pf_file_descriptor[1]);
    if WIFEXITED(status)
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
}


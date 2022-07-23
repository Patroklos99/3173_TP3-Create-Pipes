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
 
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char ** argv) {
    int status;
    int compteur = 0;
    int argument_n =  1 << 20;
    int nb_commandes = 1;
    int pid_enfant;
    int index_position = 0;
    
    for (int i = 0; i < argc; ++i)
        if (argv[i][0] == ':') {
            argv[i] = NULL;
            ++nb_commandes;
        }
    if (argv[1][0] == '-') {
        argument_n = ((argv[1][3]) - '0') - 1;
        compteur = 2;
    }
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
    }
    close(pf_file_descriptor[0]);
    close(pf_file_descriptor[1]);  
    return 0;
}

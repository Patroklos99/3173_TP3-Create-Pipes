#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define MAX_PIPE_SIZE 64000
#define READ_END     0
#define WRITE_END    1

/**
 * Modifie les arguments ':' et compte le nombre de commandes.
 *
 * @param argv pointeur contenant les arguments.
 * @param argc nb d'arguments.
 * @param nb_commandes variable du nombre de commandes.
 */
void modifier_argv(char **argv, int argc, int *nb_commandes) {
    for (int i = 0; i < argc; ++i)
        if (argv[i][0] == ':') {
            argv[i] = NULL;
            ++*nb_commandes;
        }
}

/**
 * Valide si l'argument -n existe et le sauvegarde en variable.
 *
 * @param argv pointeur contenant les arguments.
 * @param argument_n variable ou l'argument de -n est placé.
 *
 * @return position_1ere_commande position de la 1ere commande (echo).
 */
int valider_arg_n(char *const *argv, int *argument_n) {
    int position_1ere_commande = 0;
    if (argv[1][0] == '-') {
        *argument_n = ((argv[1][3]) - '0') - 1;
        position_1ere_commande = 2;
    }
    return position_1ere_commande;
}

/**
 * Indexe les positions correcte des commandes valides.
 *
 * @param argc nb d'arguments.
 * @param argv pointeur contenant les arguments.
 * @param position_1ere_commande position de la 1ere commande (echo).
 * @param index tableau de int ou les positions sont sauvegardées.
 *
 */
void indexer_arguments(int argc, char *const *argv, int position_1ere_commande, int *index) {
    int index_position = 0;
    index[index_position++] = ++position_1ere_commande;
    for (int r = 0; r < argc; ++r) {
        if (argv[r] == NULL)
            index[index_position++] = r + 1;
    }
}

/**
 * Execute le processus fils et ses fonctions correspondante.
 *
 * @param argv pointeur contenant les arguments.
 * @param argument_n variable ou l'argument de -n est placé.
 * @param nb_commandes variable du nombre de commandes.
 * @param index tableau de int ou les positions sont sauvegardées.
 * @param pipe_precedent pipe contenant la dernière lecture en sortie.
 * @param pf_file_descriptor array contenant les 2 derniers pipes d'ecriture et de lecture créés.
 * @param i compteur de la boucle for necessaire pour l'execution ordonnée des commandes.
 *
 */
void executer_processus_fils(char *const *argv, int argument_n, int nb_commandes, const int *index, int pipe_precedent,
                             const int *pf_file_descriptor, int i) {
    if (pipe_precedent != STDIN_FILENO) {
        dup2(pipe_precedent, STDIN_FILENO);  // redirect pipe_precedent vers stdin
        close(pipe_precedent);
    }
    if (i < nb_commandes - 1 || i == argument_n) {      // redirect pdf[1] vers stdout
        dup2(pf_file_descriptor[1], STDOUT_FILENO);
        close(pf_file_descriptor[1]);
    }
    execvp(argv[index[i]], argv + (index[i]));
    _Exit(127);
}

/**
 * Affiche les octets lus s'ils sont égales ou plus grand que 0.
 */
void afficher_octets(ssize_t octets_lus) {
    if (octets_lus >= 0)
        printf("%ld", octets_lus);
}


/**
 * Execute le processus fils et ses fonctions correspondante.
 *
 * @param pf_file_descriptor array contenant les 2 derniers pipes d'ecriture et de lecture créés.
 *
 * @return file_descriptor_splice[0] la nouvelle valeur de pipe_precendant.
 */
int executer_splice(const int *pf_file_descriptor) {
    ssize_t octets_lus = 0;
    int file_descriptor_splice[2];
    pipe(file_descriptor_splice);
    octets_lus = splice(pf_file_descriptor[0], NULL, file_descriptor_splice[1], NULL, MAX_PIPE_SIZE, SPLICE_F_MOVE);
    close(file_descriptor_splice[1]);
    afficher_octets(octets_lus);
    return file_descriptor_splice[0]; //retourne la nouvelle valeur de pipe_précedant.
}

/**
 * Valide le type de sortie (sortie reguliere ou sortie signal)
 *
 * @param status valeur necessaire pour les validations.
 *
 * @return le valeur de la sortie dependant du type.
 */
int valider_type_sortie(int status) {
    if WIFEXITED(status)
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
}

/**
 * Execute les commandes désirées passées en argument.
 *
 * @param argv pointeur contenant les arguments.
 * @param argument_n variable ou l'argument de -n est placé.
 * @param nb_commandes variable du nombre de commandes.
 * @param index tableau de int ou les positions sont sauvegardées.
 *
 * @return le valeur de sortie.
 */
int executer_tubes(char *const *argv, int argument_n, int nb_commandes, const int *index) {
    int status;
    int pid_enfant;
    int pipe_precedent, pf_file_descriptor[2];
    pipe_precedent = STDIN_FILENO;
    for (int i = 0; i < nb_commandes; i++) {
        pipe(pf_file_descriptor);
        if ((pid_enfant = fork()) == 0)
            executer_processus_fils(argv, argument_n, nb_commandes, index, pipe_precedent, pf_file_descriptor, i);
        else {
            wait(&status);
            close(pipe_precedent); // Ferme read end du pipe precent (pas necessaire dans parent)
            close(pf_file_descriptor[1]); // Ferme write du pipe courrant (pas necessaire dans parent)
            pipe_precedent = pf_file_descriptor[0]; // Sauvegarde read end du peipe courrant pour utiliser dans la prochaine iteration
            if (i == argument_n)
                pipe_precedent = executer_splice(pf_file_descriptor);
        }
    }
    close(pf_file_descriptor[0]);
    close(pf_file_descriptor[1]);
    return valider_type_sortie(status);
}

int main(int argc, char *argv[]) {
    int argument_n = 1 << 20;
    int nb_commandes = 1;
    modifier_argv(argv, argc, &nb_commandes);
    int position_1ere_commande = valider_arg_n(argv, &argument_n);
    int index[nb_commandes];
    indexer_arguments(argc, argv, position_1ere_commande, index);
    return executer_tubes(argv, argument_n, nb_commandes, index);
}


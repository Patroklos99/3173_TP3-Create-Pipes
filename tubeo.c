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
    int command_count = 0;
    for (int k = 1; k < argc; k++)
        if (strcmp(argv[k], ":") == 0)
            command_count++;

    command_count++;
    char ***cmd = calloc(command_count + 1, sizeof(char **));

    int command = 0;
    int first_arg = 1;
    if (argv[1][0] == '-')
        first_arg = 3;
    return 0;
}

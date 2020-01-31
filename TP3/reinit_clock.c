/* ================================================================================ */
/*                              Signaux et tubes */
/* ================================================================================ */

/*

Auteur : Fabien SIMONET
Date : 31/01/2020

Objectif :

Écrivez un programme qui réinitialise l'heure à une heure, minute et seconde données. 
Ce programme communiquera avec le processus principal de l'horloge.

*/

/* ================================================================================ */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("./reinit_clock clock_pid\n");
    }
    else 
    {
        // On lance un signal SIGUSR1
        kill(atoi(argv[1]), SIGUSR1);
    }
    return 0;
}
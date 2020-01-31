/* ================================================================================ */
/*                              Signaux et tubes */
/* ================================================================================ */

/*

Auteur : Fabien SIMONET
Date : 31/01/2020

Objectif : Écrivez un programme qui arrête l'horloge et fait le ménage. 

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
        // On lance un signal SIGKILL
        kill(atoi(argv[1]), SIGKILL);
    }
    return 0;
}
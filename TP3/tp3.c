/* ================================================================================ */
/*                              Signaux et tubes */
/* ================================================================================ */

/*

Auteur : Fabien SIMONET
Date : 31/01/2020

Objectif :

Dans cet exercice, vous allez coordonner plusieurs processus pour simuler le comportement d'une horloge. 
L'utilisation de sleep() est donc prohibée. 
Pour vos tests, une minute peut durer moins que 60 secondes.

Écrivez un programme qui lance deux processus : le premier surveille l'horloge du système avec time() ; le deuxième dort. 
Quand le premier a vu une seconde passer, il réveille le deuxième qui incrémente un compteur et se rendort.

Ajoutez deux autres processus pour compter les minutes et les heures. 
On aura donc quatre processus au total. Leur réveil se fera en cascade.

*/

/* ================================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// Contient tous les compteurs
typedef struct counters
{
    int sec_cpt;
    int min_cpt;
    int hour_cpt;
};

void displayHeader()
{
    printf("================ Horloge ================\nMon pid est : %d\n\n", getpid());
}

void signal_handler(int i, siginfo_t * s, void *v) { 
  printf("Received signal\n"); 
  // Reinitialiser ici
} 

// Reinitialise les heures, minutes et secondes
void reinit(struct counters* c, int s_cpt, int m_cpt, int h_cpt)
{
    c->sec_cpt = s_cpt;
    c->min_cpt = m_cpt;
    c->hour_cpt = h_cpt;
}

// Arrete l'horloge
void stopClock()
{
    exit(1);
}

int main(int argc, char ** argv) {

    struct counters cpts;

    struct sigaction action;
    action.sa_sigaction = signal_handler;
    sigfillset(&action.sa_mask);
    sigaction(SIGUSR1, &action, 0);

    /// Initialisation des compteurs
    if (argc >= 4) {
        reinit(&cpts, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    } 
    else 
    {
        reinit(&cpts, 0, 0, 0);
    }

    displayHeader();

    printf("Init : hour = %d, min = %d, sec = %d\n", cpts.hour_cpt, cpts.min_cpt, cpts.sec_cpt);

    // Initialisation des tubes : chaque tube contient un descripteur de lecture (en 0) et un descripteur d'ecriture (en 1)

    int obs_fd[2]; // Descripteurs pour le pipe d'observation de l'horloge
    pipe(obs_fd); // Pipe pour l'observation de l'horloge

    int sec_fd[2];
    pipe(sec_fd);

    int min_fd[2];
    pipe(min_fd);

    int hour_fd[2];
    pipe(hour_fd);

    // Creation du processus qui incremente les secondes
    int sec_process = fork();
    if (sec_process) {
        // Creation du processus qui incremente les minutes
        int min_process = fork();
        if (min_process) {
            // Creation du processus qui incremente les heures
            int hour_process = fork();
            if (hour_process) {
                
                // Observation de l'heure courante
                int result;
                int last_date = time(NULL);     // Recuperation de la date courante qu'on enregistre en tant que derniere valeur connue
                int current_date = 0;           // Date courante que l'on pourra comparer avec l'ancienne heure courante

                // Ferme le tube des secondes en lecture
                close(sec_fd[0]);
                // Ferme le tube de l'observateur en ecriture
                close(obs_fd[1]);

                // Lit le tube de l'observateur 
                read(obs_fd[0], &result, sizeof(result));
                
                // Attente active
                while (1) {
                    
                    // Mise a jour de la date courante
                    current_date = time(NULL);
                    if (current_date-last_date >= 1) { // Si une seconde est passee
                        // Envoi du signal du passage d'une seconde au tube des secondes
                        write(sec_fd[1], &result, sizeof(result));
                        last_date = current_date;
                    }
                }
            
            } else {
                // Incrementation des heures
                int start;
                // Ferme le tube des observation en lecture
                close(obs_fd[0]);
                // Envoi du signal de depart
                write(obs_fd[1], &start, sizeof(start));

                int result;
                // Ferme le tube des observation en ecriture
                close(hour_fd[1]);

                // Attente active
                while (1) {
                    // Attente du passage d'une minute
                    read(hour_fd[0], &result, sizeof(result));
                    cpts.hour_cpt++; // Incrémentation des heures
                    if (cpts.hour_cpt > 23) { // Si une minute est passee
                        cpts.hour_cpt = 0;
                        // Passage a un nouveau jour (cas non gere)
                    }
                    // Affiche les heures
                    printf("%dhour\n", cpts.hour_cpt);
                }
            }

        
        } else {
            // Incrementation des minutes
            int result;

            // Ferme le tube des minutes en ecriture
            close(min_fd[1]);
            // Ferme le tube des heures en lecture
            close(hour_fd[0]);

            // Attente active
            while (1) {
                /* Attente du passage d'une minute */
                read(min_fd[0], &result, sizeof(result));
                cpts.min_cpt++; // Incrémentation des minutes
                if (cpts.min_cpt > 59) { // Une heure vient de s'ecouler
                    cpts.min_cpt = 0;
                    // Envoi du signal du passage d'une heure au tube des heures
                    write(hour_fd[1], &result, sizeof(result));
                }
                // Affiche les minutes
                printf("%dmin\n", cpts.min_cpt);
            }
        }
        


    } else {
        // Incrementation des secondes
        int result;
        // Ferme le tube des secondes en ecriture
        close(sec_fd[1]);
        // Ferme le tube des minutes en lecture
        close(min_fd[0]);

        while (1) {
            // Attend le passage d'une seconde
            read(sec_fd[0], &result, sizeof(result));
            
            cpts.sec_cpt++; // Incrémentation des minutes
            if (cpts.sec_cpt > 59) { // Une minute vient de s'écouler
                cpts.sec_cpt = 0;
                // Envoi du signal du passage d'une minute au tube des minutes
                write(min_fd[1], &result, sizeof(result));
            }
            
            // Affiche les secondes
            printf("%dsec\n", cpts.sec_cpt);
        }
    }   

    return 0;
}
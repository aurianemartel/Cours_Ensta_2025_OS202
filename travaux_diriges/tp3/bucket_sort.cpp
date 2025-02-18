/*
Sujet :
Implémenter l'algorithme "bucket sort" tel que décrit sur les deux dernières planches du cours n°3 :

le process 0 génère un tableau de nombres arbitraires,
il les dispatch aux autres process,
tous les process participent au tri en parallèle,
le tableau trié est rassemblé sur le process 0.
*/

# include <cstdlib>
# include <iostream>
# include <random>
#include <mpi.h>


int main(int nargs, char* argv[]) {
    MPI_Comm commGlob;
    int nbp, rank;
    MPI_Init(&nargs, &argv);// Initialization of the parallel context

    MPI_Comm_dup(MPI_COMM_WORLD, &commGlob);// Copy global communicator in own communicator;
    MPI_Comm_size(commGlob, &nbp);// Get the number of processes launched by the used;
    MPI_Comm_rank(commGlob, &rank);// Get the rank of the process in the communicator commGlob.

    
    if(rank==0) {
        // Génère tableau
        // Dispatch nombres aux autres process : scatter
    }    
    
    // Détermination des intervalles : (sur jeu réduit)
        // Tri local
        // Répartition des buckets selon chaque process
        // Allgather : obtention de la répartition des buckets
    // Tri des données :
        // Distribution des valeurs dans chaque bucket
        // Tri local
        // Gather tableaux triés

    MPI_Finalize();// Terminates the parallel context
    return EXIT_SUCCESS;
}
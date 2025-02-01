# include <cstdlib>
# include <iostream>
#include <mpi.h>

int main(int nargs, char* argv[]) {
    MPI_Comm commGlob;
    int nbp, rank;
    MPI_Init(&nargs, &argv);// Initialization of the parallel context

    MPI_Comm_dup(MPI_COMM_WORLD, &commGlob);// Copy global communicator in own communicator;
    MPI_Comm_size(commGlob, &nbp);// Get the number of processes launched by the used;
    MPI_Comm_rank(commGlob, &rank);// Get the rank of the process in the communicator commGlob.

    int jeton = 1;
    MPI_Status status;
    
    if(rank==0) {
        MPI_Send(&jeton, 1, MPI_INT, 1, 0, commGlob);
        MPI_Recv(&jeton, 1, MPI_INT, nbp-1, 0, commGlob, &status);
        std::cout << "Le jeton vaut " << jeton << "\n";
    } else {
        MPI_Recv(&jeton, 1, MPI_INT, rank-1, 0, commGlob, &status);
        jeton++;
        MPI_Send(&jeton, 1, MPI_INT, (rank+1)%nbp, 0, commGlob);
    }


    MPI_Finalize();// Terminates the parallel context
    return EXIT_SUCCESS;
}
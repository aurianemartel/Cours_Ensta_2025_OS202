/*
Sujet :
Implémenter l'algorithme "bucket sort" tel que décrit sur les deux dernières planches du cours n°3 :

le process 0 génère un tableau de nombres arbitraires,
il les dispatch aux autres process,
tous les process participent au tri en parallèle,
le tableau trié est rassemblé sur le process 0.
*/

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>
#include <mpi.h>

using namespace std;

void generate_random_vector(vector<double>& v, float min, float max, 
                            const string type_distrib = "uniform") {
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> distrib(min, max);
    auto mydistrib = [&]() {return distrib(gen);};
    std::generate(v.begin(),v.end(), mydistrib);
}

void affichage_vect(string msg, vector<double> v) {
    cout << msg << "\n";
    for (double& val:v) {
        cout << val << " ";
    }
    cout << "\n";
}

void calc_quantiles(vector<double>& q, const vector<double> v){
    // Tri du vecteur
    vector<double> cv = v;
    sort(cv.begin(),cv.end());
    //affichage_vect(">> Tableau trié:",cv);

    // Calcul des quantiles
    for (long unsigned int i=0; i<q.size(); i++) {
        const int j1 = (i*(cv.size()-1))/(q.size()-1);
        const int j2 = (i*(cv.size()-1))%(q.size()-1);
        if (j2 == 0) {
            q[i] = cv[j1];
        } else {
            q[i] = (cv[j1] + cv[j1+1])/2;
        }
    }
}

int main(int nargs, char* argv[]) {

    int taille = 10;
    if (nargs>1) {
        taille = atoi(argv[1]);
    }

    using Clock = std::chrono::high_resolution_clock;
    using Duration = Clock::duration;
 
    MPI_Comm commGlob;
    MPI_Init(&nargs, &argv);// Initialization of the parallel context

    int nbp, rank;
    MPI_Comm_dup(MPI_COMM_WORLD, &commGlob);// Copy global communicator in own communicator;
    MPI_Comm_size(commGlob, &nbp);// Get the number of processes launched by the used;
    MPI_Comm_rank(commGlob, &rank);// Get the rank of the process in the communicator commGlob.

    vector<double> v(taille);
    //cout << "Rank:" << rank << ", nb proc:" << nbp << "\n";

    Clock::time_point beginning;

    if(rank==0) {
        // Génère tableau
        generate_random_vector(v,0,1);
        affichage_vect("Tableau d'entrée:",v);

        /*
        vector<double> q(5);
        calc_quantiles(q,v);
        affichage_vect("Quantiles:",q);
        */
        beginning = Clock::now();
    }    
    
    // Dispatch nombres aux autres process : scatter
    int taille_loc = taille/nbp;
    if (taille%nbp != 0) {
        cout << "Taille non multiple de nbp\n";
    }
    vector<double> loc_v(taille_loc);
    MPI_Scatter(v.data(),taille_loc,MPI_DOUBLE,loc_v.data(),taille_loc,MPI_DOUBLE,0,commGlob);
    //affichage_vect("Sous-vecteur:",loc_v);

    vector<double> loc_q(nbp+1);
    calc_quantiles(loc_q,loc_v);
    //affichage_vect("Quantiles:",loc_q);

    // Gather : récupération des quantiles
    vector<double> glob_q((nbp+1)*nbp);
    MPI_Gather(loc_q.data(),loc_q.size(),MPI_DOUBLE,glob_q.data(),loc_q.size(),MPI_DOUBLE,0,commGlob);

    vector<double> loc_bucket(taille_loc);

    // Quantiles : détermination des buckets
    if (rank == 0) {
        vector<double> qq(nbp+1);
        calc_quantiles(qq,glob_q);
        affichage_vect("Quantiles globaux:",qq);
        
        vector<vector<double>> buckets(nbp);
        // boucle for : répartir valeurs du tableau en vecteurs selon leur bucket
        for (long unsigned int i=0; i<v.size(); ++i) {
            for (long unsigned int j=1; j<qq.size(); j++) {
                if (v[i]<qq[j]) { // quantile trouvé
                    buckets[j-1].push_back(v[i]);
                    break;
                }
            }
            if (v[i]==qq[qq.size()-1]) {
                buckets[nbp-1].push_back(v[i]);
            }
        }
        /*
        for (int i=0;i<nbp;++i) {
            affichage_vect("Buckets:",buckets[i]);
        }*/

        for (int i=0;i<nbp;++i) {
            // Envoyer vecteur de données à chaque processeur
            MPI_Send(buckets[i].data(),taille_loc,MPI_DOUBLE,i,101,commGlob);
        }
    }
    MPI_Status status;
    MPI_Recv(loc_bucket.data(),taille_loc,MPI_DOUBLE,0,101,commGlob, &status);
    
    sort(loc_bucket.begin(),loc_bucket.end());
    vector<double> vt(taille);
    MPI_Gather(loc_bucket.data(),taille_loc,MPI_DOUBLE,vt.data(),taille_loc,MPI_DOUBLE,0,commGlob);

    // Affichage tableau trié
    if (rank==0) {
        Clock::time_point end = Clock::now();
        auto duration = end - beginning;

        affichage_vect("Tableau trié",vt);
        cout << "Durée de calcul: " << duration.count() 
                                            * double(Duration::period::num)
                                            / Duration::period::den
                                    << " sec\n";
    }

    MPI_Finalize();// Terminates the parallel context
    return EXIT_SUCCESS;
}
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>

#include "model.hpp"
#include "display.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

struct ParamsType
{
    double length{1.};
    unsigned discretization{20u};
    std::array<double,2> wind{0.,0.};
    Model::LexicoIndices start{10u,10u};
};

void analyze_arg( int nargs, char* args[], ParamsType& params )
{
    if (nargs ==0) return;
    std::string key(args[0]);
    if (key == "-l"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une valeur pour la longueur du terrain !" << std::endl;
            exit(EXIT_FAILURE);
        }
        params.length = std::stoul(args[1]);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    auto pos = key.find("--longueur=");
    if (pos < key.size())
    {
        auto subkey = std::string(key,pos+11);
        params.length = std::stoul(subkey);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-n"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une valeur pour le nombre de cases par direction pour la discrétisation du terrain !" << std::endl;
            exit(EXIT_FAILURE);
        }
        params.discretization = std::stoul(args[1]);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--number_of_cases=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+18);
        params.discretization = std::stoul(subkey);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-w"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une paire de valeurs pour la direction du vent !" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string values =std::string(args[1]);
        params.wind[0] = std::stod(values);
        auto pos = values.find(",");
        if (pos == values.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(values, pos+1);
        params.wind[1] = std::stod(second_value);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--wind=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+7);
        params.wind[0] = std::stoul(subkey);
        auto pos = subkey.find(",");
        if (pos == subkey.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(subkey, pos+1);
        params.wind[1] = std::stod(second_value);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-s"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une paire de valeurs pour la position du foyer initial !" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string values =std::string(args[1]);
        params.start.column = std::stod(values);
        auto pos = values.find(",");
        if (pos == values.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la position du foyer initial" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(values, pos+1);
        params.start.row = std::stod(second_value);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--start=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+8);
        params.start.column = std::stoul(subkey);
        auto pos = subkey.find(",");
        if (pos == subkey.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(subkey, pos+1);
        params.start.row = std::stod(second_value);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }
}

ParamsType parse_arguments( int nargs, char* args[] )
{
    if (nargs == 0) return {};
    if ( (std::string(args[0]) == "--help"s) || (std::string(args[0]) == "-h") )
    {
        std::cout << 
R"RAW(Usage : simulation [option(s)]
  Lance la simulation d'incendie en prenant en compte les [option(s)].
  Les options sont :
    -l, --longueur=LONGUEUR     Définit la taille LONGUEUR (réel en km) du carré représentant la carte de la végétation.
    -n, --number_of_cases=N     Nombre n de cases par direction pour la discrétisation
    -w, --wind=VX,VY            Définit le vecteur vitesse du vent (pas de vent par défaut).
    -s, --start=COL,ROW         Définit les indices I,J de la case où commence l'incendie (milieu de la carte par défaut)
)RAW";
        exit(EXIT_SUCCESS);
    }
    ParamsType params;
    analyze_arg(nargs, args, params);
    return params;
}

bool check_params(ParamsType& params)
{
    bool flag = true;
    if (params.length <= 0)
    {
        std::cerr << "[ERREUR FATALE] La longueur du terrain doit être positive et non nulle !" << std::endl;
        flag = false;
    }

    if (params.discretization <= 0)
    {
        std::cerr << "[ERREUR FATALE] Le nombre de cellules par direction doit être positive et non nulle !" << std::endl;
        flag = false;
    }

    if ( (params.start.row >= params.discretization) || (params.start.column >= params.discretization) )
    {
        std::cerr << "[ERREUR FATALE] Mauvais indices pour la position initiale du foyer" << std::endl;
        flag = false;
    }
    
    return flag;
}

void display_params(ParamsType const& params)
{
    std::cout << "Parametres définis pour la simulation : \n"
              << "\tTaille du terrain : " << params.length << std::endl 
              << "\tNombre de cellules par direction : " << params.discretization << std::endl 
              << "\tVecteur vitesse : [" << params.wind[0] << ", " << params.wind[1] << "]" << std::endl
              << "\tPosition initiale du foyer (col, ligne) : " << params.start.column << ", " << params.start.row << std::endl;
}

int main( int nargs, char* args[] )
{
    std::cout << "Starting process\n";

    // Mise en place de l'environnement MPI

    MPI_Init( &nargs, &args );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);

	MPI_Status status;
    int return_status;
    char error_string[MPI_MAX_ERROR_STRING];
    int length_of_error_string;

    auto params = parse_arguments(nargs-1, &args[1]);
    if (!check_params(params)) return EXIT_FAILURE;
    
    int Loop;

    if (rank == 0) {
        display_params(params);
        auto displayer = Displayer::init_instance( params.discretization, params.discretization );
        SDL_Event event;

        std::vector<std::uint8_t> vegetation_map(params.discretization * params.discretization, 255u); 
        std::vector<std::uint8_t> fire_map(params.discretization * params.discretization, 0);

        while (true) {    
            // Synchronisation
            MPI_Recv(&Loop, 1, MPI_INT, 1, 1, globComm, &status);
            if(! Loop) {
                break;
            }
            return_status = MPI_Recv(vegetation_map.data(),vegetation_map.size(),MPI_UINT8_T,1,101,globComm,&status);
            if (return_status != MPI_SUCCESS) {
                MPI_Error_string(return_status, error_string, &length_of_error_string);
                printf("MPI_Recv failed for vegetal: %s\n", error_string);
                exit(-1);
            }
            return_status = MPI_Recv(fire_map.data(),fire_map.size(),MPI_UINT8_T,1,102,globComm,&status);
            if (return_status != MPI_SUCCESS) {
                MPI_Error_string(return_status, error_string, &length_of_error_string);
                printf("MPI_Recv failed for fire: %s\n", error_string);
                exit(-1);
            }
            displayer->update(vegetation_map, fire_map);
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
                MPI_Abort(globComm, EXIT_FAILURE);
                break;
            } 
            //std::this_thread::sleep_for(0.1s);
        }
    }
    
    if (rank == 1) {
        auto simu = Model( params.length, params.discretization, params.wind, params.start);    
        Loop = 1;

        while(simu.update()) {
            MPI_Send(&Loop, 1, MPI_INT, 0, 1, globComm);
            return_status = MPI_Send(simu.vegetal_map().data(),simu.vegetal_map().size(),MPI_UINT8_T,0,101,globComm);
            if (return_status != MPI_SUCCESS) {
                MPI_Error_string(return_status, error_string, &length_of_error_string);
                printf("MPI_Send failed for vegetal: %s\n", error_string);
                exit(-1);
            }
            return_status = MPI_Send(simu.fire_map().data(),simu.fire_map().size(),MPI_UINT8_T,0,102,globComm);
            if (return_status != MPI_SUCCESS) {
                MPI_Error_string(return_status, error_string, &length_of_error_string);
                printf("MPI_Send failed for fire: %s\n", error_string);
                exit(-1);
            }
            if ((simu.time_step() & 31) == 0) 
                std::cout << "Time step " << simu.time_step() << "\n===============" << std::endl;
        }
        // Signale la fin de la simulation
        Loop = 0;
        MPI_Send(&Loop, 1, MPI_INT, 0, 1, globComm);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

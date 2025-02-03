#

make calcul_pi_seq.exe calcul_pi_OpenMP.exe calcul_pi_MPI.exe

(
echo "Séquentiel :"
for nbSamples in 10000000 10000000000
do
    echo "nbSamples = " $nbSamples
    ./calcul_pi_seq.exe $nbSamples
done

echo "OpenMP :"
for nbSamples in 10000000 10000000000
do
    for threads in 1 2 4 8
    do
        echo "nbSamples = " $nbSamples
        echo "nbThreads = " $threads
        OMP_NUM_THREADS=$threads ./calcul_pi_OpenMP.exe $nbSamples
    done
done

echo "MPI :"
for nbSamples in 10000000 10000000000
do
    for threads in 1 2 4 8
    do
        echo "nbSamples = " $nbSamples
        echo "nbThreads = " $threads
        mpirun -np $threads --host localhost:$threads ./calcul_pi_MPI.exe $nbSamples
    done
done
) | tee results_pi.txt

#

N=200
date >> results.txt
echo "N = " $N

for threads in 1 2 3 4
do 
    echo "Nb threads" $threads
    export OMP_NUM_THREADS=$threads
    mpirun -n 2 --bind-to none ./simulation.exe -n $N 
done >> results.txt
#

N=200
date >> results.txt
echo "N = " $N

export OMP_NUM_THREADS=4

mpirun -n 2 --bind-to none ./simulation.exe -n $N >> results.txt
#

N=200
date >> results.txt
echo "N = " $N

mpirun -n 2 ./simulation.exe -n $N >> results.txt
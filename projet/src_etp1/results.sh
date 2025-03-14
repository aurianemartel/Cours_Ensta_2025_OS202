#

N=200
date >> results.txt
echo "N = " $N
for threads in 1 2 4
do
    echo "nbThreads = " $threads
    mkdir r$threads
    OMP_NUM_THREADS=$threads ./simulation.exe -n $N
    mv dump*.txt r$threads
done >> results.txt
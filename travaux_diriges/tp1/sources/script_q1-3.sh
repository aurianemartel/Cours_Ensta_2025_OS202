#

for threads in 1 2 3 4 5 6 7 8
do 
    echo $threads threads
    for n in 1024 2048 512 4096
    do
        echo "n=" $n 
        OMP_NUM_THREADS=$threads ./TestProductMatrix.exe $n
    done
done | tee rq1-3.txt

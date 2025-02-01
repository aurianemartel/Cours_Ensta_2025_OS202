#

for szBlock in 1024 512 256 128
do
    echo "szBlock =" $szBlock
    for threads in 1 4 8
    do
        echo $threads threads
        for n in 1024 2048 512 4096
        do
            echo "n=" $n 
            OMP_NUM_THREADS=$threads ./TestProductMatrix.exe $n $szBlock
        done
    done
done | tee rq1-7.txt
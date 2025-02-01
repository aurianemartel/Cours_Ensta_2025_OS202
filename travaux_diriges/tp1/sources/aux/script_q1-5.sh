#

for szBlock in 4096 32 64 128 256 512 1024
do 
    echo "szBlock =" $szBlock
    for n in 1024 2048 512 4096
    do
        echo "n=" $n 
        ./TestProductMatrix.exe $n $szBlock
    done
done | tee rq1-5.txt
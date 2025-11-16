reps=25
folderOutput="YOUR_FOLDER"

gcc -o 'YOUR_CODE' 'YOUR_CODE.c' -O3 -lgsl -lgslcblas -lm

for i in $(seq 1 $reps); do
    ./YOUR_CODE
    fileOutput=$(ls -t1 | head -n1)
    mv "$fileOutput" "$folderOutput/"
done

#!/bin/bash


for file in data/*.in; do
    echo "$(date +"%T") ----- ${file} -----"

    # Get file without extenstion
    f="${file%.in}";

    # Exact time
    # time ./main < ${file} | diff ${f}.out -

    ./automaton.out < ${file} | diff ${f}.out -

    echo "$(date +"%T") ----- DONE -----";
    echo "";
done;

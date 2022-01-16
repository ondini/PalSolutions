#!/bin/bash

TG=match.out

for file in data/*.in; do
    printf "%s ___ STARTED comparison for file: %s ____ \n" "$(date +'%T')" "$file"

    name="${file%.in}";

    # With time
    # ./$TG < ${file} | diff ${name}.out -

    # Without time
    ./$TG < ${file} | diff ${name}.out -

    printf "%s ___ FINISHED comparison  ____ \n\n" "$(date +'%T')"
done;

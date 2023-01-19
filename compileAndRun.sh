#!/bin/bash

# Compile the C files

gcc -g -Wall -o serial SerialProgram.c -lpthread -lm
gcc -g -Wall -o mutex PthreadMutex.c -lpthread -lm
gcc -g -Wall -o rwlock PthreadReadWriteLock.c -lpthread -lm

# Run the program
if [[ $? -eq 0 ]]; then
    for case in 1 2 3; do
        ./serial $case 1000
        for num_threads in 1 2 4 8; do
            ./mutex $case 1000 $num_threads
            ./rwlock $case 1000 $num_threads
        done
    done
fi

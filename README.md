# Concurrent-Lab-1

## Prerequisites

- Make sure you have GCC compiler installed on your machine. You can check if it's installed by running gcc --version in the command line.
- Make sure you have pthread and math libraries installed on your machine.
- Make sure you have a bash shell.

## Compiling and Running the program

1. Download the `SerialProgram.c`, `PthreadMutex.c`, `PthreadReadWriteLock.c` and the compileAndRun.sh files.
2. Open a terminal and navigate to the directory where the files are saved.
3. Make the script executable by running `chmod +x compileAndRun.sh`
4. Run the script by executing `./compileAndRun.sh`

This script will compile the C files and run the three executable files (serial, mutex, and rwlock) with different input arguments. The first argument is the case number (1, 2 or 3), the second argument is the number of operations (100), and the third argument is the number of threads (1, 2, 4, or 8) for the parallel programs. The script runs all the possible combinations of inputs for the three programs.

## Note

- You can change the input arguments in the script as per your requirement.
- You can also add or remove some cases from the script as per

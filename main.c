#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>




int main(int argc, char *argv[]){
    
    // check if the user provided enough arguments
    // first argument is the name of the program and second is the size of the file we want to create
    if (argc != 2){
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }

    // convert the second argument to an integer
    int size = atoi(argv[1]);
    if (size <= 0){
        printf("Size must be a positive integer\n");
        return 1;
    }
    // create an array in the size given by the user to write to the file
    int *array = malloc(size * sizeof(int));
    if (array == NULL){
        printf("Memory allocation failed\n");
        return 1;
    }
    // fill the array with random numbers
    for (int i = 0; i < size; i++){
        array[i] = rand() % 100;
    }


    // create a file using fb (lazy writing) without using fopen only the unix system calls
    // open the file using open() system call
    int fd = open("testfile.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd == -1){
        printf("Error opening file\n");
        free(array);
        return 1;
    }
    
    // repeating this part for the number of times specified by the user or given as a parameter

    //// read the file sequentially 

    //// read the file randomly

    // compare the time taken for both sequential and random reads and print the results
}
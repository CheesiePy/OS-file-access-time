#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 512

int main(int argc, char *argv[]){
    
    // check if the user provided enough arguments
    // first argument is the name of the program and second is the size of the file we want to create
    if (argc != 2){
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }

    // convert the second argument to an integer
    int block_num = atoi(argv[1]);
    if (block_num <= 0){
        printf("Size must be a positive integer\n");
        return 1;
    }

    // create an array in the size given by the user to write to the file
    int *array = malloc(block_num * sizeof(int));
    if (array == NULL){
        printf("Memory allocation failed\n");
        return 1;
    }
    // fill the array with random numbers
    for (int i = 0; i < block_num; i++){
        array[i] = rand() % 100;
    }


    // create a file using fb (lazy writing) without using fopen only the unix system calls
    int fd = open("testfile.txt", O_RDWR | O_CREAT); // O_RDWR for read and write, O_CREAT to create the file if it doesn't exist 

    if (fd == -1){
        printf("Error opening file\n");
        free(array);
        return 1;
    }

    int iterations = 1000; // number of iterations to write to the file
    // sequentially write the array to the file
    // time start
    struct timeval start, end;
    gettimeofday(&start, NULL);


    for (int i = 0; i < iterations; i++){
        // write the array to the file
        ssize_t bytes_written = write(fd, array, block_num * sizeof(int));
        if (bytes_written == -1){
            printf("Error writing to file\n");
            close(fd);
            free(array);
            return 1;
        }
    }
    // flush the file descriptor to ensure all data is written to the file
    if (fsync(fd) == -1){
        printf("Error flushing file descriptor\n");
        close(fd);
        free(array);
        return 1;
    }
    // close the file descriptor
    close(fd);
    // time end
    gettimeofday(&end, NULL);
    // calculate the time taken to write the file
    long seconds_s = end.tv_sec - start.tv_sec;
    long microseconds_s = end.tv_usec - start.tv_usec;
    long elapsed_s = seconds_s * 1000000 + microseconds_s;
    printf("Time taken to write the file sequentially: %ld microseconds\n", elapsed_s);

    // open the file again for reading
    fd = open("testfile.txt", O_RDONLY);
    if (fd == -1){
        printf("Error opening file\n");
        free(array);
        return 1;
    }

    //// read the file randomly
    // time start
    gettimeofday(&start, NULL);

    for (int i = 0; i < iterations; i++){
        // seek to a random position in the file
        off_t offset = rand() % (block_num * sizeof(int));
        if (lseek(fd, offset, SEEK_SET) == -1){
            printf("Error seeking in file\n");
            close(fd);
            free(array);
            return 1;
        }
        // read the data from the file
        ssize_t bytes_read = read(fd, array, BLOCK_SIZE);
        if (bytes_read == -1){
            printf("Error reading from file\n");
            close(fd);
            free(array);
            return 1;
        }
    }
    // time end
    gettimeofday(&end, NULL);
    
    // calculate the time taken to read the file
    long seconds_r = end.tv_sec - start.tv_sec;
    long microseconds_r = end.tv_usec - start.tv_usec;
    long elapsed_r = seconds_r * 1000000 + microseconds_r;
    printf("Time taken to read the file randomly: %ld microseconds\n", elapsed_r);

    // compare the time taken for both sequential and random reads and print the results
    if (elapsed_s < elapsed_r){
        printf("Sequential write is faster than random read\n");
    } else {
        printf("Random read is faster than sequential write\n");
    }
    
    // close the file descriptor
    close(fd);
    // free the allocated memory
    free(array);
    return 0;
}
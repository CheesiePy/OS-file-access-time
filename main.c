#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BLOCK_SIZE 512

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("Usage: %s <number_of_blocks>\n", argv[0]);
        return 1;
    }

    int block_num = atoi(argv[1]);
    if (block_num <= 0){
        printf("Block number must be positive\n");
        return 1;
    }

    char block[BLOCK_SIZE];
    memset(block, 'A', BLOCK_SIZE); // fill the block with dummy data

    // Create and write the file
    int fd = open("testfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1){
        perror("open");
        return 1;
    }
    for (int i = 0; i < block_num; i++) {
        if (write(fd, block, BLOCK_SIZE) != BLOCK_SIZE){
            perror("write");
            close(fd);
            return 1;
        }
    }
    fsync(fd);
    close(fd);

    // Sequential read
    fd = open("testfile.txt", O_RDONLY);
    if (fd == -1){
        perror("open for read");
        return 1;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < block_num; i++) {
        if (read(fd, block, BLOCK_SIZE) != BLOCK_SIZE){
            perror("sequential read");
            close(fd);
            return 1;
        }
    }
    gettimeofday(&end, NULL);
    long elapsed_seq_read = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Sequential read took: %ld microseconds\n", elapsed_seq_read);
    close(fd);

    // Random read
    fd = open("testfile.txt", O_RDONLY);
    if (fd == -1){
        perror("open for random read");
        return 1;
    }

    gettimeofday(&start, NULL);
    for (int i = 0; i < block_num; i++) {
        off_t offset = (rand() % block_num) * BLOCK_SIZE;
        if (lseek(fd, offset, SEEK_SET) == -1){
            perror("lseek");
            close(fd);
            return 1;
        }
        if (read(fd, block, BLOCK_SIZE) != BLOCK_SIZE){
            perror("random read");
            close(fd);
            return 1;
        }
    }
    gettimeofday(&end, NULL);
    long elapsed_rand_read = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Random read took: %ld microseconds\n", elapsed_rand_read);

    close(fd);
    return 0;
}

// Compile with: gcc -o main main.c
// Run with: ./main <number_of_blocks>
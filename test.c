#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_RUNS 5
#define MAX_LINE_LEN 256
#define BLOCK_SIZE 512

int main() {
    int block_counts[] = {1000, 10000, 100000, 1000000}; // number of 512-byte blocks
    int num_tests = sizeof(block_counts) / sizeof(block_counts[0]);

    printf("===== File Access Benchmark (Average over %d runs) =====\n", NUM_RUNS);
    printf("Fixed block size: %d bytes\n", BLOCK_SIZE);

    for (int i = 0; i < num_tests; i++) {
        int blocks = block_counts[i];
        long total_seq = 0, total_rand = 0;

        printf("\n--- Testing with %d blocks (%.2f MB) ---\n", blocks, (blocks * BLOCK_SIZE) / (1024.0 * 1024));

        for (int j = 0; j < NUM_RUNS; j++) {
            char command[128];
            snprintf(command, sizeof(command), "./main %d", blocks);

            FILE *fp = popen(command, "r");
            if (fp == NULL) {
                perror("popen failed");
                return 1;
            }

            char line[MAX_LINE_LEN];
            long seq_time = -1, rand_time = -1;

            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "Sequential read took:")) {
                    sscanf(line, "Sequential read took: %ld microseconds", &seq_time);
                }
                if (strstr(line, "Random read took:")) {
                    sscanf(line, "Random read took: %ld microseconds", &rand_time);
                }
            }

            pclose(fp);

            if (seq_time >= 0 && rand_time >= 0) {
                total_seq += seq_time;
                total_rand += rand_time;
            } else {
                fprintf(stderr, "Error: could not parse time from output (run %d, blocks: %d)\n", j + 1, blocks);
                return 1;
            }
        }

        long avg_seq = total_seq / NUM_RUNS;
        long avg_rand = total_rand / NUM_RUNS;

        printf("Average Sequential Read Time: %ld μs\n", avg_seq);
        printf("Average Random Read Time:     %ld μs\n", avg_rand);
    }

    printf("\n===== All tests completed successfully =====\n");
    remove("testfile.txt"); // Clean up the test file
    return 0;
}

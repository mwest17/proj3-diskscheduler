#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALGORITHM_STRING_SIZE 6



int max(int a, int b) {
    return (a > b) ? a : b;
}


// // FCFS (First Come, First Served): Serve pending requests by the input order of the requests.
// void FCFS(int n, int h, int d, int r,
//           const int* arrival_time,      // Array of arrival times
//           const int* requested_number,  // Array of requested track numbers
//           int* track_ordering,          // Ordering of served tracks
//           int* total_seek_time,               // Total seek time
//           int* last_completion_time) {  // Completion time of last served request  
//     int cur_time = 0;
//     *total_seek_time = 0;
//     int prev = 0;
//     for (int i = 0; i < r; i++) {
        
//         // Choose a request
//         int c = i;
//         cur_time += max(arrival_time[c] - cur_time, 0); // Add any wait time
//         track_ordering[c] = requested_number[c];
        
//         int seek_time;
//         if (i > 0) {
//             // Find seek time from previous position
//             seek_time = abs(track_ordering[c] - track_ordering[prev]);
//         } else {
//             // Seek time from starting position
//             seek_time = abs(track_ordering[c] - 50);
//         }
//         *total_seek_time += seek_time;
//         cur_time += seek_time;

//         prev = c;
//     }
//     *last_completion_time = cur_time;
// }

int FCFS(int n, int h, int d, int r,
         const int* arrival_time,      // Array of arrival times
         const int* requested_number,  // Array of requested track numbers
         int* pending,
         int cur_iteration,
         int prev) {

    return cur_iteration;
}


void scheduler(int n, int h, int d, int r,
               const int* arrival_time,      // Array of arrival times
               const int* requested_number,  // Array of requested track numbers
               int (*scheduling_rule)(int, int, int, int, const int*, const int*, int*, int, int),         // Ptr to scheduler algorithm ruleset
               int* track_ordering,          // Ordering of served tracks
               int* total_seek_time,         // Total seek time
               int* last_completion_time) {  // Completion time of last served request  
    int cur_time = 0;
    *total_seek_time = 0;
    int prev = -1, prev_start = -1;
    int pending[r];

    for (int i = 0; i < r; i++) {
        // Add any newly available jobs to the availble list
        // for (int i = 0; i < r; i++) {
        //     if (prev_start < arrival_time[i] && arrival_time[i] <= cur_time) {
        //         append(i);
        //     }
        // }

        // Choose a request
        int c = (*scheduling_rule)(n, h, d, r, arrival_time, requested_number, pending, i, prev);
        // Add any wait time
        cur_time += max(arrival_time[c] - cur_time, 0);
        track_ordering[c] = requested_number[c];
        
        int seek_time;
        if (i > 0) {
            // Find seek time from previous position
            seek_time = abs(track_ordering[c] - track_ordering[prev]);
        } else {
            // Seek time from starting position
            seek_time = abs(track_ordering[c] - 50);
        }
        *total_seek_time += seek_time;
        cur_time += seek_time;

        prev = c;
    }
    *last_completion_time = cur_time;
}


int main() {
    char alg[ALGORITHM_STRING_SIZE + 1] = {'\0'};
    int n = 0, h = 0, d = 0, r = 0;

    // Read in algorithm ALG, number tracks N, initial head position H
    scanf("%s %d %d", alg, &n, &h);

    // Read initial head direction D
    if (strcmp(alg, "FCFS") != 0 && !strcmp(alg, "SSTF") != 0) {
        scanf("%d", &d);
    }

    // Read in number of requests R
    scanf("%d", &r);
    // printf("%d\n", r);

    // Read in all requests
    int arrival_time[r],  requested_number[r];
    for (int i = 0; i < r; i++) {
        scanf("%d %d", &arrival_time[i], &requested_number[i]);
    } 

    int track_ordering[r];
    int seek_time = 0;
    int last_completion_time = 0;
    if (strcmp(alg, "FCFS") == 0) {
        // Run FCFS Algorithm
        scheduler(n, h, d, r, arrival_time, requested_number, FCFS, track_ordering, &seek_time, &last_completion_time);
    } else if (strcmp(alg, "SSTF") == 0) {
        // Run SSTF Algorithm

    } else  if (strcmp(alg, "SCAN") == 0) {
        // Run SCAN Algorithm

    } else  if (strcmp(alg, "C-SCAN") == 0) {
        // Run C-SCAN Algorithm

    } else  if (strcmp(alg, "LOOK") == 0) {
        // Run LOOK Algorithm

    } else  if (strcmp(alg, "C-LOOK") == 0) {
        // Run C-LOOK Algorithm

    }

    // Print all output
    for (int i = 0; i < r; i++) {
        // printf("Correct: %d ", requested_number[i]);
        printf("%d ", track_ordering[i]);
    }
    printf("\n");
    printf("%d\n", seek_time);
    printf("%d\n", last_completion_time);
}
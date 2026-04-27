#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define ALGORITHM_STRING_SIZE 6

typedef struct {
    int* arr;
    int len;
    int size;
} vector;

void init(vector* v, int size) {
    v->arr = (int*) malloc(sizeof(int)*size);
    v->len = 0;
    v->size = size;
}

void append(vector* v, int val) {
    if (v->len < v->size) {
        v->arr[v->len] = val;
        v->len++;
    }
}

int delete(vector* v, int index) {
    int val = 0;
    if (index < v->size) {
        val = v->arr[index];
        for (int i = index; i < v->len - 1; i++) {
            v->arr[i] = v->arr[i + 1];
        }
        v->len--;
    }
    return val;
}

int print_vector(vector* v) {
    for (int i = 0; i < v->len; i++) {
        printf("%d ", v->arr[i]);
    }
    printf("\n");
}


int max(int a, int b) {
    return (a > b) ? a : b;
}


// FCFS (First Come, First Served): Serve pending requests by the input order of the requests.
int FCFS(int n, int h, int d, int r,
         const int* arrival_time,      
         const int* requested_number,
         vector* pending,
         int cur_iteration,
         int prev) {

    return cur_iteration;
}

int SSTF(int n, int h, int d, int r,
         const int* arrival_time,      
         const int* requested_number,
         vector* pending,
         int cur_iteration,
         int prev) {

    print_vector(pending);

    int prev_pos = (prev > -1) ? requested_number[prev] : 50;
    int closest_index = pending->len - 1, min_dist = INT_MAX;
    for (int i = 0; i < pending->len; i++) {
        // check if difference from requested_number[prev] to requested_number[i]
        int distance = abs(prev_pos - requested_number[pending->arr[i]]);
        if (distance < min_dist || (distance == min_dist && requested_number[pending->arr[i]] < requested_number[pending->arr[closest_index]])){
            closest_index = i;
            min_dist = distance;
        }
    }

    int selected = cur_iteration;
    if (closest_index != -1) {
        selected = pending->arr[closest_index];
        delete(pending, closest_index);
    }
    return selected;
}



void scheduler(int n, int h, int d, int r,
               const int* arrival_time,      // Array of arrival times
               const int* requested_number,  // Array of requested track numbers
               int (*scheduling_rule)(int, int, int, int, const int*, const int*, vector*, int, int),         // Ptr to scheduling algorithm function
               int* track_ordering,          // Ordering of served tracks
               int* total_seek_time,         // Total seek time
               int* last_completion_time) {  // Completion time of last served request  
    int cur_time = 0;
    *total_seek_time = 0;
    int prev = -1, prev_start = -1;
    vector pending;
    init(&pending, r);

    for (int i = 0; i < r; i++) {
        // Add any newly available jobs to the pending list
        for (int i = 0; i < r; i++) {
            if (prev_start < arrival_time[i] && arrival_time[i] <= cur_time) {
                append(&pending, i);
                // printf("New: %d ", i);
            }
        }
        // printf("\n");
        prev_start = cur_time;

        // Choose a request
        int c = (*scheduling_rule)(n, h, d, r, arrival_time, requested_number, &pending, i, prev);
        // Add wait time if there was any
        cur_time += max(arrival_time[c] - cur_time, 0);
        track_ordering[i] = requested_number[c];
        
        int seek_time;
        if (i > 0) {
            // Find seek time from previous position
            seek_time = abs(track_ordering[i] - track_ordering[i-1]);
        } else {
            // Seek time from starting position
            seek_time = abs(track_ordering[i] - 50);
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
    if (strcmp(alg, "FCFS") != 0 && strcmp(alg, "SSTF") != 0) {
        scanf("%d", &d);
    }

    // Read in number of requests R
    scanf("%d", &r);

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
        scheduler(n, h, d, r, arrival_time, requested_number, SSTF, track_ordering, &seek_time, &last_completion_time);
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define ALGORITHM_STRING_SIZE 6


// -----------------------------------------------------------------------------------------------------
//  Vector Data Structure
//  - Static size data structure that allows for easy appending and deletion
// ----------------------------------------------------------------------------------------------------- 
typedef struct {
    int* arr;
    int len;
    int size;
} vector;

// Initializes vector based on specified size
void vector_init(vector* v, int size) {
    v->arr = (int*) malloc(sizeof(int)*size);
    v->len = 0;
    v->size = size;
}

// Append val to end of v
void vector_append(vector* v, int val) {
    if (v->len < v->size) {
        v->arr[v->len] = val;
        v->len++;
    }
}

// Delete element at index, shift right values to fill in space. Returns the element deleted
int vector_delete(vector* v, int index) {
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

int vector_print(vector* v) {
    for (int i = 0; i < v->len; i++) {
        printf("%d ", v->arr[i]);
    }
    printf("\n");
}



// -----------------------------------------------------------------------------------------------------
//  Helper Functions 
// -----------------------------------------------------------------------------------------------------

// Returns max of a and b
int max(int a, int b) {
    return (a > b) ? a : b;
}


// Improved modulo that can properly handle negative remainders (ie convert them to their equivalent positive counterpart)
int modulo(int m, int n) {
    if (m >= 0) {
        return m % n;
    } else {
        return (m % n) + n;    
    }   
}



// -----------------------------------------------------------------------------------------------------
//  Non-directional Scheduling Algorithms
// -----------------------------------------------------------------------------------------------------

// Next job selection algorithms:

// FCFS (First Come, First Served): Serve pending requests by the input order of the requests.
int FCFS(int n, int h, int d, int r,
         const int* arrival_time,      
         const int* requested_number,
         vector* pending,
         int cur_iteration,
         int prev) {

    // Since input is given in order of arrival time, just choose next available job.
    return cur_iteration;
}


// SSTF (Shortest Seek Time First)
int SSTF(int n, int h, int d, int r,
         const int* arrival_time,      
         const int* requested_number,
         vector* pending,
         int cur_iteration,
         int prev) {
    // Save previous position and initialize closest index and distance
    int prev_pos = (prev > -1) ? requested_number[prev] : h;
    int closest_index = pending->len - 1, min_dist = INT_MAX;

    // Must find the available track number closest to the previous position
    for (int i = 0; i < pending->len; i++) {
        // Find distance between
        int distance = abs(prev_pos - requested_number[pending->arr[i]]);
        // Choose this if distance is smaller or if equal that the index is less (tiebreaker)
        if (distance < min_dist || (distance == min_dist && requested_number[pending->arr[i]] < requested_number[pending->arr[closest_index]])){
            closest_index = i;
            min_dist = distance;
        }
    }

    // Delete the the closest index (if pending is empty, then selected is just next available job)
    int selected = cur_iteration;
    if (closest_index != -1) {
        selected = pending->arr[closest_index];
        vector_delete(pending, closest_index);
    }
    return selected;
}


// Template scheduler for non-directional algorithms
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
    vector_init(&pending, r);

    for (int i = 0; i < r; i++) {
        #ifdef DEBUG
        printf("\nIteration: %d, cur_time: %d\n", i, cur_time);
        #endif

        // Add any newly available jobs to the pending list
        for (int i = 0; i < r; i++) {
            if (prev_start < arrival_time[i] && arrival_time[i] <= cur_time) {
                vector_append(&pending, i);
            }
        }

        // Choose a request based on given algorithm
        int c = (*scheduling_rule)(n, h, d, r, arrival_time, requested_number, &pending, i, prev);
        // Add wait time if there was any
        cur_time += max(arrival_time[c] - cur_time, 0);
        prev_start = cur_time;
        track_ordering[i] = requested_number[c];
        
        int seek_time;
        if (i > 0) {
            // Find seek time from previous position
            seek_time = abs(track_ordering[i] - track_ordering[i-1]);
        } else {
            // Seek time from starting position
            seek_time = abs(track_ordering[i] - h);
        }
        *total_seek_time += seek_time;
        cur_time += seek_time;

        prev = c;
    }
    // Ending time is completion time
    *last_completion_time = cur_time;
}



// -----------------------------------------------------------------------------------------------------
//  Directional Scheduling Helper Functions
// -----------------------------------------------------------------------------------------------------

// Calculates the distance from the cur_pos the target_position given direction and number of tracks
int find_distance(int cur_pos, int target_pos, int n, int d) {
    int seek_time;
    if (d == 1) {
        // forward
        seek_time = modulo(target_pos - cur_pos, n);
    }
    else if (d == -1) {
        // backward
        seek_time = modulo(cur_pos - target_pos, n);
    }
    
    return seek_time; 
}


// Based on direction and position of request, adds request to forward or backward pending vector
void add_request(int index, int requested_track, int cur_pos, int dir, vector* forward, vector* backward) {
    vector* pending;
    if (dir == 1) { // UP
        pending = (requested_track >= cur_pos) ? forward : backward;
    } else if (dir == -1) { //DOWN
        pending = (requested_track <= cur_pos) ? forward : backward;
    }

    vector_append(pending, index);
}


// Chooses the closest request in the pending queue
int choose_closest(int d, int n, int cur_iter,
                   const int* requested_number,
                   int* track_ordering,
                   vector* pending,
                   int cur_pos,
                   int* count) {
    int here = -1;
    int closest_index = 0, min_dist = INT_MAX;
    for (int i = pending->len - 1; i >= 0; i--) {
        // Get distance from element
        int distance = find_distance(cur_pos, requested_number[pending->arr[i]], n, d);
        if (distance < min_dist && distance > 0){
            // If close than minimum, save
            closest_index = i;
            min_dist = distance;
        } else if (distance == 0) {
            // If we are already on it, mark as complete
            here = i;
            track_ordering[cur_iter] = requested_number[pending->arr[i]];
            (*count) += 1;
        }
    }

    int selected = 0;
    if (here != -1) { // If we deleted one we were already on
        // Delete based on position in pending queue, so indecies aren't messed up
        if (here > closest_index) {
            vector_delete(pending, here);
            selected = vector_delete(pending, closest_index);
        } else {
            selected = vector_delete(pending, closest_index);
            vector_delete(pending, here);
        }
    } else {
        // If we did not delete one we were already on
        selected = vector_delete(pending, closest_index);
    }

    return selected;
}



// -----------------------------------------------------------------------------------------------------
//  Directional Scheduling Direction Change Handlers
// -----------------------------------------------------------------------------------------------------

int SCAN(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    // Move head until it reaches an endpoint, add seek time spent
    int seek_time;
    if (*d == 1) {
        seek_time = find_distance(*cur_pos, n-1, n, *d);
        *cur_time += seek_time;
        *cur_pos = n - 1;
    } else {
        seek_time = find_distance(*cur_pos, 0, n, *d);
        *cur_time += seek_time;
        *cur_pos = 0;
    }

    // change direction d *= -1
    *d *= -1;

    // swap pending vectors (forward is now backwards and backwards is not forwards)
    vector* tmp = *pending_forward;
    *pending_forward = *pending_backward;
    *pending_backward = tmp;

    return seek_time;
}


int C_SCAN(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    
    // Move head until it reaches an endpoint, add seek time spent
    // Then move to other endpoint and add that time
    int seek_time;
    if (*d == 1) {
        seek_time = find_distance(*cur_pos, n - 1, n, *d);
        *cur_pos = n - 1;
        seek_time += find_distance(*cur_pos, 0, n, -1*(*d));
        *cur_time += seek_time;
        *cur_pos = 0;
    } else {
        seek_time = find_distance(*cur_pos, 0, n, *d);
        *cur_pos = 0;
        seek_time += find_distance(*cur_pos, n-1, n, -1*(*d));
        *cur_time += seek_time;
        *cur_pos = n - 1;
    }

    // Place all backwards in forward as now there is nothing behind us
    for (int i = (*pending_backward)->len - 1; i >= 0; i--) {
        vector_append(*pending_forward, vector_delete(*pending_backward, i));
    }

    return seek_time;
}


int LOOK(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    // change direction d *= -1
    *d *= -1;
    
    // swap pending vectors
    vector* tmp = *pending_forward;
    *pending_forward = *pending_backward;
    *pending_backward = tmp;

    return 0;
}


int C_LOOK(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    // Find the furthest request behind cur_pos
    int furthest, max = 0;
    for (int i = 0; i < (*pending_backward)->len; i++) {
        int pos = requested_number[(*pending_backward)->arr[i]];
        int dist = find_distance(*cur_pos, pos, n, -1*(*d));
        if (dist > max) { // Save one that is furthest
            furthest = pos;
            max = dist;
        }
    }
    
    // Traverse to the furthest backwards
    int seek_time = find_distance(*cur_pos, furthest, n, -1*(*d));
    // Add seek time
    *cur_time += seek_time;
    *cur_pos = furthest;

    // Place all backwards in forward, since nothing is behind us anymore
    for (int i = (*pending_backward)->len - 1; i >= 0; i--) {
        vector_append(*pending_forward, vector_delete(*pending_backward, i));
    }

    return seek_time;
}

// Template scheduler for all directional algorithms
// Must be supplied with an algorithm that determines what happens when there is 
// nothing pending ahead, but there is something pending behind
void directional_scheduler(int n, int h, int d, int r,
               const int* arrival_time,      // Array of arrival times
               const int* requested_number,  // Array of requested track numbers
               int (*algo)(int, int*, int*, int*, vector**, vector**, const int*),         // Ptr to direction switch handler
               int* track_ordering,          // Ordering of served tracks
               int* total_seek_time,         // Total seek time
               int* last_completion_time) {
    int cur_pos = h, cur_time = 0;
    vector *pending_forward = (vector*) malloc(sizeof(vector)*2), *pending_backward;
    pending_backward = pending_forward + 1;
    vector_init(pending_forward, r);
    vector_init(pending_backward, r);

    for (int i = 0; i < r;) {
        #ifdef DEBUG
        printf("cur_time: %d, seek_time: %d, cur_pos: %d\n", cur_time, *total_seek_time, cur_pos);
        #endif
        int start_time = cur_time;

        if (pending_forward->len > 0) {
            // Case 1: There are pending requests in head direction, then serve the nearest one
            #ifdef DEBUG
            printf("1st case\n");
            #endif

            // Choose nearest in pending forward, c
            int count = 0;
            int c = choose_closest(d, n, i, requested_number, track_ordering, pending_forward, cur_pos, &count);
            // Add any that were compeleted without moving
            i += count;
            // Add to ordering of completed
            track_ordering[i] = requested_number[c];

            // Find seek time from previous position
            int seek_time = find_distance(cur_pos, track_ordering[i], n, d);
            *total_seek_time += seek_time;
            cur_time += seek_time;
            cur_pos = track_ordering[i];

            // Add any new job arrivals from range start_time to cur_time
            for (int j = 0; j < r; j++) {
                if (start_time < arrival_time[j] && arrival_time[j] <= cur_time) {
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }
        
            i++;
        } else if (pending_backward->len > 0) {
            // Case 2: There are pending requests in opposite direction, then perform the algorithm specific logic
            #ifdef DEBUG
            printf("2nd case\n");
            #endif

            // Function call to opposite direction logic
            *total_seek_time += algo(n, &d, &cur_time, &cur_pos, &pending_forward, &pending_backward, requested_number);

            // Add any new job arrivals from range start_time to cur_time
            for (int j = 0; j < r; j++) {
                if (start_time < arrival_time[j] && arrival_time[j] <= cur_time) {
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }

            #ifdef DEBUG
            vector_print(pending_forward);
            vector_print(pending_backward);
            #endif
        } else {
            #ifdef DEBUG
            printf("3rd case\n");
            #endif

            // Wait for next track request (ie one at i)
            cur_time = arrival_time[i];
            for (int j = i; j < r; j++) {
                if (arrival_time[j] == cur_time) {
                    // Add all that arrive at that time to the queues
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }
            #ifdef DEBUG
            vector_print(pending_forward);
            vector_print(pending_backward);
            #endif
        }
    }
    *last_completion_time = cur_time;   
}






int main() { 
    char alg[ALGORITHM_STRING_SIZE + 1] = {'\0'};
    char direction[5] = {'\0'};
    int n = 0, h = 0, d = 0, r = 0;

    // Read in algorithm ALG, number tracks N, initial head position H
    scanf("%s %d %d", alg, &n, &h);

    // Read initial head direction D
    if (strcmp(alg, "FCFS") != 0 && strcmp(alg, "SSTF") != 0) {
        scanf("%s", &direction);
        if (strcmp(direction, "UP") == 0) {
            d = 1;
        } else if (strcmp(direction, "DOWN") == 0) {
            d = -1;
        }
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
        directional_scheduler(n, h, d, r, arrival_time, requested_number, SCAN, track_ordering, &seek_time, &last_completion_time);
    } else  if (strcmp(alg, "C-SCAN") == 0) {
        // Run C-SCAN Algorithm
        directional_scheduler(n, h, d, r, arrival_time, requested_number, C_SCAN, track_ordering, &seek_time, &last_completion_time);
    } else  if (strcmp(alg, "LOOK") == 0) {
        // Run LOOK Algorithm
        directional_scheduler(n, h, d, r, arrival_time, requested_number, LOOK, track_ordering, &seek_time, &last_completion_time);
    } else  if (strcmp(alg, "C-LOOK") == 0) {
        // Run C-LOOK Algorithm
        directional_scheduler(n, h, d, r, arrival_time, requested_number, C_LOOK, track_ordering, &seek_time, &last_completion_time);
    }

    // Print all output
    for (int i = 0; i < r; i++) {
        printf("%d ", track_ordering[i]);
    }
    printf("\n");
    printf("%d\n", seek_time);
    printf("%d\n", last_completion_time);
}
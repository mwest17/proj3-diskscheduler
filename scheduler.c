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

void vector_append(vector* v, int val) {
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

    int prev_pos = (prev > -1) ? requested_number[prev] : h;
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
        // printf("\nIteration: %d, cur_time: %d\n", i, cur_time);
        for (int i = 0; i < r; i++) {
            if (prev_start < arrival_time[i] && arrival_time[i] <= cur_time) {
                vector_append(&pending, i);
            }
        }

        // print_vector(&pending);

        // Choose a request
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
    *last_completion_time = cur_time;
}




/*
Add any pending requests that are in direction of head (requested_pos - cur_pos )to a vector (OF HEAD DIRECTION)
Create a 2nd pending vector (OPPOSITE HEAD DIRECTION)

If 1st is nonempty, serve the nearest one of the 1st

If pending vector is empty and 2nd is nonempty,
    ## Different behavior per type



If both are empty, return cur_iter and jump to it.
    

Movement to a track is handled by smaller functions. 
This determines the seek time, resulting direction, and if lists get switched.
SCAN:
    cur_pos = H
    for all requests    
        if there are any pending in head direction (serve logic)
            choose nearest one
            move to it
            Add any vectors that became available since last job selection to vectors
        else if there are any pending in other direction (direction chance logic)
            CHOOSE LOGIC HERE
                Move head until it reaches an endpoint
                add seek time spent
                change direction d *= -1
                swap pending vectors
            END CHOOSE LOGIC
            Add any vectors that became available since last job selection to vectors
            i--         
        else (wait logic)
            wait until next one(s)
            add tracks to vectors (should we choose the best ones if all arriving at same time?)
            i--
*/

// Seek time to a position based on the cur_pos, target position, N, and direction
int modulo(int m, int n) {
    if (m >= 0) {
        return m % n;
    } else {
        return (m % n) + n;    
    }   
}

int find_seek(int cur_pos, int target_pos, int n, int d) {
    int seek_time;
    if (d == 1) {
        // forward
        // Start at N-1, then go upwards towards others. So subtract distance mod n?
        seek_time = modulo(target_pos - cur_pos, n);
    }
    else if (d == -1) {
        // backward
        seek_time = modulo(cur_pos - target_pos, n);
    }
    
    return seek_time; 


}


void add_request(int index, int requested_track, int cur_pos, int dir, vector* forward, vector* backward) {
    // If positive, it is up, if negative it is down
    int distance = requested_track - cur_pos;

    vector* pending;
    // if (distance > 0) {
    //     pending = (dir == 1) ? forward: backward;
    // } else if (distance < 0) {
    //     pending = (dir == -1) ? forward: backward;
    // }
    if (dir == 1) {
        pending = (requested_track >= cur_pos) ? forward : backward;
    } else if (dir == -1) {
        pending = (requested_track <= cur_pos) ? forward : backward;
    }

    vector_append(pending, index);
}


int choose_closest(int d, int n, int cur_iter,
                   const int* requested_number,
                   int* track_ordering,
                   vector* pending,
                   int cur_pos,
                   int* count) {
    int here = -1;
    int closest_index = 0, min_dist = INT_MAX;
    for (int i = pending->len - 1; i >= 0; i--) {
        int distance = find_seek(cur_pos, requested_number[pending->arr[i]], n, d);//requested_number[pending->arr[i]] - cur_pos;
        if (distance < min_dist && distance > 0){
            closest_index = i;
            min_dist = distance;
        } else if (distance == 0) {
            here = i;
            track_ordering[cur_iter] = requested_number[pending->arr[i]];
            (*count) += 1;
        }
    }

    int selected = 0;
    if (here != -1) {
        if (here > closest_index) {
            delete(pending, here);
            selected = delete(pending, closest_index);
        } else {
            selected = delete(pending, closest_index);
            delete(pending, here);
        }
    } else {
        // printf("%d\n", closest_index);
        selected = delete(pending, closest_index);
    }

    return selected;
}


int SCAN(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    // Move head until it reaches an endpoint
    // add seek time spent
    int seek_time;
    if (*d == 1) {
        seek_time = (n - 1) - *cur_pos;
        *cur_time += seek_time;
        *cur_pos = n - 1;
    } else {
        seek_time = *cur_pos;
        *cur_time += seek_time;
        *cur_pos = 0;
    }

    // change direction d *= -1
    *d *= -1;
    // swap pending vectors
    vector* tmp = *pending_forward;
    *pending_forward = *pending_backward;
    *pending_backward = tmp;

    return seek_time;
}


int C_SCAN(int n, int* d, int* cur_time, int* cur_pos, vector** pending_forward, vector** pending_backward, const int* requested_number) {
    // Move head until it reaches an endpoint
    // add seek time spent
    int seek_time;
    if (*d == 1) {
        seek_time = find_seek(*cur_pos, n - 1, n, *d);
        *cur_pos = n - 1;
        seek_time += find_seek(*cur_pos, 0, n, -1*(*d));
        *cur_time += seek_time;
        *cur_pos = 0;
    } else {
        seek_time = find_seek(*cur_pos, 0, n, *d);
        *cur_pos = 0;
        seek_time += find_seek(*cur_pos, n-1, n, -1*(*d));
        *cur_time += seek_time;
        *cur_pos = n - 1;
    }

    // Place all backwards in forward
    for (int i = (*pending_backward)->len - 1; i >= 0; i--) {
        vector_append(*pending_forward, delete(*pending_backward, i));
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
    // Move head until it reaches an endpoint
    // add seek time spent
    int furthest, max = 0;
    for (int i = 0; i < (*pending_backward)->len; i++) {
        int pos = requested_number[(*pending_backward)->arr[i]];
        int dist = find_seek(*cur_pos, pos, n, -1*(*d));
        if (dist > max) {
            furthest = pos;
            max = dist;
        }
    }
    
    int seek_time = find_seek(*cur_pos, furthest, n, -1*(*d));
    *cur_time += seek_time;
    *cur_pos = furthest;

    // Place all backwards in forward
    for (int i = (*pending_backward)->len - 1; i >= 0; i--) {
        vector_append(*pending_forward, delete(*pending_backward, i));
    }

    return seek_time;
}


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
    init(pending_forward, r);
    init(pending_backward, r);

    for (int i = 0; i < r;) {
        #ifdef DEBUG
        printf("cur_time: %d, seek_time: %d, cur_pos: %d\n", cur_time, *total_seek_time, cur_pos);
        #endif
        int start_time = cur_time;

        if (pending_forward->len > 0) {
            #ifdef DEBUG
            printf("1st case\n");
            #endif
            // Choose nearest in pending forward, c
            int count = 0;
            int c = choose_closest(d, n, i, requested_number, track_ordering, pending_forward, cur_pos, &count);
            i += count;
            // printf("c: %d\n", c);
            track_ordering[i] = requested_number[c];

            // Find seek time from previous position
            int seek_time = find_seek(cur_pos, track_ordering[i], n, d);
            *total_seek_time += seek_time;
            cur_time += seek_time;
            cur_pos = track_ordering[i];

            // Add any new pending from range start_time to cur_time
            for (int j = 0; j < r; j++) {
                if (start_time < arrival_time[j] && arrival_time[j] <= cur_time) {
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }
        
            i++;
        } else if (pending_backward->len > 0) {
            #ifdef DEBUG
            printf("2nd case\n");
            #endif
            // Function call to opposite direction logic
            *total_seek_time += algo(n, &d, &cur_time, &cur_pos, &pending_forward, &pending_backward, requested_number);

            // Add any new pending from range start_time to cur_time
            for (int j = 0; j < r; j++) {
                if (start_time < arrival_time[j] && arrival_time[j] <= cur_time) {
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }

            #ifdef DEBUG
            print_vector(pending_forward);
            print_vector(pending_backward);
            #endif
        } else {
            #ifdef DEBUG
            printf("3rd case\n");
            #endif
            // Wait for next track request (ie one at i)
            cur_time = arrival_time[i];
            for (int j = i; j < r; j++) {
                if (arrival_time[j] == cur_time) {
                    add_request(j, requested_number[j], cur_pos, d, pending_forward, pending_backward);
                }
            }
            #ifdef DEBUG
            print_vector(pending_forward);
            print_vector(pending_backward);
            #endif
        }
    }
    *last_completion_time = cur_time;   
}



/*



    if d == -1 (down):
        move down until 0
    else if d == 1 (up):
        move up until n-1
    
    add previous to seek time
    d *= -1
    switch vectors
    then compute seek time to position as normal

C-SCAN 
    If d == -1 (down):
        move down until 0
    else if d == 1 (up):
        move up until n-1
    
    add previous to seek time
    also add an additional N/2
    d *= -1
    then compute seek time to position as normal

LOOK
    d *= -1
    switch vectors
    then compute seek time to position as normal

C-LOOK

*/



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
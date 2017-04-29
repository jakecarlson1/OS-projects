/**
 * Pager
 * By: Jake Carlson
 * 2017-04-06
 * This program implements FIFO, LRU, and OPT/MIN page-replacement algorithms.
 * A random page-refrence string is generated at the start of the program and
 * all of the algorithms run. The total number of pagefaults per algorithm is
 * reported at the end
 */

// includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

// defines
#define PAGE_REF_STRING_LENGTH 20
#define NUM_PAGE_FRAMES 5

// prototypes
int FIFO(int*, int);
int LRU(int*, int);
int OPT_MIN(int*, int);
int find_least_recent_pos(int*, int*, int, int);
int find_most_distant_pos(int*, int*, int, int);
int find_max_pos(int*, int);
void init_frame_table(int*, int);
void print_frame_table(int*, int);
bool in_arr(int*, int, int);

// main driver
int main(int argc, char const *argv[])
{
    // seed rand with time
    srand((unsigned int)time(NULL));

    // generate page reference string
    printf("\npage reference string: ");
    int page_ref_str[PAGE_REF_STRING_LENGTH];
    for (int i = 0; i < PAGE_REF_STRING_LENGTH; i++)
    {
        page_ref_str[i] = rand() % 10;
        printf("%i", page_ref_str[i]);
    }
    printf("\n\n");

    // run FIFO
    int FIFO_faults = FIFO(page_ref_str, NUM_PAGE_FRAMES);
    printf("\nFIFO faults: %i\n\n", FIFO_faults);

    // run LRU
    int LRU_faults = LRU(page_ref_str, NUM_PAGE_FRAMES);
    printf("\nLRU faults: %i\n\n", LRU_faults);

    // run OPT/MIN
    int OPT_MIN_faults = OPT_MIN(page_ref_str, NUM_PAGE_FRAMES);
    printf("\nOPT_MIN faults: %i\n\n", OPT_MIN_faults);

    return 0;
}

// uses FIFO strategy for replacing frames on page faults
int FIFO(int* page_ref_str, int num_frames)
{
    // create frame table
    int frames[num_frames];
    init_frame_table(frames, num_frames);

    int num_faults = 0;
    int last_paged = 0;

    // step over the reference string
    for (int i = 0; i < PAGE_REF_STRING_LENGTH; i++)
    {
        // page fault : a page must be paged out
        if (!in_arr(frames, num_frames, page_ref_str[i]))
        {
            printf("Fault: %i\t", page_ref_str[i]);

            // replace oldest frame
            frames[last_paged] = page_ref_str[i];

            // adjust index of next page to swap out
            last_paged = (last_paged + 1) % num_frames;

            // increment number of page faults
            num_faults++;

            // display new frame table
            printf("New frame table: ");
            print_frame_table(frames, num_frames);
        }
    }
    return num_faults;
}

// uses LRU strategy for replacing frames on page faults
int LRU(int* page_ref_str, int num_frames)
{
    // create frame table
    int frames[num_frames];
    init_frame_table(frames, num_frames);

    int num_faults = 0;

    // step over the reference string
    for (int i = 0; i < PAGE_REF_STRING_LENGTH; i++)
    {
        // page fault : a page must be paged out
        if (!in_arr(frames, num_frames, page_ref_str[i]))
        {
            printf("Fault: %i\t", page_ref_str[i]);

            // repalce last used frame
            int least_recent_pos = find_least_recent_pos(page_ref_str, frames, num_frames, i);
            frames[least_recent_pos] = page_ref_str[i];

            // increment number of page faults
            num_faults++;

            // display new frame table
            printf("New frame table: ");
            print_frame_table(frames, num_frames);
        }
    }
    return num_faults;
}

// uses optimum page replacement strategy for replacing frames on page faults
int OPT_MIN(int* page_ref_str, int num_frames)
{
    // create frame table
    int frames[num_frames];
    init_frame_table(frames, num_frames);

    int num_faults = 0;

    // step over the reference string
    for (int i = 0; i < PAGE_REF_STRING_LENGTH; i++)
    {
        // page fault : a page must be paged out
        if (!in_arr(frames, num_frames, page_ref_str[i]))
        {
            printf("Fault: %i\t", page_ref_str[i]);

            // repalce frame that is furthest from being reused
            int most_distant_pos = find_most_distant_pos(page_ref_str, frames, num_frames, i);
            frames[most_distant_pos] = page_ref_str[i];

            // increment number of page faults
            num_faults++;

            // display new frame table
            printf("New frame table: ");
            print_frame_table(frames, num_frames);
        }
    }
    return num_faults;
}

// returns the index of the least recently used page frame in frames
int find_least_recent_pos(int* page_ref_str, int* frames, int num_frames, int pos)
{
    int distances[num_frames];
    for (int i = 0; i < num_frames; i++)
    {
        // if a frame is uninitialized its distance is weighted heavily
        if (frames[i] == -1)
        {
            distances[i] = pos + 1;
        }
        // otherwise, distance from pos is detertmined
        else
        {
            for (int j = pos; j >= 0; j--)
            {
                if (page_ref_str[j] == frames[i])
                {
                    distances[i] = pos - j;
                    break;
                }
            }
        }
    }
    return find_max_pos(distances, num_frames);
}

// returns the index of the page frame furthest from being used again in frames
int find_most_distant_pos(int* page_ref_str, int* frames, int num_frames, int pos)
{
    int distances[num_frames];
    for (int i = 0; i < num_frames; i++)
    {
        // if a frame is uninitialized its distance is weighted heavily
        if (frames[i] == -1)
        {
            distances[i] = PAGE_REF_STRING_LENGTH + 1;
        }
        // otherwise, distance from pos is detertmined
        else
        {
            for (int j = pos; j < PAGE_REF_STRING_LENGTH; j++)
            {
                if (page_ref_str[j] == frames[i])
                {
                    distances[i] = j - pos;
                    break;
                }
            }
        }
    }
    return find_max_pos(distances, num_frames);
}

// returns the position of the max element in an array
int find_max_pos(int* arr, int size)
{
    int max_pos = 0;
    for (int i = 1; i < size; i++)
    {
        if (arr[i] > arr[max_pos])
            max_pos = i;
    }
    return max_pos;
}

// initializes the frame table as empty
void init_frame_table(int* table, int size)
{
    for (int i = 0; i < size; i++)
        table[i] = -1;
}

// prints a frame table
void print_frame_table(int* table, int size)
{
    printf("[");
    for (int i = 0; i < size; i++)
        printf("%i ", table[i]);
    printf("]\n");
}

// checks if a value is in an array
bool in_arr(int* arr, int size, int val)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == val)
            return true;
    }
    return false;
}

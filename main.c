/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 16, 2022
 *
 */

#include <stdio.h>
#include "cache_impl.h"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    char value_returned[37]; /* accessed data */
    // accessed data's size = 9(buff array's maximum size in type_check function) * 4(int 4 bytes) + 1(null value) = 37

    /* Invoke check_cache_data_hit() */
    int value = check_cache_data_hit(addr, data_type);
    // check_cache_data_hit function's return value is stored to variable 'value'.

    if (value != -1) {  //it means there was hit value in cache. 'value' is hit data value.
        num_cache_hits += 1;       //hit counts go up by 1.
        sprintf_s(value_returned, sizeof(value_returned), "%s", value);     //hit data value is stored to value_returned.
        return value_returned;      //return data
    }
    else if (value == -1)   //it means miss case happened.
    {
        num_cache_misses += 1;      //miss counts go up by 1.
        sprintf_s(value_returned, sizeof(value_returned), "%s", access_memory(addr, data_type));
        //invoke access_memory() to bring desired data value from memory, and store it value_returned.
        return value_returned;      //return data
    }
    
    return -1;      //if there is no data neither in cache or memory, return -1.
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();
    
    ifp = fopen("access_input3.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    print_cache_entries();

    fputs("[Accessed Data]\n", ofp);    //print out [Accessed Data] to access_output.txt.

    //At this iteration statement, it repeats task that reads input file line by line, and prints out data to output file.
    while (!feof(ifp)) {        //repeats until finish reading input file.      
        fscanf(ifp, "%d %c", &access_addr, &access_type);
        //read one line from input file, and store each value variable access_addr, access_type.
        if (feof(ifp))      //when it reaches end of the input file, escape iteration statement.
            break;    
        if ((access_type != 'h') && (access_type != 'w') && (access_type != 'b'))
            continue;
        //if access_type is neither b or h or w, go back to the start of iteration statement.
        accessed_data = retrieve_data(access_addr, access_type);    
        //invoke retrieve_data and store return value accessed_data.
        global_timestamp += 1;  //increase global_timestamp by 1.
        fprintf(ofp, "%d    %c     0x%s\n", access_addr, access_type, accessed_data);
        //print out byte address, access type, and accessed data to output file.
        printf("---\n");
        print_cache_entries();
    }
    double hit_ratio = ((double)num_cache_hits / ((double)num_cache_hits + (double)num_cache_misses));
    //calculating hit ratio, the number of hit cases devided by the number of hit cases + the number of miss cases.
    double bandwidth = ((double)num_bytes / (double)num_access_cycles);
    //calculating bandwith, the number of bytes divided by the number of access cycles.

    fputs("--------------------------------\n", ofp);
    fputs("[Direct mapped cache performance]\n", ofp);  //print out [Direct mapped cache performance] to access_output.txt.
    fprintf(ofp, "Hit ratio = %3.2f (%d/%d)\n", hit_ratio, num_cache_hits, num_cache_hits + num_cache_misses);
    //print out hit ratio to access_output.txt.
    fprintf(ofp, "Bandwith = %3.2f (%d/%d)\n", bandwidth, num_bytes, num_access_cycles);
    //print out bandwith to access_output.txt.

    fclose(ifp);
    fclose(ofp);




    return 0;
}

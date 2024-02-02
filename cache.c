/*
 * cache.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 16, 2022
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16)
            i = 0;
        if (++j >= 16)
            j = 0;
        
        if (i == 0 && j == i+gap)
            j = i + (++gap);
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;
            pEntry->tag = -1;
            pEntry->timestamp = 0;
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        printf("[Set %d] ", i);
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("%#x(%d) ", pEntry->data[k], k);
            }
            printf("\t");
        }
        printf("\n");
    }
}

int check_cache_data_hit(void *addr, char type) {

    /* Fill out here */
    num_access_cycles += CACHE_ACCESS_CYCLE;

    int x = (int)addr;
    int c_tag = (x /DEFAULT_CACHE_BLOCK_SIZE_BYTE) / (4 / DEFAULT_CACHE_ASSOC);
    int bl_offset = x % DEFAULT_CACHE_BLOCK_SIZE_BYTE;

    for (int i = 0; i < CACHE_SET_SIZE; i++) {
        for (int j = 0; j<DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            if (pEntry->tag == c_tag)                           
            {//Check if any of the tags in the existing cache set match the tag value calculated through the byte address.
                int buff = type_check(type, bl_offset, pEntry);
                //if result is 'hit', Run the type_check function to return the hit data value within the cache set
                pEntry->timestamp = global_timestamp;
                //Modify cache's timestamp. This is because we have to express the time using the data.
                return buff;
            }
            else
                continue;                                           
            //Carry out the iteration statement until a matching tag value is found.
        }
    }
    return -1;                                          
    //If there is not matching tag, return -1. (express 'miss' state.)
    /* Return the data */  
}

int find_entry_index_in_set(int cache_index) {
    int entry_index=-1;
    //if there is no entry index, we have to know that. So we should to express the situation that there is no express by value that entry_index is -1.

    /* Check if there exists any empty cache space by checking 'valid' */
    if (DEFAULT_CACHE_ASSOC == 1)
        entry_index = 0;
    //cache set size is 1, we have 1 entry per 1 cache set.

    else {
        for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
            cache_entry_t* pEntry = &cache_array[cache_index][i];
            if (pEntry->valid == 0) {
                entry_index = i;
                // The situation that valid value is zero, means there is entry_part in cache set.
            }       
        }

        /* Otherwise, search over all entries to find the least recently used entry by checking 'timestamp' */
        if (entry_index == -1) {//if there is no entry index. so we have to overwrite the cache.
            int t = cache_array[cache_index][0].timestamp;      //initialize variable t with first timestamp.
            //We are going to use the index which we used least recently. So we'll find the smaller timestamp.
            //the iteration statement which finds min value.
            //The min value t is modified by comparing with next timestamp.
            for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
                cache_entry_t* pEntry = &cache_array[cache_index][i];
                if (t >= pEntry->timestamp) {
                    t = pEntry->timestamp;
                    entry_index = i;
                    //modify the entry_index to which has the smallest timestamp.
                }
            }
        }
    }

    return entry_index; 
}

int access_memory(void *addr, char type) {
    
    /* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */
    /* You need to invoke find_entry_index_in_set() for copying to the cache */
    int x = (int)addr;
    //x is byte adress.
    int c_index = (x / DEFAULT_CACHE_BLOCK_SIZE_BYTE ) % (4 / DEFAULT_CACHE_ASSOC);
    //Calculating the cache's index. This is the remainder of the block address divided by cache_set_size.
    int c_tag = (x / DEFAULT_CACHE_BLOCK_SIZE_BYTE) / (4 / DEFAULT_CACHE_ASSOC);
    //calculating th cache's tag. The block address is divided by cache_set_size.
    int entry_index=find_entry_index_in_set(c_index);
    //Caculating the cache's entry index by the function (find_entry_index_in_set function.)

    num_access_cycles += MEMORY_ACCESS_CYCLE;

    int w_index = (x / DEFAULT_CACHE_BLOCK_SIZE_BYTE ) * DEFAULT_CACHE_BLOCK_SIZE_BYTE / WORD_SIZE_BYTE;  
    //Calculate word address
    int bl_offset = x % DEFAULT_CACHE_BLOCK_SIZE_BYTE;  
    //Calculate block offset. (Block offset means the location of desired data.)

    /* Fetch the data from the main memory and copy them to the cache */
    cache_entry_t* pEntry = &cache_array[c_index][entry_index];     
    //fetch one block in memory and copy to cache.

    char* p = (char*)&memory_array[w_index];
    //receive the word in memory (follow the word index.)
    char* q = (char*)&memory_array[w_index + 1];
    //we have to store 2 words in one cache block. so recieve the data in memory one more time (the data is in word index+1.)
    for (int i = 0; i < 4; i++) {
        pEntry->data[i] = *(p + i);
        //we have to store the cache's data to byte unit from 0th to 3rd.
    }
    for (int i = 0; i < 4; i++) {
        pEntry->data[i + 4] = *(q + i);
        //we have to store the cache's data to byte unit from 4th to 7th.
    }

    pEntry->valid = 1;
    //Modify the vaild value to 1 to express the fact that the data exists.
    pEntry->tag = c_tag;
    //Modify the tag value to c_tag to express where the data is.

    /* Return the accessed data with a suitable type */   
    int buff = type_check(type, bl_offset, pEntry);             
    //This is code that returns the desired data value. Use the type_check function to combine the data according to the desired data type and return it.

    pEntry->timestamp = global_timestamp;                      
    //Update timestamp in cache set from which data is copied

    return buff;                                      
    //return the data.
}

int type_check(char type, int bl_offset, cache_entry_t* pEntry)
{
    char buff[9]; //define data to receive
    //buff array's size calculating : Change each hex value to up to 2 chars. So, 4 hex value can be changed to up to 8 chars + 1 null value = 9

    if (type == 'b') {
        //If the type is 'b', the data's byte is 1.
        num_bytes += 1;
        unsigned char key[1] = { pEntry->data[bl_offset] };
        // receive the data in the cache to char array. Because the data in pEntry should be stored in a reverse order.
        unsigned char len = 1;

        for (int i = 0; i < len; i++) {
            sprintf_s(buff + i * 2, 3, "%02x", key[i]);
            // combine datas and store in buff.
        }
    }
    else if (type == 'h') {
        //if the type is 'h', the data's byte is 2.
        num_bytes += 2;
        unsigned char key[2] = { pEntry->data[bl_offset + 1], pEntry->data[bl_offset] };
        // receive the data in the cache to char array. Because the data in pEntry should be stored in a reverse order.
        unsigned char len = 2;

        for (int i = 0; i < len; i++) {
            sprintf_s(buff + i * 2, 3, "%02x", key[i]);
            // combine datas and store in buff.
        }
    }
    else if (type == 'w') {
        //if the type is 'w', the data's byte is 4.
        num_bytes += 4;
        unsigned char key[4] = {
            pEntry->data[bl_offset + 3], pEntry->data[bl_offset + 2],pEntry->data[bl_offset + 1], pEntry->data[bl_offset] };
        // receive the data in the cache to char array. Because the data in pEntry should be stored in a reverse order.
        unsigned char len = 4;

        for (int i = 0; i < len; i++) {
            sprintf_s(buff + i * 2, 3, "%02x", key[i]);
            // combine datas and store in buff.
        }
    }
    return buff;
    //we need to send the data. so return value is buff
}

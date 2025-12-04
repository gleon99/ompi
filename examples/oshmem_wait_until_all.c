/*
 * Copyright (c) 2024      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * This example demonstrates the use of shmem_wait_until_all
 * to implement a simple barrier synchronization.
 *
 * Based on Example 36 from the OpenSHMEM 1.5 specification.
 */

#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int mype, npes;
    int *flags;
    int *status = NULL;
    int i;

    shmem_init();
    
    mype = shmem_my_pe();
    npes = shmem_n_pes();

    /* Allocate symmetric memory for flags array */
    flags = shmem_calloc(npes, sizeof(int));
    if (flags == NULL) {
        fprintf(stderr, "PE %d: Failed to allocate symmetric memory\n", mype);
        shmem_finalize();
        return 1;
    }

    printf("PE %d of %d: Starting barrier using shmem_wait_until_all\n", 
           mype, npes);

    /* Each PE signals all other PEs that it has reached the barrier */
    for (i = 0; i < npes; i++) {
        shmem_int_atomic_set(&flags[mype], 1, i);
    }

    printf("PE %d: Waiting for all PEs to reach the barrier...\n", mype);

    /* Wait for all PEs to signal readiness */
    shmem_int_wait_until_all(flags, npes, status, SHMEM_CMP_EQ, 1);

    printf("PE %d: All PEs have reached the barrier!\n", mype);

    /* Verify all flags are correctly set */
    for (i = 0; i < npes; i++) {
        if (flags[i] != 1) {
            fprintf(stderr, "PE %d: ERROR - flag[%d] = %d (expected 1)\n",
                    mype, i, flags[i]);
            shmem_free(flags);
            shmem_finalize();
            return 1;
        }
    }

    /* Clean barrier to synchronize before finalization */
    shmem_barrier_all();

    if (mype == 0) {
        printf("\nSUCCESS: All PEs completed the barrier synchronization!\n");
    }

    /* Clean up */
    shmem_free(flags);
    shmem_finalize();

    return 0;
}



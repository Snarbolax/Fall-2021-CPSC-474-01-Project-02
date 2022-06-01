#include <stdio.h>
#include <stdlib.h> // used for srand() and rand() in int status
// #include <math.h> // used for log functions in initializing double max_round
#include <mpi.h>

int main( int argc, char *argv[] )
{
    int rank, size; // rank is used as the unique process ID to be compared
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int status = 1; // 0 = passive, 1 = active. Don't want to use stdbool.h :)))
    int round = 1; // ultimately not necessary for matching round parity, since MPI_Barrier is used.
    // double max_round = (log(size) / log(2)) + 1; // the max number of rounds allowed to run. Only functional in non-blocking communication
    int number_left = rank; // the number received from the process on the left
    int number_right = rank; // the number received from the process on the right
    int max = 0; // used to hold the max value between the received numbers from the left and right
    int local_leader = -1;
    int global_leader = -1; // acts as a global variable

    printf("\nI am %d of %d\n", rank, size );
    srand(time(NULL) + rank); // seed RNG, based on current time when the function is called and the current processor's rank
    if (rank != 0) // at least process 0 stays as an initiator to avoid a case where all processes are passive and no leader can be elected
        status = rand() % 2;
    if (status == 1)
        printf("! Process %d is an INITIATOR. !\n\n", rank);

    MPI_Barrier(MPI_COMM_WORLD);

    // using blocking communication + barriers + prioritizing receives before sends negates the need for keeping track of round parity
    while(global_leader == -1){
        MPI_Barrier(MPI_COMM_WORLD);
        // Tag 12 = (L)eft, Tag 18 = (R)ight. Corresponding to the position of the first letter of each word in the English alphabet.
        if (status == 0){ // is passive
            if (rank == 0) // special case for rank 0, since (rank - 1) % size will give us a runtime-error
                MPI_Recv(&number_left, 1, MPI_INT, size - 1, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else
                MPI_Recv(&number_left, 1, MPI_INT, (rank - 1) % size, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive from left side
            MPI_Send(&number_left, 1, MPI_INT, (rank + 1) % size, 12, MPI_COMM_WORLD); // send received to right side

            MPI_Recv(&number_right, 1, MPI_INT, (rank + 1) % size, 18, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive from right side
            if (rank == 0)
                MPI_Send(&number_right, 1, MPI_INT, size - 1, 18, MPI_COMM_WORLD);
            else
                MPI_Send(&number_right, 1, MPI_INT, (rank - 1) % size, 18, MPI_COMM_WORLD); // send received to left side
        }
        else if (status == 1){ // is active
            if (rank == 0)
                MPI_Send(&rank, 1, MPI_INT, size - 1, 18, MPI_COMM_WORLD);
            else
                MPI_Send(&rank, 1, MPI_INT, (rank - 1) % size, 18, MPI_COMM_WORLD); // send own ID to left side
            MPI_Send(&rank, 1, MPI_INT, (rank + 1) % size, 12, MPI_COMM_WORLD); // send own ID to right side

            if (rank == 0)
                MPI_Recv(&number_left, 1, MPI_INT, size - 1, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else
                MPI_Recv(&number_left, 1, MPI_INT, (rank - 1) % size, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&number_right, 1, MPI_INT, (rank + 1) % size, 18, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if (number_left > number_right || number_left == number_right)
                max = number_left;
            else if (number_left < number_right)
                max = number_right;
            printf("! Active neighbors of process %d are %d and %d !\n", rank, number_left, number_right);

            if (max < rank){ // If p’s ID is the largest of the three IDs, then p proceeds to the next election round.
                printf("! Own ID %d is LARGER THAN max ID %d between active neighbors. !\n", rank, max); // Debugging
                round++;
            }
            else if (max > rank){ // If one of the other IDs is larger than p’s ID, then p becomes passive.
                printf("! Own ID %d is LESS THAN max ID %d between active neighbors. !\n", rank, max); // Debugging
                status = 0;
                printf("! Process %d becomes PASSIVE with status %d !\n", rank, status);
            }
            else if (max == rank){ // no other active processes left in the ring.
                round++;
                local_leader = rank;
            }
            printf("! Process %d reached round %d with status %d !\n\n", rank, round, status); // Debugging
        }
        MPI_Allreduce(&local_leader, &global_leader, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == global_leader){
        printf("\n! Process %d received its own ID from either side. ! \n! Process %d is the LEADER. !\n\n", rank, global_leader);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
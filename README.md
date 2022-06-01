# Fall 2021 CPSC 474-01 Project 02 - Programming using MPI

* Group Member(s)
	* __Kenneth Doan__
		* [snarbolax@csu.fullerton.edu](snarbolax@csu.fullerton.edu)

* Language(s)
	* C

* Professor
	* Prof. Doina Bein, CSU Fullerton

### Project Description
> Write a distributed program in MPI that simulates one of the leader election algorithms learnt in class for a ring topology using a fixed number of processors. In this case, I am using Franklin's algorithm as the basis of my program.
</br></br>
For this project you need to choose a topic covered in class or similar to the topics listed here and design an algorithm that uses MPI commands to solve a problem. You will describe the algorithm using clear pseudocode and implement your algorithms in C/C++/Java, compile, test it, and submit BOTH the report (as a PDF file) and the programs.

### How to Compile the Code
1. Navigate to the directory that contains __franklin_ring.c__ with your OS's terminal / command-line.
2. Type the following into the terminal / command-line and press enter:
	* > mpicc __franklin_ring.c__ -o franklin_ring

### How to Run the Code
1. Navigate to the directory that contains __franklin_ring.exe__ with your OS's terminal / command-line.
2. Type the following into the terminal / command-line and press enter:
	* > mpirun -n [INSERT NUMBER OF PROCESSES HERE] franklin_ring
		* The specified number of processes must be, at least, 2.
		* You can create an output file to view by including the name of the output file--extension included--and an ">" after the name of the compiled program.
			* the output file will be in the same directory of the program
			* e.g. "mpirun -n [INSERT NUMBER OF PROCESSES HERE] franklin_ring > insert_user_custom_output_file_here.txt", without the quotation marks
			* the output of the program will not show up in your OS's terminal / command-line if you redirect its output into an output file

### Miscellaneous Info
* This program was compiled and tested on a Cygwin64 environment, using the Cygwin-based OpenMPI libraries.

### Known Issues and Bugs
1. Sometimes, lines from the output of the program may get cut off and continue printing later in the program's runtime execution.
	* The biggest offender of this bug is line 79 in __franklin_ring.c__ *printf("! Process %d reached round %d with status %d !\n", rank, round, status);*
		* The cutoff is usually between *Process %d* and *reached round*.
2. In the program's current implementation, process 0 is an initiator by default to avoid cases where all of the processes are non-initiators and no leader can be elected.

### Possible Improvements
1. Using shared-memory variables to enable semaphore implementation may prevent any line cutoffs from occurring.
2. Use shared-memory variables to check whether there is at least 1 initiator present in a cluster.
	* This will allow us to possibly randomly assign a non-initiator to be an initiator as a safe case, instead of always having process 0 be an initiator by default.

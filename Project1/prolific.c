#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	FILE *fptr = fopen("seed.txt", "r");
	char input_seed[10];
	fgets(input_seed, 10, fptr);
	int seed = atoi(input_seed);
	srand(seed); 
	
	// creating a integer of a random number of children between 8 and 13
	int rand_children = ((rand() % 13) + 1);

	//creating array to store a random number for each child
	int array_random_numbers[rand_children]; 
	// looping through #of children
	for(int i = 0; i < rand_children; i++){
		// generate random number for each child
		array_random_numbers[i] = rand();
	}

	// Print seed
	printf("Read seed value (converted to integer): %d\n", seed);
	// print how many children there will be
	printf("Random Child Count: %d\n", rand_children);
	// print default message
	printf("I'm feeling prolific!\n");

	// loop for each child
	for(int i = 0; i < rand_children; i++){

		// fork
		int pid = fork();

		int personal_pid = getpid();

		// get random number from array 
		int child_random_number = array_random_numbers[i];
		// create random number for exit code
		int exit_code = ((child_random_number % 50) + 1);
		// create random wait time (between 1-3 seconds)
		int wait_time = ((child_random_number % 3)  + 1);

		// non-valid process
		if(pid < 0){
			printf("fork failed\n");
			return -1;
		}
		// child process
		else if (pid == 0){
			printf("\t[Child, PID: %d] I am the child and I will wait %d seconds and exit with code %d\n", personal_pid, wait_time, exit_code);
			sleep(wait_time);
			printf("\t[Child, PID: %d] Now exiting...\n",personal_pid);
			exit(exit_code);
		}
		// parent process
		else {
			printf("[Parent]: I am waiting for PID %d to finish.\n", pid);
			int child_result = waitpid(pid, NULL, 0);
			printf("[Parent]: Child %d finished with status code %d. Onward!\n", child_result, exit_code);
		}
	}
}

	
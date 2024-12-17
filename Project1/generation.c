#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int generate_child(int descendants){

    int pid_value = fork();

    if (pid_value == 0){
        printf("\t[Child, PID: %d] I was called with descendant count=%d. I'll have %d descendant(s).\n", getpid(), descendants, descendants - 1);
        if (descendants > 1){
            generate_child(descendants - 1);
        } 
        exit(0);
    }    
    // non-valid process
    else if(pid_value < 0){
        printf("fork failed\n");
        exit(1);
    }
    else {
        // parent process
        int waitstatus;
        printf("[Parent, PID: %d]: I am waiting for PID %d to finish.\n", getpid(),pid_value);
        waitpid(pid_value, &waitstatus, 0);

        if (WIFEXITED(waitstatus)){
            printf("[Parent, PID: %d] Child %d finished with status code %d. It's now my turn to exit.\n", getpid(), pid_value, descendants);
        }
        exit(WEXITSTATUS(waitstatus) + 1);
    }
}

int main(){

    FILE *fptr = fopen("seed.txt", "r");
	char input_seed[10];
	fgets(input_seed, 10, fptr);
	int seed = atoi(input_seed);
	srand(seed); 

    // creating a integer of a random number of children between 5 and 12
    int lifespan_count = (rand() % 7) + 5;

    // Print seed
	printf("Read seed value (converted to integer): %d\n", seed);
	// print how many children there will be
	printf("Random Descendant Count: %d\n", lifespan_count);
	// print default message
	printf("Time to meet the kids/grandkids/great grand kids/...\n");

    generate_child(lifespan_count);
}
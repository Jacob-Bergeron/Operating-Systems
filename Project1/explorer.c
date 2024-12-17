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
	//random number between 1 - 6
	int generate_random(){
		int my_random = (rand() % 6);
		return my_random;
	}
	// array of directories
	char* array_directories[] = {"/home","/proc","/proc/sys","/usr","/usr/bin","/bin"};

    // Print seed
	printf("Read seed value (converted to integer): %d\n", seed);
    // Print default message
    printf("It's time to see the world/file system!\n");

	char* cmd[] = {"ls", "-tr", (char *)0 };

	for(int i = 0; i < 6; i++){
		// change current working dir to the option selected by random number
		int my_random = generate_random();
		chdir(array_directories[my_random]);
		// print out the option that was selected
		printf("Selection #%d: %s [SUCCESS]\n", i , array_directories[my_random]);
		// output the value of the current directory 
		char buffer[15];
		getcwd(buffer,15);
		// then forks a child 
		int pid = fork();
		if(pid < 0 ){ //invalid process
			printf("fork failed\n");
			return -1;
		} else if (pid == 0){ // child process
			printf("\t[Child, PID: %d]: Executing 'ls -tr' command...\n", getpid());
			// execute ls -tr command ???
			execvp("ls", cmd);
			exit(0);
		} else {  //parent process
			printf("[Parent]: I am waiting for PID %d to finish\n", pid);
			waitpid(pid, NULL, 0);
			printf("[Parent]: Child %d finished with staus code 0. Onward!\n", pid);
		}
	}
} 
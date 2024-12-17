#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

    //create array for the 4 possible input files
    char* files_array[] = {"seed_slug_1.txt","seed_slug_2.txt","seed_slug_3.txt","seed_slug_4.txt"};
    // take the command line input number (1, 2, 3 or 4) and store in variable
    char* input_number = argv[1];
    // convert the string into a number and index into the files_array
    char* input_file = files_array[atoi(input_number)];
    // open that desire file
    FILE *fptr = fopen(input_file,"r");
    char input_seed[10];
	fgets(input_seed, 10, fptr);
	int seed = atoi(input_seed);
	srand(seed);

    // random number between 2 and 6
    int random_seconds = ((rand() % 4) + 2);

    // random, 0 or 1
    int coin_flip = rand() % 2;

    // Default messages
    printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);

    printf("[Slug PID: %d] Delay time is %d seconds. Coin flip: %d\n", getpid(), random_seconds, coin_flip);

    printf("[Slug PID: %d] I'll get the job done. Eventually...\n", getpid());

    // wait 2 - 6 seconds
    sleep(random_seconds);

    // list of 2 possible commands
    char* cmd1[] = {"last", "-i", "-x", NULL};
    char* cmd2[] = {"id", "--group", NULL};
    

    char* command_string;
    if(coin_flip == 1){
        command_string = "-i -x";
        printf("[Slug PID: %d] Break time is over! I am running the '%s' command.\n", getpid(), command_string);
        //execvp("last", cmd1);
    } else {
        command_string = "--group";
        printf("[Slug PID: %d] Break time is over! I am running the '%s' command.\n", getpid(), command_string);
        //execvp("id", cmd2);
        }    
}
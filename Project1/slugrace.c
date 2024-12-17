#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(){

    // This is ugly but after trying to figure it our for so long, 
    // couldn't figure out a better way
    char* slug_commands1[] = {"./slug","1", NULL};
    char* slug_commands2[] = {"./slug","2", NULL};
    char* slug_commands3[] = {"./slug","3", NULL};
    char* slug_commands4[] = {"./slug","4", NULL};

    for(int i = 0; i < 4; i++){

        float total_sleep;
        int pid_value = fork();

        //invalid process
        if(pid_value < 0){
            printf("invalid process\n");
            return -1;
        }
        // child process
        else if(pid_value == 0){
            if(i == 0){
                printf("\t[Child, PID: %d] Executing './slug 1' command...\n", getpid());
                execvp("./slug", slug_commands1); 
            } else if(i == 1){
                printf("\t[Child, PID: %d] Executing './slug 2' command...\n", getpid());
                execvp("./slug", slug_commands2); 
            } else if(i == 2){
                printf("\t[Child, PID: %d] Executing './slug 3' command...\n", getpid());
                execvp("./slug", slug_commands3); 
            } else if(i == 3){
                printf("\t[Child, PID: %d] Executing './slug 4' command...\n", getpid());
                execvp("./slug", slug_commands4); 
            }
            exit(0);
        }
        //parent process
        else {
            // print default message
            printf("[Parent]: I forked off child %d\n", pid_value);
            // create array to store the slugs
            int slug_array[4];
            // add slug to array 
            slug_array[i] = pid_value;

            int wait_status;
            waitpid(-1, &wait_status, WNOHANG);
            //parent enters a loop, checking if any child has finished
            if(WIFEXITED(wait_status)){
                int status_code = WEXITSTATUS(wait_status);
                while(status_code != 0){
                    // wait 0.33 seconds
                    usleep(330000);
                    total_sleep += 0.33;
                    // print the current racing slugs
                    printf("The race is ongoing. The following children are still racing:\n");
                    /*
                    for(int j = 0; j < 4; j++){
                        printf(" %d ", slug_array[j]);
                    }
                    */
                    if(wait_status == pid_value){
                        printf("Child %d has finished.\n", pid_value);
                    }
                }
            }
            // while(child_result == 0){
            //     // wait 0.33 seconds
            //     usleep(330000);
            //     total_sleep += 0.33;
            //     // print the current racing slugs
            //     printf("The race is ongoing. The following children are still racing:\n");
            //     /*
            //     for(int j = 0; j < 4; j++){
            //         printf(" %d ", slug_array[j]);
            //     }
            //     */
            //    if(child_result == pid_value){
            //         printf("Child %d has finished.\n", pid_value);
            //    }
            // }
            // code for when a child finishes
            //printf("Child %d has crossed the finish line! It took %f seconds\n", getpid(),total_sleep);
        }
    }
}
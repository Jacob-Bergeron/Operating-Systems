#include "stadium.h"

/*
MUTEXES
*/
pthread_mutex_t playerLock = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t mutexField = PTHREAD_MUTEX_INITIALIZER;


// locks for each of the ready counters
pthread_mutex_t lockReadyBaseball = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t lockReadyFootball = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t lockReadyRugby = PTHREAD_MUTEX_INITIALIZER;

// A bool that threads will check to see if they should try to join the field right now
bool tryToJoinField = false;
pthread_mutex_t lockTryToJoinField = PTHREAD_MUTEX_INITIALIZER;

/*
CONDITIONS
*/
pthread_cond_t condField = PTHREAD_COND_INITIALIZER;

// Creating global variable for the playing field
// Initialized to fieldFree and 0 players on field.
struct field playingField = {.fieldState = fieldFree, .totalPlayersField = 0};


// Global Variables

//The number of players in each sport which are ready to join the field and play
int readyBaseballPlayers = 0;
int readyFootballPlayers = 0;
int readyRugbyPlayers = 0;


// helper functions

/*
joinTeam()
Parameters: struct playerThread player
What function does: 
 - Takes a playerThread struct 
 - Checks what sport is stored in the struct
 - Adds the player to a count of ready players
Returns: void 
*/
void joinTeam(struct playerThread* player){

    // if baseball player
    if (strcmp(player->sport, "baseball") == 0){
        pthread_mutex_lock(&lockReadyBaseball);
        if (readyBaseballPlayers < BASEBALL_PLAYERS_PER_TEAM * 2){
            readyBaseballPlayers++;
            if (readyBaseballPlayers == BASEBALL_PLAYERS_PER_TEAM*2){
                printf("[Baseball ID: %d] TEAM READY\n", player->id);
            }
        } 
        pthread_mutex_unlock(&lockReadyBaseball);
    } 
    if(strcmp(player->sport, "football") == 0){
        //lock football
        pthread_mutex_lock(&lockReadyFootball);
        if(readyFootballPlayers < FOOTBALL_PLAYERS_PER_TEAM * 2){
            readyFootballPlayers++;
            if(readyFootballPlayers == FOOTBALL_PLAYERS_PER_TEAM *2){
                printf("[Football ID: %d] TEAM READY\n", player->id);
            }
        } 
        pthread_mutex_unlock(&lockReadyFootball);
    }
    if(strcmp(player->sport, "rugby") == 0){
        pthread_mutex_lock(&lockReadyRugby);
        if(readyRugbyPlayers < RUGBY_PLAYERS_PER_PAIR * 15){
            readyRugbyPlayers++;
            if ((readyRugbyPlayers % 2) == 0){
                printf("[Rugby ID: %d] Pair Ready\n", player->id);
            }
        }
        pthread_mutex_unlock(&lockReadyRugby);
    }   
}


/*
bool isTeamsTurn()
What it does:
- Checks which teams turn it should be
- Circular progression through teams
*/
bool isTeamsTurn(struct playerThread player){
   //Baseball -> Football -> Rugby -> Baseball
   if(playingField.lastGamePlayed == fieldFree){return true;}
   if(strcmp(player.sport, "baseball") == 0 && playingField.lastGamePlayed == fieldRugby){return true;}
   if(strcmp(player.sport, "football") == 0 && playingField.lastGamePlayed == fieldBaseball){return true;}
   if(strcmp(player.sport, "rugby") == 0 && playingField.lastGamePlayed == fieldFootball){return true;}
   return false;
}

/*
bool teamHasEnoughPlayers
What is does:
- checks if the player that is passed has enough players on its current team
*/
bool teamHasEnoughPlayers(struct playerThread player){
   if(strcmp(player.sport, "baseball") == 0 && readyBaseballPlayers >= BASEBALL_PLAYERS_PER_TEAM*2){return true;}
   if(strcmp(player.sport, "football") == 0 && readyFootballPlayers >= FOOTBALL_PLAYERS_PER_TEAM*2){return true;}
   if(strcmp(player.sport, "rugby") == 0 && readyRugbyPlayers >= RUGBY_PLAYERS_PER_PAIR*2){return true;}
   return false;
}

/*
Function to convert sport name to a field state
*/
enum state sportNameToFieldState(struct playerThread player){
   if(strcmp(player.sport, "baseball") == 0){
       return fieldBaseball;
   } else if(strcmp(player.sport, "football") == 0){
       return fieldFootball;
   } else{
       return fieldRugby;
   }
}

/*
void joinField
What it does:
- Locks field object
- 

*/
void joinField(struct playerThread* player){

   // check if field exceeds total players
   assert(playingField.totalPlayersField <= TOTAL_PLAYERS);


   if(playingField.totalPlayersField == 0){ //if players==0, then this is the first thread who is entering the field
    printf("setting field to %d\t",sportNameToFieldState(*player));
      playingField.fieldState = sportNameToFieldState(*player); //this player sets the field to it's own sport
      //creates a random num between 10 and 4
        int playingTime = (rand() % (10-4+1)) + 4;
        playingField.timeGameEnds = time(NULL) + playingTime; // takes current time and adds the random number of seconds.
        printf("[%s: %d] <<Game Started>>\n", player->sport,player->id);
   }


   if(strcmp(player->sport, "baseball") == 0)
   {
        player->whenImLeavingField = playingField.timeGameEnds;
       if(playingField.totalPlayersField <= BASEBALL_PLAYERS_PER_TEAM*2){
            playingField.totalPlayersField++;
            player->currentlyPlaying = true;
            printf("[Baseball: %d] Playing at position %d\n", player->id, playingField.totalPlayersField);
       }
       if(playingField.totalPlayersField == BASEBALL_PLAYERS_PER_TEAM*2){
           pthread_mutex_lock(&lockReadyBaseball);
           readyBaseballPlayers -= BASEBALL_PLAYERS_PER_TEAM*2; //subtracting them all at once at the end so that the number of ready baseball players doesn't go below the needed threshold before all have joined the field
           pthread_mutex_unlock(&lockReadyBaseball);
           pthread_mutex_lock(&lockTryToJoinField);
           tryToJoinField = false;
           pthread_mutex_unlock(&lockTryToJoinField);
       }
   } else if(strcmp(player->sport, "football") == 0){
        player->whenImLeavingField = playingField.timeGameEnds;
       if(playingField.totalPlayersField <= FOOTBALL_PLAYERS_PER_TEAM*2){
           playingField.totalPlayersField++;
           player->currentlyPlaying = true;
            printf("[Football: %d] Playing at position %d\n", player->id, playingField.totalPlayersField);
       }
       if(playingField.totalPlayersField == FOOTBALL_PLAYERS_PER_TEAM*2){
           pthread_mutex_lock(&lockReadyFootball);
           readyFootballPlayers -= FOOTBALL_PLAYERS_PER_TEAM*2; //subtracting them all at once at the end so that the number of ready baseball players doesn't go below the needed threshold before all have joined the field
           pthread_mutex_unlock(&lockReadyFootball);
           pthread_mutex_lock(&lockTryToJoinField);
           tryToJoinField = false;
           pthread_mutex_unlock(&lockTryToJoinField);
       }
   }else{
        int playingTime = (rand() % (10-4+1)) + 4;
        player->whenImLeavingField = time(NULL)+playingTime;
        if(playingField.timeGameEnds < player->whenImLeavingField){
            player->whenImLeavingField = playingField.timeGameEnds;
        }

       if(playingField.totalPlayersField <= RUGBY_PLAYERS_PER_PAIR*15){
           playingField.totalPlayersField++;
           player->currentlyPlaying = true;
            printf("[Rugby: %d] Playing at position %d\n", player->id, playingField.totalPlayersField);
       }
       if(playingField.totalPlayersField%2 == 0){ //Since rugby players join in pairs, we take them out of the ready list in pairs every even number
           pthread_mutex_lock(&lockReadyRugby);
           readyRugbyPlayers -= 2; //subtracting them all at once at the end so that the number of ready baseball players doesn't go below the needed threshold before all have joined the field
           pthread_mutex_unlock(&lockReadyRugby);
           if(playingField.totalPlayersField==RUGBY_PLAYERS_PER_PAIR*15){ //We only want to tell players to stop joining if the field is full
               pthread_mutex_lock(&lockTryToJoinField);
               tryToJoinField = false;
               pthread_mutex_unlock(&lockTryToJoinField);
           }
       }
   }
}

/*
void leaveField()
What it does:
- 
*/
void leaveField(struct playerThread* player){
    // pthread_mutex_lock(&mutexField);
    // playingField.totalPlayersField = 0;
    // printf("[%s: %d] <<Game Ended>>\n", player.sport, player.id);
    // pthread_mutex_unlock(&mutexField);
    // waitAndRejoin(player);

    while(1){
        if(time(NULL) >= player->whenImLeavingField){
            pthread_mutex_lock(&mutexField);
            printf("Curr: %ld, Leaving At %ld, Leaving Field\n",time(NULL),player->whenImLeavingField);
            playingField.totalPlayersField--;
            pthread_mutex_unlock(&mutexField);
            player->currentlyPlaying = false;
            if(playingField.totalPlayersField == 0)
            {
                playingField.fieldState = fieldFree;
                printf("[%s: %d] <<Game Ended>>\n", player->sport, player->id);
            }
            break;
            
        }
    }
}

/*
void waitRandomTime()
What it does:
- Waits a random time
Called after leave field to ensure that the thread waits 
*/
void waitRandomTime(){
    int waitingTime = (rand() % (10-4+1)) + 4;
    printf("wait\n");
    sleep(waitingTime);
}

/*
void *threadFunction()
Parameters: void *arg
 - void pointer
    - holds the playerThread struct
    - casted into a playerThread* struct 
Function Purpose:

*/
void *threadFunction(void *arg){
    // cast the threadFunction argument into a struct 
    struct playerThread* newPlayer = (struct playerThread*)arg;

// lock field
        // other threads wait on a cond (whaiting for field == 0 )
        // critical section
        // thread with lock calls join field
        // does whatever inside field
        // cond_broadcast (or cond_signal? ) to other threads who are waiting
        // unlock field

        // logic for checking if joinField should be run
        // if field != 0, don't join
        // if team's turn BUT not enough players, don't join
        // pthread_mutex_lock(&mutexField);

        // // while the field is NOT free, wait
        // while(playingField.fieldState != 0){
        //     // other threads wait here
        //     pthread_cond_wait(&condField, &mutexField);
        // }

        // joinField(*newPlayer);
        // leaveField(*newPlayer);

        // pthread_cond_broadcast(&condField);
        // pthread_mutex_unlock(&mutexField);
    while(1){ 
        joinTeam(newPlayer);

        while (1){
            pthread_mutex_lock(&mutexField);
            if(strcmp(newPlayer->sport, "rugby") == 0 && playingField.fieldState == fieldRugby){
            } else {
                // while the field is NOT free, wait
                while(playingField.fieldState != 0){
                // other threads wait here
                pthread_cond_wait(&condField, &mutexField);
                }
            }
            if(isTeamsTurn(*newPlayer) && !teamHasEnoughPlayers(*newPlayer)){ //forfeiting this sports turn if it's not ready
                //printf("[%s: %d], <<Game Started>>\n", player.sport,player.id);
                if(isTeamsTurn(*newPlayer)) { printf("This Team's Turn \t");} else { printf("not this team's turn \t");}
                if(teamHasEnoughPlayers(*newPlayer)) { printf("Has Enough Players \t");} else { printf("Hasn't Enough Players \t");}
                printf("got to field logic\n");
                playingField.lastGamePlayed = sportNameToFieldState(*newPlayer);
                playingField.fieldState=fieldFree;
            }else {
                pthread_mutex_unlock(&mutexField);
                printf("field state is: %d\t",playingField.fieldState);
                joinField(newPlayer);
                
                leaveField(newPlayer);
                waitRandomTime();
                break;
            }
            pthread_cond_broadcast(&condField);
            pthread_mutex_unlock(&mutexField);
            break;
        }
        
            // while(playingField.fieldState != 0 || (strcmp(newPlayer->sport, "rugby") == 0 && playingField.fieldState == fieldRugby)){
            // // if field != 0 AND if team turn BUT not enough players
            //     if(isTeamsTurn(*newPlayer) && !teamHasEnoughPlayers(*newPlayer)){
            //         //printf("[%s: %d], <<Game Started>>\n", player.sport,player.id);
            //         printf("got to field logic\n");
            //         playingField.lastGamePlayed = sportNameToFieldState(*newPlayer);
            //         playingField.fieldState=fieldFree;
            //     }
            //     else {
            //         joinField(*newPlayer);
            //         break;
            //     }
            // }
        
        
    }

    
    
}

/*
int main()
Parameters: void
What function does;
- reads a number from a file
- creates a seed for a random sequence

- creates # of threads for each sport
- waits until terminal is killed
*/
int main(void){
    FILE *fptr = fopen("seed.txt", "r");
	char input_seed[10];
	fgets(input_seed, 10, fptr);
	int seed = atoi(input_seed);
	srand(seed); 

    playingField.timeGameEnds = time(NULL);

	pthread_t threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        struct playerThread* playerData = malloc(sizeof(struct playerThread));
        if(i < 36){
            playerData->sport = strdup("baseball");
            playerData->id = i;
        } else if(i >= 36 && i < 80){
            playerData->sport = strdup("football");
            playerData->id = i;        
        }else {
            playerData->sport = strdup("rugby");
            playerData->id = i;
        }

        pthread_create(&threads[i], NULL, threadFunction, playerData);
    }

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }



}

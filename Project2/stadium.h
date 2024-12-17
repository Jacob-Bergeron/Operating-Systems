#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/*
DEFINES
*/
#define MAX_BASEBALL_PLAYERS 36
#define MAX_FOOTBALL_PLAYERS 44
#define MAX_RUGBY_PLAYERS 60

#define BASEBALL_PLAYERS_PER_TEAM 9
#define FOOTBALL_PLAYERS_PER_TEAM 11
#define RUGBY_PLAYERS_PER_PAIR 2

#define TOTAL_PLAYERS 140
#define NUM_THREADS 140

#define LENGTH_OF_GAME 3 // 3 seconds



/*
STRUCTS
*/
enum state {fieldFree, fieldBaseball, fieldFootball, fieldRugby};
// 'state' variable 
    // 0 = free 
    // 1 = baseball playing
    // 2 = football playing
    // 3 = rugby playing

// field is a struct
struct field {
   enum state fieldState;
   int totalPlayersField;
   enum state lastGamePlayed;
   time_t timeGameEnds;
};


/*
Struct for playerThread
 - Stores data for each thread that is created
Variables:
 - char* sport
    - Stores the sport that the thread is assigned
 - int id
    - stores an id integer that is unique to the thread
*/
struct playerThread{
    char* sport;
    int id; 
    bool currentlyPlaying; 
    time_t whenImLeavingField;
};


/*

HELPER FUNCTIONS

*/

void joinField(struct playerThread* player);

void leaveField(struct playerThread* player);

void waitAndRejoin(struct playerThread player);

/*


playerThread has
        char* sport; 
       int id;
       pthread_t pt;
       isReady


Field has
       enum state fieldState;
       int totalPlayersField;
       enum state lastSportPlayed
Helpers:


mutex tryToJoinField        --if this is true then players will try to join the field when ready


joinTeam()
   called on a thread 
   locks the readyPlayers count integer for it's sport and increments it,
   if(readyPlayers == playersNeededForThisSport){
       print(ID + Team Ready)
   }
   releases lock
   runs joinField


joinField()
   when the fieldState is set to 0
   if(!(isTeam's turn && teamHasEnoughPlayers)){
       change lastPlayed to this team, set field state to open
       return
   }
   locks field's totalPlayers
   if(totalPlayers == 0)
   {
       set field state to own sport
       setPlayingTime to random number
       print(Baseball + ID + Game Started)
   }
   if(totalPlayers =< maxForThisSport)
   {
       increment totalPlayers
       set isPlaying to True
       print(Baseball + ID + Playing at position + totalPlayers)
   }
   if(totalPlayers == maxForThisSport){
       lock readyPlayersCount
       subtract maxForThisSport from readyPlayersCount
       release lock on readyPlayersCount
       lock tryToJoinField
       set tryToJoinField to FALSE
       release lock on tryToJoinField


       The following is for Rugby:
        for  rugby, we keep adding new teams as longs as there are more than two available rugby players, or the 30 player cap is reached
        for rugby we do not start the game until no more teams can be added/. A thread knows it's team is last if there are 28 players onfield or less than two players available
        How can playtime work for rugby players?
           The first player joining gens a random num to be the max playing time
           each rugby pair will generate their own time limit.
           if a rugby pair sees the global leave field flag fire before their own personal time limit, they leave the field
           if their personal time limit expires first...
               how can I set a personal time limit for a rugby thread?
   }


leaveField()
    Should one thread call this?
    Or will all the threads call it? 
    Only directed at threads on field
        Specific lock and signal
    
    Have a variable that once TRUE 
        Set this variable to true, once the game has been run.
    all threads on field try to leave. 
    decrement counter  by 1
    set own playing BOOL to FALSE. 


waitAndRejoin()
   This will be called after a thread is done playing on the field
   generates a random number
   sleeps for that long
   then, runs joinTeam


Thread()
   while (True)
   {
       wait until tryToJoinField == True
       {
           then, run joinField


           wait for isPlaying to == FALSE{
           then, if(currentlyPlaying == TRUE)
           {
           currentlyPlaying = false
           waitAndRejoin(self)
           }
   }
       }
   }



How to determine which threads to waitAndRejoin after a game?
   give each player a currentlyPlaying flag.
   wait for isPlaying to == FALSE{
       then, if(currentlyPlaying == TRUE)
       {
           currentlyPlaying = false
           waitAndRejoin(self)
       }
   }






*/
Tim Stecko and Jacob Bergeron 
CS3013
February 4, 2024
Project_2

To run this program:
    'make all'
And then enter:
    './play'

For Checkpoint:
I have created a main function which creates the # of threads designate for each team.
I spent some time creating files that I could use put helper functions and defines
but ultimately ended up collapsing most of them.
Currently working on understanding how to implement semaphores/mutex/condition variables
to properly allow for synchronization.


This project aims to answer concurrency and synchronization of 3 different
sports teams trying to use a signle shared field. We have attempted multiple 
implementations but eventually tried to settle on this one. We understand that 
the threads must lock before accessing global variables and release at the proper times
to ensure deadlock does not take place. We weren't able to fully achieve this but
got a version as close as we could.

For fairness between sports and to prevent starvation, we implemented a rotating turn system for each sport to get the chance to play, such as Rugby -> BaseBall -> Football -> Rugby. This way no sport can monopolize the playtime.

We try to implement thread-safe synchronization using mutexes for shared resources,
such as playerLock, mutexField, lockReadyBaseball, lockReadyFootball, 
lockReadyRugby, and lockTryToJoinField. We also use conditions like 
condField to awake threads waiting on the field to be open.
We implement synchronization with our mutexes and with each thread 
determining for itself when to join the field and when to leave. 
Ordering the threads to leave the field is not implemented centrally, 
but rather by having each thread set it’s own playing time which is equal to 
that of the first thread that joined the field. This way, each thread has an 
identical leave time, but set that time independently. For rugby players, each pair 
may have set their leave time to exit the field before the first player’s set time.

we make use of helper functions to simplify game logic and make the code more maintainable.
joinTeam notes that a thread is ready to play by adding it to a count of ready threads and
by setting the player’s isReady var to true
joinField has a player try to grab the field lock and add itself to the 
field’s player counter if the player is ready to play. 
leaveField will have the player subtract itself from the field’s player counter 
when the thread’s timer runs out.
waitRandomTime will have the thread wait a random time to simulate the player 
resting before trying to get back onto the field

Our thread function loops through game logic continuously until the user 
disrupts the execution with ctrl+C. It loops through joinTeam,joinField,
leaveField,and waitRandomTime. 

Overall, we believe that the code is very close to being operational.
We really tried to submit a working version of the code, but the late penalties
 will be too high if we spend any longer fixing the bugs in our implementation. 
 Our code is experiencing some deadlock which is probably due to a mutex being held by a thread’s helper function, 
 but it is hard to be sure.
Please understand that we tried our very best to implement the problem, 
and please consider our thought process listed above when grading this assignment.

Thank you









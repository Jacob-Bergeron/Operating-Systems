
In this directory, you should write the program `scheduler.c` 
and compile it into the binary `scheduler` 
(e.g., `gcc -o scheduler scheduler.c -Wall -Werror`). 
You should do it inside the Ubuntu machine you have setup, 
as grading will be done so inside the Ubuntu.

After doing so, you can run the tests from this directory by running the
`./run_tests.sh` script. If all goes well, you will see standard output 
that indicate that all tests are passed.


Passing all tests are not sufficient to obtain all points for this assignment. 
As an example, any assignment in which hardcodes the expected output to pass the 
test case will lead to point deductions.


Jacob Bergeron, Tim Stecko
CS3013 
Project 4 
February 28, 2024

For Checkpoint:
Worked out generic pseudocode for main and the functions. Currently trying to understand 
how to organize the job structs into linked lists, becuase once this is achieved we can pass 
this list to the functions to be able to work on them.


For final:
Our program passes tests 1-15. We are failing on 16 and 17 which is the analysis
of the RR scheudling function. We feel that we are close but weren't able to get the 
correct implementation. 

Our program is organized as follows:
** Line #'s may not be exact **  

Lines 16 - 81:
Helper functions for the linked list.
 
Lines 84 - 148:
Helper functions for our Bubble sort algorithm for finding the shortest job.

Lines 150 - 239:
Functions for policy analysis

Lines 243 - 327:
Scheduling Functions
- FIFO
- SJF
- RR

Lines 330 - 395: 
Main Function







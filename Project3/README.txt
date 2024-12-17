Tim Stecko and Jacob Bergeron
CS3013
February 18, 2024
Project 3

For Checkpoint:
We were working on an implementation before the starter code was posted.
Once code was posted we spent time going through it and decided on a
single-level page table. We are still not 100% sure if this is the format we 
will use but trying it out. Created a struct for PTE's and made an array of them
to represent the page table. Working through understanding the different functions
within pagetable.c. We have a good grasp of the functions in instructions.c, just
waiting to complete pagetable.c before working on those. 


For Final Submission:

Explaining our approach: 
For our page table we implemented a single-level page table.
Each page table consisted of 4 PTE's which were 1 byte in size each. 
These page tables were created on a need basis, meaning we did not initialize 4 page tables
during the start of the program. 

We have spent a lot of time on our implementation and feel confident that it is 
close to being entirely correct. The functions where we go to disk is where we began to run into a 
bit of trouble. Our implemenation for disk was to make the first 80 bytes be for process id 0.
And then have the next 80 bytes for process id 1, and so on. We created logic to handle the proper offset into 
disk based on the pid that was calling the fucntion. 
We use the simple round robin approach to decide who gets evicted next.

Our code compiles but some potential erros pop up. These errors were from the 
code provided by the Prof regarding the Disk functions. We weren't sure if this was intentional or if this
was a mistake. 

To summarize, we feel confident that our code handles the internal mechanisms 
that are neccesary for an mmu but problems arose when trying to evict pages from physmem
to disk as well as bringing data in from disk. 
We have left comments on our code to try and explain our thought process and to make it easier
to understand. 

Please consider our comments and our thought process when grading this project. We spent a lot of time working
together in person and asking questions at office hours, but still couldn't completely finish the implementation.

Thank you 


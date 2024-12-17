// Starting code version 1.1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "mmu.h"
#include "pagetable.h"
#include "memsim.h"


/*
pageTableEntry struct
*/
// 8 bits per PTE -> 1 byte
typedef struct {
	unsigned int pte_VPN : 2;
	unsigned int pte_PFN : 2;
	unsigned int pte_valid: 1;
	unsigned int pte_protection : 1;
	unsigned int pte_present : 1;
	unsigned int pte_referenced : 1;
	
} pageTableEntry;



int pageToEvict = 1; //if using round robin eviction, this can be used to keep track of the next page to evict

// Page table root pointer register values (one stored per process, would be "swapped in" to MMU with process, when scheduled)
typedef struct{
	int ptStartPA;
	int present; 
} ptRegister;

// Page table root pointer register values 
// One stored for each process, swapped in to MMU when process is scheduled to run)
ptRegister ptRegVals[NUM_PROCESSES]; 



/*
 * Public Interface:
 */

/*
 * Sets the Page Table Entry (PTE) for the given process and VPN.
 * The PTE contains the PFN, valid bit, protection bit, present bit, and referenced bit.
 */
void PT_SetPTE(int pid, int VPN, int PFN, int valid, int protection, int present, int referenced) {
	// why do we need to get physcial memory array for this function??
	char* physmem = Memsim_GetPhysMem();
	assert(PT_PageTableExists(pid)); // page table should exist if this is being called

	// since pageTableExists is true, then the page table is known to be existing in physical mem, since it's present bit is 1

	int PTStartAddress = ptRegVals[pid].ptStartPA;
	int start = physmem[PTStartAddress];

	//todo 
	
	// loop through page table
	// look for the next blank PTE spot in the Page table	
	// if spot is empty, add data
	// else the loop just continues
	for(int i = start; i <= start + PAGE_SIZE; i++){
		pageTableEntry* page_table_entry = (pageTableEntry*)&physmem[i];
		if(page_table_entry->pte_present == 0){
			// if the spot is empty -> add the data for the PTE
			page_table_entry->pte_VPN = VPN;
			page_table_entry->pte_PFN = PFN;
			page_table_entry->pte_valid = valid;
			page_table_entry->pte_protection = protection;
			page_table_entry->pte_present = present;
			page_table_entry->pte_referenced = referenced;

			// break if data gets added
			break;
		} 
	}

	// what do we do with the pid here??
	//	We use the pid to find the pointer to the start of the page table for this process ID
	//ptRegVals[pid] = commented this code out bc I dont think we change the page table address here, just an entry in the page table.
	//so, next step is that we need to make page_table be the index VPN of the array starting at ptRegVals[pid]
	//need to keep a pointer so that we are actually editing the values of the page table, instead of a copy
	// Did i do the pointer logic correctly?
	// pageTableEntry* page_table = (pageTableEntry*)(physmem + ptRegVals[pid].ptStartPA + VPN * sizeof(pageTableEntry));

	// page_table[VPN].pte_PFN = PFN;
	// page_table[VPN].pte_valid = valid;
	// page_table[VPN].pte_protection = protection;
	// page_table[VPN].pte_present = present;
	// page_table[VPN].pte_referenced = referenced;
	
	// at this point, the page table was found in physmem and it's values set, that should be all this function needs to do.
}

/* 
 * Initializes the page table for the process, and sets the starting physical address for the page table.
 * 
 * After initialization, get the next free page in physical memory.
 * If there are no free pages, evict a page to get a newly freed physical address.
 * Finally, return the physical address of the next free page.
 */
int PT_PageTableInit(int pid, int pa){
	// we need to put the page table in our physical memory, so get physmem
	char* physmem = Memsim_GetPhysMem();
	// pa = Memsim_FirstFreePFN();

	if(pa == -1){ //if pa == -1, then there isn't a free frame for the pagetable so we need to evict a page to make space for it
		// Evict one and use the new space
		// what is the value for 2nd argument int* diskFrameNum: is it pid, ptRegVals[pid].ptStartPA, something else??
		PTEvictPageCommon(&pa, &pid, 1);
		pa = Memsim_FirstFreePFN();
	}
	//now we know that pa is the address of a physical free page

	//next we need to calc which addr to put this pageTable in our physmem
	char* startOfPTAddr = physmem + pa * PAGE_SIZE;

	//now we need to an array of Page table entries with as many entries as there are Frames
	pageTableEntry* page_table = (pageTableEntry*)(startOfPTAddr); //pointer to start of array
	for(int i = 0; i < PAGE_SIZE; i++){
		//zeroing out all the values at first, NOT SURE if that's the proper thing to do
		page_table[i].pte_PFN = 0;
		page_table[i].pte_VPN = 0;
		page_table[i].pte_valid = 0;
		page_table[i].pte_protection = 0;
		page_table[i].pte_present = 0;
		page_table[i].pte_referenced = 0;
	}

	//next, we need to set the register to the addr of where we put the PT
	ptRegVals[pid].ptStartPA = pa;
	ptRegVals[pid].present = 1;

	//is this the right thing to return?
    pa = Memsim_FirstFreePFN();
	return pa;

	//perhaps need to free up a new frame to return? idk
 }

/* 
 * Check the ptRegVars to see if there is a valid starting PA for the given PID's page table.
 * Returns true (non-zero) or false (zero).
 */
 int PT_PageTableExists(int pid){

	// if ptStartPa is between 0 and 3 (ie. 0, 1, 2 or 3), then there is a valid starting PA
	if(ptRegVals[pid].ptStartPA >= 0 && ptRegVals[pid].ptStartPA <= 3 ){
		return 1;
	}
	// else ptStartPA is not 0, 1, 2 or 3
 	return 0; 
 }

/* 
 * Returns the starting physical address of the page table for the given PID.
 * If the page table does not exist, returns -1.
 * If the page table is not in memory, first swaps it in to physical memory.
 * Finally, returns the starting physical address of the page table.
 */
int PT_GetRootPtrRegVal(int pid){
	//todo

	// If the page table does not exist, return -1
	if(PT_PageTableExists(pid) == 0 ){
		return -1;
	}
	// if the page table is not in physical memory
	else if(ptRegVals[pid].present != 1){
		// swap page table from disk
		// read frame from disk 
		// since disk is set up in order of the proccesses
		// the diskAddr passed to Disk_ReadFrame will be the pid
		Disk_ReadFrame(pid, ptRegVals[pid].ptStartPA);
	}

	// else just return the starting physical address of the page table	

	// Return the starting physical address of the page table
	return ptRegVals[pid].ptStartPA;
}


/*
Professor Code for Evict Logic
*/


/* Returns next page selected for eviction, based on policy. Currently simple Round Robin. */

int PTNextEvictionRR() {

	int nextPgToEvict = pageToEvict;

	pageToEvict++;

	pageToEvict = pageToEvict % NUM_PAGES; //wraps around to first page if it goes over

	return nextPgToEvict;

}



/* 

 * Some common code for evicting a page, and writing it to disk.

 * Selects the next physical frame to evict, and writes it's data to the disk.

 * Returns the physical frame number of the evicted page, and the disk frame number it was written to.

 */

int PTEvictPageCommon(int* targPFN, int* diskFrameNum, int verbose) {

	char* physmem = Memsim_GetPhysMem();

	FILE* swapFile = MMU_GetSwapFileHandle();

	int fd = fileno(swapFile);

	if (Disk_Flush(fd, swapFile) == -1) {

		return -1;

	} else { 

		*targPFN = PTNextEvictionRR(); // call the RR policy decider
		
		*diskFrameNum = Disk_Write(swapFile, *targPFN, verbose);

		// check if targPFN is a page table by going through pt registers, 
		// and if targPFN matches a register's PA, then set that register to not present
		for(int i = 0; i < NUM_PROCESSES; i++){
			if(ptRegVals[i].ptStartPA == physmem[*targPFN]){
				ptRegVals[i].present = 0;
				break;
			}
		}
		
	}

}

/*
Our PT_Evict() As of 2/17
CURRENTLY NOT BEING USED
*/

/*
 * Evicts the next page. 
 * Updates the corresponding information in the page table, returns the PA of the evicted page.
 * 
 * The supplied input and output used in autotest.sh *RR tests, uses the round-robin algorithm. 
 * You may also implement the simple and powerful Least Recently Used (LRU) policy, 
 * or another fair algorithm.
 */
int PT_Evict() {
	char* physmem = Memsim_GetPhysMem();
	FILE* swapFile = MMU_GetSwapFileHandle();

	//todo
    // Remove page from Physmem
    // if the page to remove is a PAGE TABLE
        // change root pointer of that PAGE TABLE 
        
    // if the page being removed is general DATA
        // update cooresponding page table -> page table entry not present

    //if

	return 0; //todo
}

/*
 * Searches through the process's page table. If an entry is found containing the specified VPN, 
 * return the address of the start of the corresponding physical page frame in physical memory. 
 *
 * If the physical page is not present, first swaps in the physical page from the physical disk,
 * and returns the physical address.
 * 
 * Otherwise, returns -1.
 */
int PT_VPNtoPA(int pid, int VPN){
	char *physmem = Memsim_GetPhysMem();

    int PTStartAddress = ptRegVals[pid].ptStartPA;
	int start = physmem[PTStartAddress];
	

	//todo 
	
	// loop through page table
	for(int i = start; i <= start + PAGE_SIZE; i++){
		// casting the spot in physmem to a PTE type
		pageTableEntry* page_table_entry = (pageTableEntry*)&physmem[i];
		// if VPN equals the correct index 
		if(page_table_entry->pte_VPN == VPN){
			// get the physical frame number
			return page_table_entry->pte_PFN;
		} 
	}

	// else (physical page is not present)
		
			// swap in physical page from physical disk


			// Our offset for disk read frame is the the pid * the number of pages each process has 
			//	(1 for the page table plus however many pages per process) times the number of bytes per page
			int firstFree = Memsim_FirstFreePFN(); //need space to make space to swap in the physical page from physical disk
			if(firstFree == -1){ // if -1, then there is no free space right now, so we need to evict
				int* pointer;
				PTEvictPageCommon(pointer, &pid,1); // we dont need to save the values for evict, so we set them to 1
				firstFree = Memsim_FirstFreePFN(); //save the pfn of the newly freed page
			}

			// arg1: diskAddr
			// arg2: targPFN (where in physmem you want to add data)
			Disk_ReadFrame((pid*((1+NUM_PAGES)*PAGE_SIZE) + VPN * PAGE_SIZE),firstFree); //write to the newly freed page

			// return the physical address
			return firstFree;

	return -1;
}

/*
 * Finds the page table entry corresponding to the VPN, and checks
 * to see if the protection bit is set to 1 (readable and writable).
 * If it is 1, it returns TRUE, and FALSE if it is not found or is 0.
 */
int PT_PIDHasWritePerm(int pid, int VPN){
	char* physmem = Memsim_GetPhysMem();
    

    int PTStartAddress = ptRegVals[pid].ptStartPA;

	// This should be the start of each set of 16 bytes.
	// this is either: 0, 15, 31, 47, 
	// or is it: 0, 16, 32, 48??
	int start = physmem[PTStartAddress];
	
	// loop through the page table to find the PTE that matches the VPN
	for(int i = start; i <= start + PAGE_SIZE; i++){
		pageTableEntry* page_table_entry = (pageTableEntry*)&physmem[i];
		// this implemenation is wrong, but I feel that the idea is very close
		if(page_table_entry->pte_VPN == VPN){
			// if the pte protection bit is set to 1
			if(page_table_entry->pte_protection == 1){
				// am i returning TRUE or 1?
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
	}
}

/* 
 * Initialize the register values for each page table location (per process).
 * For example, -1 for the starting physical address of the page table, and FALSE for present.
 */
void PT_Init() {

	for(int i = 0; i < NUM_PROCESSES; i++){
		ptRegVals[i].ptStartPA = -1;
		ptRegVals[i].present = 0;
	}
	return; 
}


/*
Helper functions for dealing with everything to do with DISK
*/

/* 

 * Flush the file to disk, and then flush the disk to ensure the data is written.

 * Seeks over the file to the end, and then flushes the file to disk.

 * Returns -1 if there was an error, and 0 if the seek was successful.

 */

int Disk_Flush(int fd, FILE* swapFile) {

	fflush(swapFile);

	off_t end_offset = lseek(fd, 0, SEEK_END);

	if(end_offset == -1){

		printf("Error on disk. Could not write to disk.\n");

		return -1;

	}

	fflush(swapFile);

	return 0;
}

/*

 * Write a PAGE_SIZE worth of bytes from the physical memory starting at the targPFN to the offset in the file.

 * Returns the offset in the file where the data was written.

 */

int Disk_Write(FILE* swapFile, int targPFN, int verbose) {

	assert(PAGE_START(targPFN) % PAGE_SIZE == 0); // only frame-aligned access from this file

	char* physmem = Memsim_GetPhysMem();

	int diskFrameNum = ftell(swapFile);

	fwrite(&physmem[PAGE_START(targPFN)], sizeof(char), PAGE_SIZE, swapFile);

	fflush(swapFile);

	if (verbose) {

		printf("Swapped Frame %d to disk at offset %d.\n", targPFN, diskFrameNum);

	}

	return diskFrameNum;

}


/*

 * Read a PAGE_SIZE worth of bytes from the offset in the file to the physical memory starting at the targPFN.

 * Returns the number of bytes read.

 */

int Disk_ReadFrame(int diskAddr, int targPFN) {

	assert(PAGE_START(targPFN) % PAGE_SIZE == 0); // only frame-aligned access from this file

	FILE* swapFile = MMU_GetSwapFileHandle();

	int fd = fileno(swapFile);

	char* physmem = Memsim_GetPhysMem();

	if (lseek(fd, PAGE_START(diskAddr), SEEK_SET) == -1) {

		printf("Error on disk. Could not read from disk.\n");

		return -1;

	} 

	fread(&physmem[PAGE_START(targPFN)], sizeof(char), PAGE_SIZE, swapFile);

}

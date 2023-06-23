/*
#include "page.h"
#include <stddef.h>

struct ppage physical_page_array[128]; // 128 pages, each 2mb in length covers 256 megs of memory
struct ppage* free_physical_pages = NULL; //initialize head of free list

				   
void init_pfa_list(void) {
//will initialize the linked list of free pages. It will loop through every element of your statically allocated
//physical page array and link it into the list.

	free_physical_pages=&physical_page_array[0]; //head points at first free page
	extern int _end_kernel;                   
	void* starting_address = (((unsigned int)&_end_kernel)&0xfffff000) + 4096; //accquire starting address using end kernel

       	//initialize first page out of loop
	physical_page_array[0].physical_addr = starting_address; //set pa value to starting address
	physical_page_array[0].next = &physical_page_array[1]; //set next of array member to next member
	physical_page_array[0].prev = &free_physical_pages;    //set prev of array member to head

	//loop from second page to 127
	for(int i=1; i<127; i++){
		physical_page_array[i].physical_addr = physical_page_array[i-1].physical_addr+4096;  //update pa by adding 4 kilobytes each iteration 
		physical_page_array[i].next= &physical_page_array[i+1];   //set nexts and prevs to corresponding array members
		physical_page_array[i].prev= &physical_page_array[i-1];
	}

	//last page
	physical_page_array[127].physical_addr = physical_page_array[126].physical_addr+4096;  //do the last one by itself b/c next needs to be null 
	physical_page_array[127].next=NULL;
	physical_page_array[127].prev = &physical_page_array[126];
}


struct ppage *allocate_physical_pages(unsigned int npages) {
//allocates one or more physical pages from the list of free pages. The input to this function
//npage specifies the number of pages to allocate. The function will unlink the requested number of pages from the free list
//and create a new separate list. It will return a pointer to the new list

	struct ppage* iterator=free_physical_pages; //first node in the free list
	struct ppage* allocd_list=free_physical_pages; //stores head of free list	
	
	for(int i=0; i<npages; i++){  //iterate through pages
		iterator=iterator->next; //traverse for num of pages
	}
	iterator->prev->next=NULL; //cut allocated list from free list by removing next
	iterator->prev=NULL; //cut allocated list from free by removing prev
	free_physical_pages = iterator; //iterator is head of free listt
	return allocd_list;  //this now points to allocated list
}

void free_physical_pages_function(struct ppage *ppage_list){
//returns a list of physical pages to the free list. Basically does the opposite of allocate physical pages. returns to free list at the begining
	
	struct ppage* iterator=ppage_list;  

	while(iterator->next != NULL){      //iterate to end of allocated list
                iterator=iterator->next; 
        }
	iterator->next = free_physical_pages;  //set the next to head of free list
	free_physical_pages->prev = iterator;   //set the head of free list prev to the itterator
	free_physical_pages=ppage_list;       //reset head of free list to start of freed portion
}
*/

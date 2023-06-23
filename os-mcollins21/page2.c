#include "page.h"
#include "ptr.h"
#include "rprintf.h" //for the tester method
//#include "pgdir.h"
#include <stddef.h>

struct ppage physical_page_array[128];
struct ppage* free_physical_pages = NULL;

                          
void init_pfa_list(void) {


      //defining variable
      extern int _end_kernel;
      //rounding up _end_kernel value
      void* addressStart = (void*)(((uintptr_t)&_end_kernel + 4095) & ~((uintptr_t)0xFFF));


      //optimization - initialization
      for ( int i = 0; i < 128; i++) {
	  physical_page_array[i].physical_addr = (i == 0) ? addressStart : (physical_page_array[i - 1].physical_addr + 4096);
	  physical_page_array[i].next = (i < 127) ? &physical_page_array[i + 1] : NULL;
	  physical_page_array[i].prev = (i > 0) ? &physical_page_array[i - 1].next : &free_physical_pages;
      }

      free_physical_pages = &physical_page_array[0];
}


struct ppage *allocate_physical_pages(unsigned int npages) {

      //checks if allocate physical pages is empty or greater than the maximum
      if(npages == 0 || npages > 128) 
	      return NULL;
      struct ppage* current = free_physical_pages;
      struct ppage* start = NULL;
      unsigned int count = 0;

      while (current) { 
	      count++;

	      if (count == npages){
		      start = current - (npages - 1);
		      if (start->prev){
			      *(start->prev) = current->next;
		      }else{
			      free_physical_pages = current->next;
		      }

		      if (current->next)
			      current->next->prev = start->prev;

		      start->prev = NULL;
		      current->next = NULL;
		      break;
	      }
	      current = current->next;
      }

      return start;
}


void free_physical_pages_function(struct ppage *ppage_list){
      
      //NULL check
      if (!ppage_list)
	      return;
      
      struct ppage* iterator = ppage_list;
      while (iterator->next)
	      iterator = iterator->next;

      iterator->next = free_physical_pages;

      //update prev pointer to current head of list
      if(free_physical_pages)
	      free_physical_pages->prev = iterator;
	
      //set prev pointer of the new head NUL
      ppage_list->prev = NULL;

      //update head of list to be ppage_list
      free_physical_pages = ppage_list;     

}

//helper method for free_physical_pages_function - tester method
void test_free_physical_pages_function() {

	struct ppage *allocated_pages = allocate_physical_pages(5);


	free_physical_pages_function(allocated_pages);

	if(!allocated_pages){
		return "TEST FAILED";
	}

	struct ppage *iterator = free_physical_pages;
	int count = 0;
	while(iterator) {
		count++;
		iterator = iterator->next;
	}

	if(count >= 5) {
		return "TEST PASSED";
	} else {
		return "TEST COUNT FAILED";
	}
}


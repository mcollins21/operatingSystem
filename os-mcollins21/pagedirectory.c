#include <stdint.h>
#include <string.h>
#include "pagedirectory.h"
#include "page.h"

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));

struct page pt[1024] __attribute__((aligned(4096))); 
struct page another_pt[1024] __attribute__((aligned(4096))); 


void *map_pages(void *vaddr, struct physical_page *ppage, struct page_directory_entry *pd) {
	
	uint32_t vpn = (long int)vaddr>>12; //virtual page number
	
	uint32_t directory_number = vpn>>10; //page directory number
	
	uint32_t page_table_number = vpn&0x3FF; //page table number

	
	pd[directory_number].rw=1;
	pd[directory_number].present=1;
	pd[directory_number].frame=(long int)another_pt>>12;
	
	//assign physical address
	another_pt[page_table_number].frame= (int)ppage>>12;
	another_pt[page_table_number].rw=1;
	another_pt[page_table_number].present=1;
	
	return 0;


}



void map_pages_init() {


	for(int i=0;i<=1024; i++) { 
		pd[i].present=0;
		pd[i].rw=0;
		pd[i].user=0;
		pd[i].writethru=0;
		pd[i].cachedisabled=0;
		pd[i].accessed=0;
		pd[i].pagesize=0;
		pd[i].ignored=0;
		pd[i].os_specific=0;
		pd[i].frame=0;
	}


	pd[0].present=1;
        pd[0].rw=1;

	pd[0].frame=(long int)pt>>12;



	for(int i =0; i<=1023;i++){
                pt[i].frame =i; //physical address
                pt[i].present =1;
                pt[i].rw=1;
        }

//Setting bits 0 and 31 in CR1

	asm("mov %0,%%cr3"
	:
	: "r"(pd)
	:);



	asm("mov %cr0, %eax\n"
        "or $0x80000001,%eax\n"
        "mov %eax,%cr0");

}

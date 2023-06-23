//#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "page.h"
#include "sleep.h"
#include "pagedirectory.h"
//cant use stdio.h anywhere in project
//#include <stdlib.h> // included for the the exit(0)

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6
#define HEIGHT	25
#define WIDTH	80


unsigned int multiboot_header[] __attribute__ ((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};


int offset = 0;

void putch(char type){
	
	unsigned short *vram = (unsigned short*)0xb8000;   //base address of video mem
	const unsigned char color = 7;

	if(type == '\n') {
		offset = (offset/WIDTH+1)*WIDTH;
	}else{
		unsigned short data = (color << 8) + type;
		vram[offset] = data;
		offset++;
	}

	if(offset>=2000) {
		for(size_t y=1; y<HEIGHT; y++) {
			for(size_t x=0; x<WIDTH; x++) {
			size_t src = y * WIDTH + x;
			size_t dst = (y-1) * WIDTH + x;
			vram[dst] = vram[src];
			}
		//SHIFTS CONTENT UP BY ONE ROW
	}
		for(size_t x = 0; x < WIDTH; x++) {
			vram[(HEIGHT-1)*WIDTH+x] = (color << 8) + ' ';
		//THIS FOR LOOP CLEARS THE LAST ROW  
		}

		offset = (HEIGHT-1) * WIDTH;
		//SETS THE OFFSET TO BEGINNING OF LAST ROW

	}
}


void putptr(void *p) {
	//cast ptr addr to unsigned long int
	unsigned long long int n = (unsigned long long)p;

	//convert address to string
	unsigned char buf[16]; //max 16 hex digits for 64-bit addr

	int i = 0; //counter	
	
	//converting dec to hex
	do {
		buf[i++] = "0123456789ABCDEF"[n % 16];
		n /= 16;
	} while (n > 0);

	//printing in reverse order
	while(i-- > 0)
		putch(buf[i]);


}


uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}


void outb (uint16_t _port, uint8_t val) {
__asm__ __volatile__ ("outb %0, %1" : : "a" (val), "dN" (_port) );
}


unsigned char keyboard_map[128]=  //needs fixing
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0, 
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0, 
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};


//KEYBOARD DRIVER
/*
char keyboardDriver(void) {
	while(1){
		uint8_t status = inb(0x64);
		if (status & 1) {
			uint8_t scancode = inb(0x60);
			if(keyboard_map[scancode] >= 1 && keyboard_map[scancode] != 0){
				esp_printf(putch, "%c", keyboard_map[scancode]);
		}
	}
}
}
*/

char buf[4096];
void main() {


	struct ppage *free_physical_pages = NULL;

	struct page_directory_entry pd[1024] __attribute__((aligned(4096)));

//HOMEWORK 2 (MEMORY MANAGEMENT) - PAGE2.C | PAGE.H | PAGE.O

        esp_printf(putch, "INITIALIZING PAGE FRAME ALLOCATOR LIST.\n");
	
	esp_printf(putch, "\n");
	//sleep(20);
	esp_printf(putch, "...");
	sleep(20);
	esp_printf(putch, "\n");
	esp_printf(putch, "\n");
	//LOADING SEQUENCE 

	init_pfa_list();
	//INITALIZING PFA LIST
	
	unsigned int pages_to_allocate = 5;

	struct ppage *allocated_pages = allocate_physical_pages(pages_to_allocate);
	
	if(allocated_pages) {
		esp_printf(putch, "PAGES HAVE BEEN ALLOCATED \n\n");
	} else {
		esp_printf(putch, "PAGES HAVE NOT BEEN ALLOCATED \n\n");
	}



	if(allocated_pages) {
		esp_printf(putch, "ALLOCATED PHYSICAL ADDRESSES:\n");
		struct ppage *iterator = allocated_pages;
		int i = 1;
		while(iterator) {
			putptr(iterator->physical_addr);
			esp_printf(putch, "\n");
			iterator = iterator->next;
			i++;
		}

	} else { 
		esp_printf(putch, "ALLOCATION FAILED. PLEASE TRY AGAIN. ABORTING...");
		return 1;
	}




	esp_printf(putch, "\nINITIALIZING VIRTUAL MEMORY MAPPING OF PAGES...\n");
	sleep(4);

	map_pages_init(pd);

	esp_printf(putch, "VIRTUAL MEMORY MAP INITIALIZED.\n\n");
	sleep(1);
	
	//defining variables needed for mem mapping
	void *virtual_address = (void *)0x100; //dummy value
	struct physical_page *physical_page = allocated_pages;


	//mapping pages
	esp_printf(putch, "MAPPING PAGES... \n");
	sleep(4);

	map_pages(virtual_address, physical_page, pd);

	//testing map_pages()
	uint32_t test_value = 0xDEADBEEF;

	//writing a test value to mapped virt addr
	*(uint32_t *)virtual_address = test_value;

	//reading test value to mapped virt addr
	uint32_t read_value = *(uint32_t *)virtual_address;

	if(read_value == test_value) {
		esp_printf(putch, "PAGES SUCESSFULLY MAPPED. \n\n");
		sleep(1);
	} else {
		esp_printf(putch, "PAGES FAILED TO MAP. PLEASE TRY AGAIN. ABORTING... \n\n");
		sleep(4);
		return 1; //returns error
	}


	//freeing physical pages
	esp_printf(putch, "FREEING PAGES... \n");
	sleep(4);



	//confirmed works
	//esp_printf(putch, test_free_physical_pages_function());

	free_physical_pages_function(allocated_pages);


	esp_printf(putch, "SUCCESSFULLY FREED PAGES. \n\n");
	sleep(4);
	
	
	//initializing FAT fs
	esp_printf(putch, "INITIALIZING FAT FILE SYSTEM...\n");
	sleep(4);






	//esp_printf(putch, "PAGE FRAME ALLOCATOR INITIALIZED.\n");
	


//HOMEWORK 1 (PRINTS CHARACTERS TO TERMINAL)	//COMMENTED OUT


/* 
	esp_printf(putch, "This is a sentence that is meant to take up all the spaces so that the line wraps around. \n");
	esp_printf(putch, "These are some new characters that will be printed underneath the inital line. \n");
	esp_printf(putch, "This next line will add even more characters to print them below the first and second lines. \n");
	esp_printf(putch, "Successive calls to putc should write characters to the terminal at sequential locations on the screen. It should not keep writing characters to the upper left corner of the screen. To make this work, you will need to keep track of the offset of the last character that you wrote to video memory and increment that offset each time you write a new character. When you reach the bottom of the screen (past the 24th row of characters), your terminal driver should scroll the contents of the screen up. Do this by overwriting the first line of characters with the contents of the second line, overwrite the second line of characters with the contents of the third line, and so on. \n");
	esp_printf(putch, "Successive calls to putc should write characters to the terminal at sequential locations on the screen. It should not keep writing characters to the upper left corner of the screen. To make this work, you will need to keep track of the offset of the last character that you wrote to video memory and increment that offset each time you write a new character. When you reach the bottom of the screen (past the 24th row of characters), your terminal driver should scroll the contents of the screen up. Do this by overwriting the first line of characters with the contents of the second line, overwrite the second line of characters with the contents of the third line, and so on.Successive calls to putc should write characters to the terminal at sequential locations on the screen. It should not keep writing characters to the upper left corner of the screen. To make this work, you will need to keep track of the offset of the last character that you wrote to video memory and increment that offset each time you write a new character. When you reach the bottom of the screen (past the 24th row of characters), your terminal driver should scroll the contents of the screen up. Do this by overwriting the first line of characters with the contents of the second line, overwrite the second line of characters with the contents of the third line, and so on. \n");
	esp_printf(putch, "Successive calls to putc should write characters to the terminal at sequential locations on the screen. It should not keep writing characters to the upper left corner of the screen. To make this work, you will need to keep track of the offset of the last character that you wrote to video memory and increment that offset each time you write a new character. When you reach the bottom of the screen (past the 24th row of characters), your terminal driver should scroll the contents of the screen up. Do this by overwriting the first line of characters with the contents of the second line, overwrite the second line of characters with the contents of the third line, and so on.Successive calls to putc should write characters to the terminal at sequential locations on the screen. It should not keep writing characters to the upper left corner of the screen. To make this work, you will need to keep track of the offset of the last character that you wrote to video memory and increment that offset each time you write a new character. When you reach the bottom of the screen (past the 24th row of characters), your terminal driver should scroll the contents of the screen up. Do this by overwriting the first line of characters with the contents of the second line, overwrite the second line of characters with the contents of the third line, and so on. \n");

*/


//KEYBOARD DRIVER
        while(1){
                uint8_t status = inb(0x64);
                if (status & 1) {
                        uint8_t scancode = inb(0x60);
                        if(keyboard_map[scancode] >= 1 && keyboard_map[scancode] != 0){
                                esp_printf(putch, "%c", keyboard_map[scancode]);
                }
        }
}


}



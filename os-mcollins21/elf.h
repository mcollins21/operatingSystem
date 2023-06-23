


#ifndef __ELF_H
#define __ELF_H

#define PROG_HEADER_TYPE_NULL    0
#define PROG_HEADER_TYPE_LOAD    1
#define PROG_HEADER_TYPE_DYNAMIC 2
#define PROG_HEADER_TYPE_INTERP  3
#define PROG_HEADER_TYPE_NOTE    4
#define PROG_HEADER_TYPE_SHLIB   5
#define PROG_HEADER_TYPE_PHDR    6
#define PROG_HEADER_TYPE_LOOS    0x60000000
#define PROG_HEADER_TYPE_HIOS    0x6fffffff
#define PROG_HEADER_TYPE_LOPROC  0x70000000
#define PROG_HEADER_TYPE_HIPROC  0x7fffffff

struct elf32_header {
    uint32_t  magic;          // Magic number should be 0x7f 'E' 'L' 'F'
    uint8_t   elfclass;       // 1- 32bit, 2- 64bit ELF file
    uint8_t   endianness;     // 1- Little endian, 2- Big endian
    uint8_t   elfversion;     // Version of the ELF file
    uint8_t   osabi;          // ABI
    uint8_t   osabiversion;   // ABI version
    uint8_t   pad[7];         // Unused
    uint16_t  e_type;         // Object file type
    uint16_t  e_machine;      // Architecture
    uint32_t  e_version;      // Object file version
    uint32_t  e_entry;        // Entry point virtual address
    uint32_t  e_phoff;        // Program header table file offset
    uint32_t  e_shoff;        // Section header table file offset
    uint32_t  e_flags;        // Processor-specific flags
    uint16_t  e_ehsize;       // ELF header size in bytes
    uint16_t  e_phentsize;    // Program header table entry size
    uint16_t  e_phnum;        // Program header table entry count
    uint16_t  e_shentsize;    // Section header table entry size
    uint16_t  e_shnum;        // Section header table entry count
    uint16_t  e_shstrndx;     // Section header string table index
}__attribute__((packed));

struct elf_program_header {
    uint32_t type;          // Segment type
    uint32_t offset;        // Offset of segment within the image
    uint32_t vaddr;       // Virtual address of the segment
    uint32_t paddr;         // Physical address of segment
    uint32_t filesz;        // Size of the segment in the file image
    uint32_t memsz;         // Size of the segment in memory
    uint32_t flags;         // Segment-dependent flags
    uint32_t align;         // 0/1: No alignment. Otherwise, should be power of 2
};

//int parse_elf_header(void *start);

//---------------------------- EXPERIMENTAL CODE BEGINS HERE


void* elfLoader(const char* filename) {

	int fd = open(filename, 0);
	if (fd < 0) {
		return NULL;
	}

	//FINDS FILE SIZE
	off_t size = lseek(fd, 0, SEEK_END);
	if (size < 0) {
		close(fd);
		return NULL;


//DEFINING MEMSET AND MEMCPY


static inline void memset(void *dest, char val, size_t len) {
	char *dist = (char*)dest;
	for (; len !=0; len--) *dist++ = val;
}

static inline void memcpy (void *dest, const void *src, size_t len) {
	char *d = (char*)dest;
    	const char *s = (const char*)src;
    	while (len--) *d++ = *s++;
}

//MAPS SPECIFIED MEM REGION WITH FLAGS AND RETURNS POINTER TO MAPPED MEM

static void* map_memory(uint32_t addr, uint32_t size, uint32_t flags) {
    // Round the size up to the nearest page boundary
    size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    // Map the memory region
    void *ret = (void*)addr;
    memset(ret, 0, size);

    // Return the pointer to the mapped memory
    return ret;
}


int parse_elf_header(void *start);

	struct elfHeader *header = (struct elf32_header*)start;

	//CHECKS IF THIS IS AN ELF FILE
	if (header->magic != 0x7F454c46 || header->elfclass != 1 ||
	   header->endianness != 1 || header->elfversion != 1 ||
	   header -> osabi != 0 || header->osabiversion != 0) {
	   return -1;
	}

	if (header->e_type != 2) {
		return -1;
	}

	//PARSES THE PROGRAM HEADERS
	struct elfProgramHeader *phdr = (struct elf_program_header*)((unit32_t)start + header->e_phoff);

	//LOADS EACH PROGRAM SEGMENT INTO MEM
	for (int i = 0; i < header->e_phnum; i++) {
		if(phdr[i].type != PROG_HEADER_TYPE_LOAD) {
			continue;
		}

	//MAPS MEMORY FOR SEGMENT
	void *mem = map_mem(phdr[i].vaddr, phdr[i].memsz, phdr[i].flags);

	//USING MEMCPY TO COPY SEGMENT DATA INTO MEM
	memcpy(mem, (void*)(((unit32_t)start + phdr[i].offset), phdr[i].filesz);

	//SETTING ZERO TO ANY REMAINING MEM IN SEG
	memset(mem + phdr[i].filesz, 0, phdr[i].memsz - phdr[i].filesz);

	}
	
	//GOES TO ENTRY POINT OF ELF
	void (*entry)() = (void(*)())header->e_entry;
	entry();
	
	return 0;

}

#endif

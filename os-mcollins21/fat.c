/*
#include <stddef.h>
#include <stdbool.h>
#include "ide.h"
#include "fat.h"
#include <string.h>
#include <stdlib.h>

#define BOOT_SIGNATURE 0xAA55
#define SECTOR_SIZE 512
#define FAT_OFFSET 2048

static struct boot_sector *bs;
static struct root_directory_entry *r;
static struct file *f = NULL;
static struct file file_instance;
unsigned char *fatbuff = NULL;
unsigned char *rdbuff = NULL;
static unsigned char buff[SECTOR_SIZE];

int reserved_region_size;
int fat_region_offset;
int fat_region_size;
int rde_region_offset;
int rde_region_size;

bool compare_buffers(const void* buf1, const void* buf2, size_t size) {
    const unsigned char* b1 = buf1;
    const unsigned char* b2 = buf2;
 
    for(size_t i = 0; i < size; i++) {
	char b1_upper = (b1[i] >= 'a' && b1[i] <= 'z') ? b1[i] - 'a' + 'A' : b1[i];
	char b2_upper = (b2[i] >= 'a' && b2[i] <= 'z') ? b2[i] - 'a' + 'A' : b2[i];
	if(b1_upper != b2_upper && !(b1_upper == 0x20 && b2_upper == '\0'))
	    return false;
    }
    return true;
}

void *bufcpy(void *dest, const void *src, size_t n) {
    char *dest_ptr = (char*)dest;
    const char *src_ptr = (const char*)src;

    for (size_t i = 0; i < n; i++)
        dest_ptr[i] = src_ptr[i];

    return dest;
}

int fatInit(){
    ata_lba_read(FAT_OFFSET, buff, 1);
    bs = (struct boot_sector *) buff;
    static unsigned char fatbuff[4096] = {0};
    static unsigned char rdbuff[SECTOR_SIZE * 32] = {0};
    if(bs->boot_signature != BOOT_SIGNATURE){
        return -1;
    }

    reserved_region_size = bs->num_reserved_sectors * SECTOR_SIZE;
    fat_region_offset = FAT_OFFSET + reserved_region_size;
    fat_region_size = (bs->num_fat_tables * bs->num_sectors_per_fat) * SECTOR_SIZE;
    rde_region_size = (bs->num_root_dir_entries) * 32;
    rde_region_offset = fat_region_offset + fat_region_size;
    int fat_sectors = bs->num_fat_tables * bs->num_sectors_per_fat;
  

    ata_lba_read(fat_region_offset, fatbuff, fat_sectors);
    ata_lba_read(rde_region_offset, rdbuff, (rde_region_size / SECTOR_SIZE));

    return 1;
}

int fatOpen(const char *path) {
    r = (struct root_directory_entry *)rdbuff;
    //char formatted_file_name[11] = {0};
    //int path_idx = 0 , name_idx = 0;	


    int i = 0; int j = 0;
    while(path[path_idx] != '\0' && name_idx < 11) {
	    if (path[path_idx] == '.') {
		    name_idx = 8;
	    } else {
		    formatted_file_name[name_idx++] = path[path_idx];
	    }
	    path_idx++;
    }

    /*
    int rde_offset = 0;
    bool found = false;

    char fat16_path[12] = {0};
    int i = 0;
    int j = 0;
    while(path[i] && j < 11) {
	    if(path[i] == '.'){
		    i++;
		    j = 8;
	    } else {
		    fat16_path[j++] = path[i++];
	    }
    }
 	
    while (rde_offset < rde_region_size) {
	    if( compare_buffers(fat16_path, r->file_name, 11)) {
		    found = true;
		    break;
	    }

	    rde_offset += 32;
	    r = (struct root_directory_entry *)&rdbuff[rde_offset];
    }

    /*
    for (rde_offset = 0; rde_offset < rde_region_size; rde_offset += 32) {
	r = (struct root_directory_entry *)&rdbuff[rde_offset];
   
	if((r->attribute & 0x1F) == 0) {
		if (compare_buffers(formatted_file_name, r->file_name, 11)) {
	found = true;
	break;
	}
    }	
    }
*/
/*
    if (found) {
	   f->start_cluster = r->cluster;
        return rde_offset;
    } else {
        return -1;
    }

*/

/*	
}

void fatRead(int rde_offset, char *databuff, int sizedatabuff) {
    unsigned char bbuff[SECTOR_SIZE];
    int clusternum = ((struct root_directory_entry *)&rdbuff[rde_offset])->cluster;

    int file_region_offset = FAT_OFFSET
                             + reserved_region_size
                             + fat_region_size
                             + rde_region_size
                             + (clusternum - 2) * bs->num_sectors_per_cluster * SECTOR_SIZE;

    int offset = 0;
   // int num_sectors_to_read = (sizedatabuff + SECTOR_SIZE - 1) / SECTOR_SIZE;
	
    for( int i = 0; i < sizedatabuff; i += SECTOR_SIZE){
	    ata_lba_read(file_region_offset + offset, bbuff, 1);
	    for(int j = 0; j < SECTOR_SIZE; j++){
			    databuff[offset + j] = bbuff[j];
	    }
	    offset += SECTOR_SIZE;

	    
    for (int i = 0; i < num_sectors_to_read; i++) {
        ata_lba_read(file_region_offset + i * SECTOR_SIZE, bbuff, 1);
        bufcpy(databuff + offset, bbuff, SECTOR_SIZE);

        offset += SECTOR_SIZE;
	
    }
}








/*


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ide.h" // See this file for function prototype of ata_lba_read()
#include "fat.h"

#define SECTOR_SIZE 2048 //Sector size is 512, not 2048 
#define ATTR_VOLUME_ID 0x08

struct boot_sector *bs;
const char bootSector[512]; // Allocate a global array to store boot sector
char fat_table[8*SECTOR_SIZE];
unsigned int root_sector;

//implementing strncpy
void fat_strncpy(char* dest, const char* src, unsigned int n) {
    for(size_t i = 0; i < n; i++){
	dest[i]=src[i];
	if (src[i]=='\0'){
	    break;
	}
    }
}


int fatInit() {
     bs = (struct boot_sector*)bootSector; // Point boot_sector struct to the boot sector so we can read fields
     ata_lba_read(0, bootSector, 1); // Read sector 0 from disk drive into bootSector array
         
    // Print out some of the elements of the BIOS information block using rprintf...
//    rprintf("Bytes per sector: %d\n", bs->bytes_per_sector);
//    rprintf("Sectors per cluster: %d\n", bs->num_sectors_per_cluster);

 
    // TODO: Validate the boot signature = 0xaa55
    // check boot signature is valid
    // NAK Commenting out, this was not working for some reason
    if (bootSector[510] != 0x55 || bootSector[511] != 0xAA) { 
        return -1;  
    }

    // TODO: Validate fs_type = "FAT12" using strcmp
    // fs_type is valid
    char fs_type[9];
    // NAK you need to implmenet strncpy. It's part of the C library, but we're not linking with the C library. See if Chat GPT can help.
    fat_strncpy(fs_type, bs->fs_type, 8);
    strncpy(fs_type, bs->fs_type, 8);
    fs_type[8] = '\0';
    if(strcmp(fs_type, "FAT12") != 0){
        return -1; //fs_type isn't valid
    }

    // TODO: Read FAT table from the SD card into array fat_table
    unsigned int fat_start_sector = bs->num_reserved_sectors;
    unsigned int fat_size_sector = bs->num_sectors_per_fat * bs->num_fat_tables;
    for (int i = 0; i < fat_size_sector; i++) {
        ata_lba_read(fat_start_sector + i, fat_table + i*SECTOR_SIZE, 1);
    } 


    // TODO: Compute root_sector as:
   root_sector = bs->num_fat_tables * bs->num_sectors_per_fat + bs->num_reserved_sectors + bs->num_hidden_sectors;

   return 0;
}


int fatOpen(const char *filename, struct file* file) {

    char name[13];
    struct root_directory_entry rde;
    unsigned int i, j, cluster, sector, rd_size;

    //finding the root dir sixe in sectors
    rd_size = ((bs->num_root_dir_entries * sizeof(struct root_directory_entry)) + (bs->bytes_per_sector -1)) / bs->bytes_per_sector;
    //calculate first sector of data region
    unsigned int first_data_sector = bs->num_reserved_sectors + (bs->num_fat_tables * bs->num_sectors_per_fat) + rd_size;

    //search for the file's RDE in the root dir 
    for(i = 0; i < bs->num_root_dir_entries; i++) {
	//calculate sector and offset
	sector = root_sector + (i * sizeof(struct root_directory_entry)) / SECTOR_SIZE;
	unsigned int offset = (i * sizeof(struct root_directory_entry)) % SECTOR_SIZE;
	//read RDE into memory
	if(fatRead(sector, bootSector) != 0) {
	    return -1;
	}
	//copy file name into name array
	for(j = 0; j < 8;j++){
	    if(rde.file_name[j] == ' '){
		break;
	    }
	    name[j+1] = rde.file_name[j];
        }
	//copy file extension into name array 
	for(j = 0; j < 3;j++){
            if(rde.file_extension[j] == ' '){
                break;
            }
            name[j+9] = rde.file_name[j];
	}
	//set null term
	name[0] ='.';
	name[12] = '\0';
    
        //comparing file names  
        for(j = 0; j < 12; j++){
	    char f1 = name[j];
	    char f2 = filename[j];
	    
	    //convert char to uppercase
	    if(f1 >= 'a' && f1 <= 'z'){
		f1 -= 32;
	    }
	    if(f2 >= 'a' && f2 <= 'z'){
                f2 -= 32;
            }
	    if(f1 != f2) {
		break;
	    }
	    if (name[j] == '\0'){
	        //found files RDE
	        file->rde = rde;
	        file->start_cluster=rde.cluster;
	        return 0;
	    }
	}
    }
    return -1; //file was never found in root dir
}

        
int fatRead(int fd, void *buffer,int count) {
    //find the start sector of file 
    unsigned int sector = bs->num_reserved_sectors + (bs->num_fat_tables * bs->num_sectors_per_fat) + ((bs->num_root_dir_entries * 32) + (bs->bytes_per_sector - 1)) / bs->bytes_per_sector;
    unsigned int cluster = fd;
    while (cluster > 1 && cluster >= 0xFF8) {
	cluster = *((unsigned short *)(fat_table + cluster * 2));
	sector += bs->num_sectors_per_cluster;
    } 
    
    if (cluster < 2 || cluster >= 0xFF8) {
       return -1; //invalid cluster
    }

    //read data into buffer 
    int read_bytes= 0;
    while(read_bytes < count && cluster < 0xFF8) {
	ata_lba_read(sector, buffer + read_bytes, bs->bytes_per_sector);
	read_bytes += bs->bytes_per_sector;
	sector += bs->num_sectors_per_cluster;
	cluster = *((unsigned short*)(fat_table + cluster * 2));
    }
    return read_bytes;
}
*/
















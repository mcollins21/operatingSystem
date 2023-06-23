
#ifndef __IDE_H__
#define __IDE_H__

int ata_lba_read(unsigned int lba, unsigned char *buffer, unsigned int numsectors);

#endif

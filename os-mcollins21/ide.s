;       Reading the harddisk using ports!
;       +-------------------------------+   by qark
;
;
;  This took me months to get working but I finally managed it.
;
;  This code only works for the 286+ so you must detect for 8088's somewhere
;  in your code.
;
;  Technical Information on the ports:
;      Port    Read/Write   Misc
;     ------  ------------ -------------------------------------------------
;       1f0       r/w       data register, the bytes are written/read here
;       1f1       r         error register  (look these values up yourself)
;       1f2       r/w       sector count, how many sectors to read/write
;       1f3       r/w       sector number, the actual sector wanted
;       1f4       r/w       cylinder low, cylinders is 0-1024
;       1f5       r/w       cylinder high, this makes up the rest of the 1024
;       1f6       r/w       drive/head
;                              bit 7 = 1
;                              bit 6 = 0
;                              bit 5 = 1
;                              bit 4 = 0  drive 0 select
;                                    = 1  drive 1 select
;                              bit 3-0    head select bits
;       1f7       r         status register
;                              bit 7 = 1  controller is executing a command
;                              bit 6 = 1  drive is ready
;                              bit 5 = 1  write fault
;                              bit 4 = 1  seek complete
;                              bit 3 = 1  sector buffer requires servicing
;                              bit 2 = 1  disk data read corrected
;                              bit 1 = 1  index - set to 1 each revolution
;                              bit 0 = 1  previous command ended in an error
;       1f7       w         command register
;                            commands:
;                              50h format track
;                              20h read sectors with retry
;                              21h read sectors without retry
;                              22h read long with retry
;                              23h read long without retry
;                              30h write sectors with retry
;                              31h write sectors without retry
;                              32h write long with retry
;                              33h write long without retry
;
;  Most of these should work on even non-IDE hard disks.
;  This code is for reading, the code for writing is the next article.

;=============================================================================
; ATA read sectors (LBA mode) 
;
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
;
; @return None
;
; Lots of info from:
; https://wiki.osdev.org/ATA_PIO_Mode
;
; C Prototype:
; ata_lba_read(unsigned int lba, unsigned char *buffer, unsigned int numsectors)
;
; |-------------------------------|
; |      Num Sectors to Read      |
; |-------------------------------|
; |         Ptr to Buffer         |
; |-------------------------------|
; |          LBA To Read          |
; |-------------------------------|
; |         Return Address        |
; |-------------------------------|
; |          Caller's BP          |
; |-------------------------------|
;
;
;
;=============================================================================
    [BITS 32]
    global ata_lba_read
ata_lba_read:
    push ebp
    mov ebp,esp
    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov edx, 0x03F6      ; Digital output register
    mov al,2             ; Disable interrupts
    out dx,al

    mov eax,[8+ebp]      ; Get LBA in EAX
    mov edi,[12+ebp]     ; Get buffer in EDI
    mov ecx,[16+ebp]     ; Get sector count in ECX
    and eax, 0x0FFFFFFF
    mov ebx, eax         ; Save LBA in RBX

    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al

    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al

    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al
 

; ignore the error bit for the first 4 status reads -- ie. implement 400ns delay on ERR only
; wait for BSY clear and DRQ set
    mov ecx, 4
.lp1:
    in al, dx       ; grab a status byte
    test al, 0x80       ; BSY flag set?
    jne short .retry
    test al, 8      ; DRQ set?
    jne short .data_rdy
.retry:
    dec ecx
    jg short .lp1
; need to wait some more -- loop until BSY clears or ERR sets (error exit if ERR sets)
 
.pior_l:
    in al, dx       ; grab a status byte
    test al, 0x80       ; BSY flag set?
    jne short .pior_l   ; (all other flags are meaningless if BSY is set)
    test al, 0x21       ; ERR or DF set?
    jne short .fail
.data_rdy:
; if BSY and ERR are clear then DRQ must be set -- go and read the data
    mov edx, 0x1F0       ; Data port, in and out
    mov cx, 256
    rep insw        ; gulp one 512b sector into edi

    mov edx,0x1f7   ; "point" dx back at the status register
    in al, dx       ; delay 400ns to allow drive to set new values of BSY and DRQ
    in al, dx
    in al, dx
    in al, dx

; After each DRQ data block it is mandatory to either:
; receive and ack the IRQ -- or poll the status port all over again
 
;   inc ebp         ; increment the current absolute LBA
    dec long [16+ebp]           ; decrement the "sectors to read" count
    jne short .pior_l
 
    mov edx,0x1f0
; "test" sets the zero flag for a "success" return -- also clears the carry flag
    xor eax,eax
    test al, 0x21       ; test the last status ERR bits
    je short .done

.fail:
    mov eax,-1
 
.done:
    pop edi
    pop edx
    pop ecx
    pop ebx
    leave
    ret

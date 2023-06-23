# operatingsystem

Terminal Driver
  Printing Works
  Newline Works

Memory Management
  Page frame allocator returns physical addresses
  map_pages() creates virtual -> physical mappings

FAT Driver
  Can find files in the RDE listing of the root
  Can read data from files into memory

ELF Loader
  Parses ELF header
  Parses Program Headers
  Maps memory for ELF headers & copies data into place
  Jumps to ELF entry point

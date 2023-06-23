//CODE PROVIDED BY NEIL FOR THE PAGE.H FILE. THIS IS NECESSARY FOR THE PAGE.C FILE TO WORK

struct ppage {
	struct ppage *next;
	struct ppage **prev;
	void *physical_addr;
};

void init_pfa_list(void);

struct ppage *allocate_physical_pages(unsigned int npages);


void free_physical_pages_function(struct ppage *ppage_list);

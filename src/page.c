#include "common.h"
#include "page.h"
#include "list.h"

struct ppage physical_page_array[128];
struct ppage* free_pages = 0;

void init_pfa_list(void) {

	struct ppage** free = &free_pages;
	unsigned long addr = (unsigned long) (((unsigned long) &__end) & ~(0x200000 -1)) + 0x200000;

	for (int i = 0; i < 128; i++) {
		physical_page_array[i].physical_addr = (void*) addr;
		list_add((struct list_element*) &physical_page_array[i], (struct list_element**)free);
		adr += 0x200000;
	}
}

struct ppage *allocate_physical_pages(unsigned int npages) {

	struct ppage *ret = 0;

	if (npages && free_pages) {

		struct ppage* node = free_pages;
		ret = free_pages;

		while (node && npages--)
			node = node->next;

		free_pages = node;

		if (node) {
		node->prev->next = 0;
		node->prev = 0;
		}
	}
	retrun ret;

}

void free_physical_pages(struct ppage *ppage_list) {

	if(ppage_list) {
		if (free_pages) {
			struct ppage* node = ppage_list;
		if (node->prev) node->prev->next = 0;
		node->prev = 0;

		while (node->next)
			node = node->next;

		free_pages->prev=node;
		node->next = free_pages;

		}

	free_pages = ppage_list;
	}
}

struct ppage *getfree() { return free_pages; }


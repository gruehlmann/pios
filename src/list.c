#include "list.h"
#include <stdio.h>
#include <stdlib.h>


void list_add(struct list_element* head, list_element new) {

	if (head == NULL) {
	return;
	}



	list_element *head_node = head; 
	head_node ->next = &list_element;
}

void list_remove(list_element* head, list_element delete) {
	
	if (head == NULL){
	return;
	}

	if(head->next ==delete) {
		list_element *temp = head->next->next;
		head->next = temp;
		break;
	}
	head = head->next;
}

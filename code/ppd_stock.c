#include "ppd_stock.h"

/*** This function receives stock items line by line from previous function it, 
splits up the items and store them in their correct spot it the data structure. 
The function also validates the data***/

BOOLEAN items_handle(struct ppd_system *system, FILE * itemsfile){

  	char items[MAX_ITEM_LENGTH];
  	int item_lines = 1;
  
    /*reads the file line by line into buffer*/
    while(fgets(items,MAX_ITEM_LENGTH + TERM_CHARS , itemsfile) != NULL){
    	
        if(items[strlen(items) - CHAR] != '\n' ){
          /*Tests if the file uses more characters than allocted*/
          fprintf(stderr, "\nError: Line in file: '%s' too long. \n\n", 
          	system->stock_file_name);
          return FALSE;
        }
        /*If the file is invalid returns an error and exits*/
        if(loadItems(system, items, item_lines++) == FALSE){
          return FALSE;
        }
    }
  return TRUE;
}

BOOLEAN loadItems(struct ppd_system *system, char *items, int line_number){
    
    struct 	ppd_stock *temp_items;
	char *token, 
		 *tokens[MAX_ITEM_DELIM],
		 *endPtr,
		 *dollars,
		 *cents;	
	
	int tokenCount;
	BOOLEAN ADD = TRUE; 

	/*allocates memory to the temp node if fails returns error*/
	if((temp_items = malloc(sizeof(struct ppd_stock))) == NULL){

		fprintf(stderr, "Error: Unable to allocate memory. Aborting....\n" );
		return FALSE;
	}
	/*remove stock items and place into a holding array*/
	token = strtok(items, ITEM_DELIM);
	tokenCount = 0;
	
	while(token != NULL)
	{	/*if token exceeds 5, exit the program as file invalid*/
		tokenCount++;
		if(tokenCount > MAX_ITEM_DELIM)
		{
			break;
		}

		tokens[tokenCount - 1] = token;
		token = strtok(NULL, ITEM_DELIM);
	}
	/*if token exceeds 5, exit the program as file invalid*/
	if(tokenCount != MAX_ITEM_DELIM)
	{
		fprintf(stderr, "\nError: Invalid Line in File: '%s'\n\n", 
					system->stock_file_name);
		free(temp_items);
		return FALSE;
	}
	/*split the cost so that it can be validated and later added to list*/	
	dollars = strtok(tokens[3], PRICE_DELIM);
	cents = strtok(NULL, PRICE_DELIM);

	/*validate the stock items except the money component*/
	if(validate_stock_file(system, tokens, line_number) == FALSE){

		free(temp_items);
		return FALSE;
	}
	/*validates the money component of stock file*/
	if(validate_money(system, dollars, cents, line_number) == FALSE){

		free(temp_items);
		return FALSE;
	}
	/*copy stock data into temporary structure*/
	strcpy(temp_items->id, tokens[0]);
	strcpy(temp_items->name, tokens[1]);
	strcpy(temp_items->desc, tokens[2]);
	temp_items->price.dollars = strtol(dollars, &endPtr, 10);
	temp_items->price.cents = strtol(cents, &endPtr, 10);
	temp_items->on_hand = strtol(tokens[4], &endPtr, 10);
		
	/*adds items to the linked list*/
	ADD = add_to_list(system, temp_items);
	/*if adding fails frees the memory and exits the program*/
	if(ADD == FALSE){
	
		free(temp_items);
		return FALSE;
	}
	return TRUE;
}
/*linked list from RMIT samples/tutorials, with help from Matthew Bulger*/
BOOLEAN add_to_list(struct ppd_system *system, struct ppd_stock *items ){

	struct ppd_node *new;
	struct ppd_node * previous, * current;
	assert(system);
	/*allocate memory to new node*/
	if((new = malloc(sizeof(struct ppd_node))) == NULL){

		fprintf(stderr, "Erorr: Memory allocation failed!!\n" );
		return FALSE;
	}
	/*sets item to new node*/
	new->data = items;
	new->next = NULL;
	/*assign previous to NULL and current to the head of the list*/
	previous = NULL;
	current = system->item_list->head;
	/*iterate through list so we can insert into correct posiion*/
	while(current != NULL)
	{
		if(strcmp(new->data->name, current->data->name) < 0)
		{
			break;
		}

		previous = current;
		current = current->next;
	}

	if(previous == NULL)
	{
		/* Insert at head */
		new->next = system->item_list->head;
		system->item_list->head = new;
	}
	else 
	{
		previous->next = new;
		new->next = current;
	}

	++system->item_list->count;
	
	return TRUE;
}


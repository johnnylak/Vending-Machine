#include "ppd_options.h"

/**
 * @file ppd_options.c this is where you need to implement the main 
 * options for your program. You may however have the actual work done
 * in functions defined elsewhere. 
 * @note if there is an error you should handle it within the function
 * and not simply return FALSE unless it is a fatal error for the 
 * task at hand. You want people to use your software, afterall, and
 * badly behaving software doesn't get used.
 **/

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true as this particular function should never fail.
 **/
BOOLEAN display_items(struct ppd_system * system)
{   
    struct ppd_node *current;
    assert(system);
    /*Sets current to the head of the linked list*/
    current = system->item_list->head;
    /*If there are no items in machine display message*/
    if(system->item_list->count == EMPTY)
    {
        printf("Sorry no items available\n");
        return FALSE;
    }

    printf("\nItems menu\n");
    printf("-----------\n");
    printf("%-*s| %-*s | %-8s | %s\n", IDLEN, "ID", NAMELEN, "NAME",
                                             "AVAILABLE","PRICE");
    printf("-------------------------------------------------------------------"
                                                                        "--\n");
    /*Itterates through the linked list and prints all items in it*/
    while(current != NULL){
        printf("%*s| %-*s | %-8d  | $%d.%.2d\n", IDLEN, current->data->id, 
            NAMELEN, current->data->name, current->data->on_hand, 
            current->data->price.dollars, current->data->price.cents);
        /*sets current to the next node in the list*/   
        current = current->next;
    }
    return TRUE;
}
/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true when a purchase succeeds and false when it does not
 **/
BOOLEAN purchase_item(struct ppd_system * system)
{
    char itemID[IDLEN];
    struct ppd_node *purchaseItem;
    int owingAmount;
    
    printf("Purchase Item\n");
    printf("--------------\n");

    printf("Please enter the ID of the item you wish to purchase: \n");
    printf("Pressing enter or CTRL-D at any time will return you to the"
                                                                "menu!\n");
    /*requests user input*/
    if(getString(itemID, IDLEN) == RTM){

        printf("Purchase cancelled, returning to menu...\n");
        return FALSE;
    }
    /*checks whether the item is in the vending machine*/
    purchaseItem = search_Nodes(system, itemID);
    /*if the item is not in the machine return to the menu*/
    if(purchaseItem == NULL){

        fprintf(stderr,"Error: This product does not exist in the system:"
                                                                " %s\n",itemID);
        printf("Returning to Menu!\n");
        return FALSE;
    }
    /*if the product is avail to purchase request money and confirm product*/
    if((int) purchaseItem->data->on_hand > 0){

        printf("You have selected \"%s - %s. This will cost you $%d.%.2d\n\"", 
                                    purchaseItem->data->name, 
                                        purchaseItem->data->desc, 
                                            purchaseItem->data->price.dollars,
                                                purchaseItem->data->price.cents);

        printf("Please hand over the money - type in the value of each note/coin" 
                                                                " in cents.\n");
    }else{

        printf("Temporarily out of stock.\n");
        printf("Returning to Menu!\n");
        return FALSE;
    }
    /*Calulates the cost of the purchase into cents*/
    owingAmount = convert_Cents(purchaseItem->data->price.dollars,
                                            purchaseItem->data->price.cents);
    /*passing the prodcut and the amount owing to the cash register for 
                                                                calculation*/
    if(cash_register(system, owingAmount,purchaseItem->data->name) == FALSE){

        return FALSE;
    }
    /*If purchase successful reduced the amount of the product*/
    purchaseItem->data->on_hand--;
    return TRUE;;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true when a save succeeds and false when it does not
 **/
BOOLEAN save_system(struct ppd_system * system)
{
    FILE *stock_file, *coin_file;
    struct ppd_node *current;
    int i;
    /*Sets current to the head of the linked list*/   
    current = system->item_list->head;
    /*if there is a problem openin the file prints eror and exits*/
    if((stock_file = fopen(system->stock_file_name, "w")) == NULL){

        fprintf(stderr, "Error: File: %s could not be opened!\n", 
                                                    system->stock_file_name );
        
        return FALSE;
    }
    /*goes through each node and prints it to the file*/
    while(current != NULL){

        fprintf(stock_file, "%s|%s|%s|%d.%.2d|%d\n", 
            current->data->id,current->data->name, current->data->desc, 
                current->data->price.dollars, current->data->price.cents,
                    current->data->on_hand  );
        /*moves current to the next node in the list*/
        current = current->next;
    }
     /*if there is a problem openin the file prints eror and exits*/
    if((coin_file = fopen(system->coin_file_name, "w")) == NULL){

        fprintf(stderr, "File: %s could not be opened!\n", 
                                                    system->coin_file_name );

        fclose(stock_file);
        return FALSE;
    }
    /*goes through the denom array and writes to file*/
    for(i = NUM_DENOMS-1; i >= 0; i--){
        
        fprintf(coin_file,"%d,%d\n",system->cash_register[i].denom,
                                        system->cash_register[i].count );
    }
    /*closes the file then exits*/
    fclose(stock_file);
    fclose(coin_file);
    
    return FALSE;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true when adding an item succeeds and false when it does not
 **/
BOOLEAN add_item(struct ppd_system * system)
{
    struct ppd_stock *temp_items;
    struct ppd_node *node_Search;
    
    int number_of_items = NONE, 
        add_Item = 1;

    char *endPtr, 
        *dollars, 
        *cents,
        itemName[NAMELEN], 
        new_ID[IDLEN], 
        itemDescription[DESCLEN], 
        itemPrice[COST_LEN];
    
    if((temp_items = malloc(sizeof(struct ppd_stock))) == NULL){

        fprintf(stderr, "Error: Memory allocation failed!!\n" );

        return FALSE;
    } 
    /*Allocates new ID if available*/
    do{ 
        /*item count starts at 0 and adds 1 to it*/
        number_of_items += add_Item;
        /*Number of items in machine cannot be greater than 30*/   
        if(number_of_items <= MAX_ITEMS){
            sprintf(new_ID, "I%.4d", number_of_items);
        }/*skippiungv every two ids*/
        else if(number_of_items > MAX_ITEMS){
    
            fprintf(stderr, "Error: System unable to cater for more than 30" 
                                                                " items:\n");
            return FALSE;
        }
        /*checks if ID taken, if taken, goes back to top and starts again
        add 1 to the count and ID*/
        node_Search = search_Nodes(system, new_ID);
        
    }while(node_Search != NULL);

    printf("Pressing enter or CTRL-D will exit without saving!\n");
    printf("This new meal will have the Item if of: %s\n", new_ID);
    /*Requests user input, if fails frees temp and returns to menu*/
    printf("Enter the item name:\n");
    if(getString(itemName, NAMELEN) == RTM){
        
        free(temp_items);
        printf("No items added, returning to menu...");
        
        return FALSE;
    }
    /*Requests user input, if fails frees temp and returns to menu*/
    printf("Enter the item description:\n");
    if(getString(itemDescription, DESCLEN) == RTM){
        
        free(temp_items);
        printf("No items added, returning to menu...");
        
        return FALSE;
    }
    /*Requests user input, if fails frees temp and returns to menu*/
    do{
        printf("Enter the price for this item: e.g: $8 -> 8.00\n");
        if(getString(itemPrice, COST_LEN) == RTM){
            
            free(temp_items);
            printf("No items added, returning to menu...");
            
            return FALSE;
        }
        /*split the price from the dollars and cents components.*/
        dollars = strtok(itemPrice, MONEY_DELIM);
        cents = strtok(NULL, MONEY_DELIM);
    
    }while(validate_money(system, dollars, cents, LINE_NO) != TRUE);
    /*Copies all data to the temp node*/
    strcpy(temp_items->id, new_ID);
    strcpy(temp_items->name, itemName);
    strcpy(temp_items->desc, itemDescription);
    temp_items->price.dollars = strtol(dollars, &endPtr, 10);
    temp_items->price.cents = strtol(cents, &endPtr, 10);
    temp_items->on_hand = DEFAULT_STOCK_LEVEL;
    /*Adds item to the system then returns to menu*/
    if(add_to_list(system, temp_items) == TRUE){
        printf("This item %s - %s. Has now been added to the system\n", 
                                                    itemName, itemDescription);
    }
    else{
        fprintf(stderr, "Error: Unable to add item.\n" );
        free(temp_items);
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true when removing an item succeeds and false when it does not
 **/
BOOLEAN remove_item(struct ppd_system * system)
{
    struct ppd_node *current, *DELETE_node;
    char itemID[IDLEN];
    /*Sets current to the begining of the list*/
    current = system->item_list->head;
    printf("Pressing enter or CTRL-D will exit without saving!\n");
    printf("Enter the item if of the item to remove from the menu:\n");
    /*requests user input if fails returns to menu*/
    if(getString(itemID, IDLEN) == RTM){

        printf("No items removed, returning to menu...\n");
        return FALSE;
    }
    /*search for the item in the vending machine*/
    DELETE_node = search_Nodes(system,itemID);
    /*if the product isn't in the machine, advise and refturn to menu*/
    if(!DELETE_node){

        printf("Item %s does not exist. Returning to menu.\n", itemID);
        
        return FALSE;
    }
    /*if the product is at the begining of the list remove it*/
    if(DELETE_node == system->item_list->head){
            
        system->item_list->head = current->next;
    }
    /*if the product is anywhere other then at the begining of the list remove*/
    else{ 
        while(current->next != NULL){
            
            if(current->next == DELETE_node){
                current->next = DELETE_node->next;
                break;
            }
            /*set current to the next item*/
            current = current->next;
        }
    }
    /*reduce the stock count by 1*/   
    --system->item_list->count;
    printf("\"%s - %s - %s\" has been removed from the system\n", 
                                DELETE_node->data->id, DELETE_node->data->name, 
                                                    DELETE_node->data->desc );
    /*free the deleted node*/
    free(DELETE_node->data);
    free(DELETE_node); 
    DELETE_node = NULL;
    return TRUE;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true as this function cannot fail.
 **/
BOOLEAN reset_stock(struct ppd_system * system)
{
    struct ppd_node *current;
    /*set current to the begining of the list*/
    current = system->item_list->head;
    /*itterate through the stock and reset values to default level*/
    while(current){

        current->data->on_hand = DEFAULT_STOCK_LEVEL;

        current = current->next;
    }

    return TRUE;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true as this function cannot fail.
 **/
BOOLEAN reset_coins(struct ppd_system * system)
{
    int i;
    /*sets the coin count to default level*/
    for(i = 0; i < NUM_DENOMS; i++){

        system->cash_register[i].count = DEFAULT_COIN_COUNT;
    }
    
    printf("All coins have been reset to the default amount of 20\n");

    return TRUE;
}

/**
 * @param system a pointer to a  ppd_system struct that contains 
 * all the information for managing the system.
 * @return true as this function cannot fail
 **/
BOOLEAN display_coins(struct ppd_system * system)
{
    int i; 
    char *coins[NUM_DENOMS] = {"5 Cents ", "10 Cents", "20 Cents", 
            "50 Cents", "1 Dollar", "2 Dollars", "5 Dollars", "10 Dollars"};
   
       printf("\nCoin Summary\n");
       printf("-----------------\n");
       printf("Denomination    | Count\n");
       printf("------------------------\n");
        /*iterates through the coin array displaying denom & count*/
        for(i = NUM_DENOMS - 1; i >= 0; i--)
        {
            printf("%s\t| %d\n", coins[i],system->cash_register[i].count);        
             
        } 

    return FALSE;
}


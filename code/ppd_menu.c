#include "ppd_menu.h"
/**
 * @file ppd_menu.c handles the initialised and management of the menu 
 * array
 **/

/**
 * @param menu the menu item array to initialise
 **/
void init_menu( struct menu_item* menu)
{	/*array of function names*/
  	char name[AMT_MENU_ITEMS][MENU_NAME_LEN]= {"Display Items", "Purchase Item", 
  					"Save and Exit","Add Item", "Remove Item", "Display Coins", 
  						"Reset Stock", "Reset Coins", "Abort Program"};
    /*array of function pointers*/ 
    BOOLEAN (*function[])(struct ppd_system*) = {display_items,purchase_item,
    						save_system,add_item,remove_item, display_coins,
    											reset_stock,reset_coins}; 
  	int i;
   	/*inits the menu_item datastructure*/
    for (i = 0; i < AMT_MENU_ITEMS; i++)
    {  	/*copies data into function struct*/
        strcpy(menu[i].name, name[i]);
        menu[i].function = function[i];
    }
}
 /*Displays the menu when called*/
BOOLEAN print_menu(struct menu_item* menu){

    int i;

    printf("\nMain Menu:\n");
    printf("----------------------------\n");
    for (i = 0; i < AMT_MENU_ITEMS; i++)
    {   /*prints out the menu items*/
        if(i != ADMIN_MENU_OPTION_3){
          printf("\t%d) %s\n", i+1, menu[i].name);
        }
        else
        {
          printf("\nAdministrative Menu:\n\n");
          printf("\t4) %s\n", menu[ADD_ITEM].name);
        }
    }
    return TRUE; 
 }

/*displays the menu and asks for input*/
BOOLEAN display_menu(struct ppd_system *system, struct menu_item* items ){

    int option;
    char *prompt = "\nSelect your option (1 - 9): ";
    
    print_menu(items);
    
    do{
        /*test if user enters an iput if not keeps asking otherwise
        calls function*/
        if(getInteger(&option,OPTION_LENGTH,prompt,1,9) == RTM)
        {
             continue;
        } 
        
        if (option == 9)
        {   /*quits program and frees memory*/
            return TRUE;
        }
        else if (option == 3){
            /*calls function then quits program*/
            items[option-1].function(system); 
            return TRUE;
        }
        
        items[option-1].function(system); 
        print_menu(items);

    }while(option);
    
    return TRUE;
}
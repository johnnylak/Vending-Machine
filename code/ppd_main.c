#include "ppd_main.h"
#include "ppd_menu.h"
#include "ppd_options.h"
#include "ppd_utility.h"

/**
 * @file ppd_main.c contains the main function implementation and any 
 * helper functions for main such as a display_usage() function.
 **/

/**
 * manages the running of the program, initialises data structures, loads
 * data and handles the processing of options. The bulk of this function
 * should simply be calling other functions to get the job done.
 **/
int main(int argc, char **argv)
{  
    /* represents the data structures to manage the system */
    struct ppd_system system;
    /* represents menu_items to pass to init_menu*/
    struct menu_item items[AMT_MENU_ITEMS];
    /* validate command line arguments */
    if(argc != 3){

        fprintf(stderr, "Error: You have not entered the correct amount of" 
                                                            " arguments!\n\n");                                                     
        fprintf(stderr, "Please enter arguments as follows:\n");
        fprintf(stderr, "./[program_name] <itemsfile> <coinsfile>\n\n");

        return EXIT_FAILURE;
    }
    /*Save the given arguments to ppd_system cars*/
    system.stock_file_name = argv[1];
    system.coin_file_name = argv[2];

    /* init the system */
    if(system_init(&system) == FALSE){

        return EXIT_FAILURE;
    }

    /* load data */
    if(load_data(&system, system.coin_file_name, 
                                system.stock_file_name) == FALSE){
    /*Frees memory if any used in the case that coins file corrupted and items
            file has been implemented*/
        
        system_free(&system);
        return EXIT_FAILURE;
    }
    /* initialise the menu system */
    init_menu(items);

    /*displays the menu items as selectable options*/
    if(display_menu(&system, items) == TRUE){
        system_free(&system);
        return EXIT_SUCCESS;
    }
    
    return EXIT_SUCCESS;
}

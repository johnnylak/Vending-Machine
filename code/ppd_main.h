#include <stdio.h>
#include <stdlib.h>
#include "ppd_stock.h"
#include "ppd_coin.h"
#ifndef PPD_MAIN
#define PPD_MAIN

/**
 * @file ppd_main.h this file holds the rest of the application together.
 * From these header files, main() can call all functions refered to 
 * in the header files included. You might also want to insert here
 * any functions for handling and validation of command line arguments
 **/

/**
 * datatype to represent a boolean value within the system
 **/
typedef enum truefalse
{
    /**
     * the constant for false
     **/
    FALSE, 
    /**
     * the constant for true
     **/
    TRUE
} BOOLEAN;


#endif

/**
 * @file ppd_coin.h defines the coin structure for managing currency in the
 * system. You should declare function prototypes for managing coins here
 * and implement them in ppd_coin.c
 **/
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include "ppd_stock.h"

#ifndef PPD_COIN
#define PPD_COIN
#include "ppd_main.h"

#define COIN_DELIM ","
/**
 * The number of denominations of currency available in the system 
 **/
#define NUM_DENOMS 8
#define MAX_MONEY_INPUT 1000
#define MIN_MONEY_INPUT 5
#define LENGTH_MONEY 4
#define TERM_CHARS 2
#define ONE_HUNDRED_CENTS 100 
#define EXTRA_COIN_SLOT 2
#define MAX_MONEY_HOLDER 10
#define MAX_MONEY_TENDERED 10
#define DELETE 0
#define SAVE_DELETE 1
#define MAX_COIN_LEN 7

struct ppd_system;
/**
 * enumeration representing the various types of currency available in
 * the system. 
 **/

enum denomination
{
    FIVE_CENTS, TEN_CENTS, TWENTY_CENTS, FIFTY_CENTS, ONE_DOLLAR, 
    TWO_DOLLARS, FIVE_DOLLARS, TEN_DOLLARS
};

struct money{

    int money_In[MAX_MONEY_TENDERED];
};
/**
 * represents a coin type stored in the cash register. Each demonination
 * will have exactly one of these in the cash register.
 **/
struct coin
{
    /**
     * the denomination type
     **/
    enum denomination denom;
    /**
     * the count of how many of these are in the cash register
     **/
    unsigned count;
};
BOOLEAN coins_handle(struct ppd_system *, FILE *);
BOOLEAN loadCoins(struct ppd_system*, char*, int);
int convert_Cents(int, int);
float convert_Dollars(int);
BOOLEAN test_valid_monies(struct ppd_system*, int);
BOOLEAN cash_register(struct ppd_system*, int, char*);
BOOLEAN calculate_Change(struct ppd_system*, int, char*);
int available_money(struct ppd_system*);
BOOLEAN bank_money(struct ppd_system*, BOOLEAN, struct money*, int);

#endif

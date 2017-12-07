#include "ppd_coin.h"

 /**
  * @file ppd_coin.c implement functions here for managing coins in the
  * "cash register" contained in the @ref ppd_system struct.
  **/
/*Function reads in coin file*/
BOOLEAN coins_handle(struct ppd_system *system, FILE * coinsfile){

  	char coins[MAX_COIN_LEN];
  	int coins_lines = 1;

   	/*reads the file line by line into buffer*/
    while(fgets(coins,MAX_COIN_LEN + TERM_CHARS,coinsfile) != NULL){
        /*Tests if the file uses more characters than allocted*/
        if(coins[strlen(coins) - CHAR] != '\n' ){

          fprintf(stderr, "\nError: Line in file: '%s' too long. \n\n", 
          	system->coin_file_name);
          return FALSE;
        }
        /*calls load coins to load into array an validate*/
        if(loadCoins(system, coins, coins_lines++) == FALSE){
          return FALSE;
        } 
    } 
    /*checks if the correct amount of denoms are in file*/
    if(coins_lines < NUM_DENOMS + CHAR){

      fprintf(stderr, "\nError: Not enough denominations in file: %s\n\n", 
                system->coin_file_name);
      return FALSE;
    }
    /*Checks if the coin has already been added into the system*/
    if(validate_duplicate_coin(system) == FALSE){
       return FALSE; 
    }
    /*this function sorts the coin array into correct order*/
    sort_coins(system);
    
  return TRUE;
}
/*Splits up the coin file, validates and adds to system*/ 
BOOLEAN loadCoins(struct ppd_system *system, char * coins, int line_counter)
{
	char *token, *endPtr, *tokens[2];
	int tokenCount;
	/*tokenises coins line by line*/ 
	token = strtok(coins,COIN_DELIM);
	tokenCount = 0;

	while(token != NULL){
		
		tokenCount++;
		if(tokenCount > 2)
		{
			break;
		}
		/*add's coims to array*/
		tokens[tokenCount - 1] = token;
		token = strtok(NULL, COIN_DELIM);
		
	}
	/*If there arent two tokens or the coin file has exceeded 
		its allowed number on denominations exits program*/
	if((tokenCount != 2) || (line_counter > NUM_DENOMS)) {

		fprintf(stderr, "\nError: Invalid Line in File: '%s' on line: %d\n\n", 
					system->stock_file_name, line_counter);
		return FALSE;
	}	
	/*If validation unsuccessful exits program*/
	if(validate_coins(system, tokens, line_counter) == FALSE){

		return FALSE;
	}
	/*Add the coins to the coin array once validated*/
	system->cash_register[NUM_DENOMS - line_counter].denom = strtol(tokens[0], 
																	&endPtr, 10);
	system->cash_register[NUM_DENOMS - line_counter].count = strtol(tokens[1], 
																	&endPtr, 10);
	return TRUE;
}
/*Simple function to convert dolarrs to cents*/
int convert_Cents(int x, int y){

	return (x*100)+y;
}
/*simple function to convert cents to dollars*/
float convert_Dollars(int x){
	
	return((float)x/100);
}
/*Function test in money is valid currency*/
BOOLEAN test_valid_monies(struct ppd_system *system, int money){

  int i;
  
  for(i = 0; i < NUM_DENOMS; i++){
      /*If money found in coin array returns true to calling function*/
      if(system->cash_register[i].denom == money){
        return TRUE;
      }
  }
  return FALSE;
}
/*Function handles tendered coins*/ 
BOOLEAN cash_register(struct ppd_system *system, int owingAmount, 
															char * item_Name){
	struct money money_Tendered; 
	int money, 
		sale_Finished = FALSE, 
		coin_Tendered = 0; 
	char *cancelTransaction = "Press enter or ctrl-D on a new line to" 
												" cancel this transaction:\n";

	do{
		/*check if less then 10 coins entered into vending machine*/
        if (coin_Tendered >= MAX_MONEY_TENDERED)
        {	
        	fprintf(stderr, "Sorry Vending Machine unable to take more then" 
        													" 10 coins.\n" );
        	printf("Transaction Cancelled. Returning to menu.\n");
        	/*Releases the money from the holding array*/ 
        	bank_money(system, DELETE, &money_Tendered,coin_Tendered);
        	return FALSE;
        }
        /*Requests user input if RTM returned, releases money and exits*/
        if(getInteger(&money, LENGTH_MONEY, cancelTransaction, MIN_MONEY_INPUT, 
        											MAX_MONEY_INPUT) == RTM){
        	bank_money(system, DELETE, &money_Tendered, coin_Tendered);
        	printf("Purchase cancelled, returning to menu...\n");
        	return FALSE;
        }/*if currency is not valid lets the user know and asks for reinput*/
        if(!test_valid_monies(system, money)){

            printf("Not valid currency, please try again!\n");
            continue;
        }
        else{
        	/*if currency valid reduces it from the owing amount*/ 
        	owingAmount -= money;
        	/*if amount owing is less then 0 test if sale is finished*/
        	if(owingAmount < 0){
        		/*send the owed change to the cash dispenser*/ 
        		sale_Finished = calculate_Change(system, owingAmount, item_Name);
        		/*if sale cannot be processed return money and exit*/
        		if(sale_Finished== FALSE){
        			bank_money(system, DELETE,&money_Tendered,coin_Tendered);
        			printf("Unable to process sale, returning to menu.\n");
        			return FALSE;
        		}
        	}else{
        		printf("You still need to give us $%.2f\n",
        										convert_Dollars(owingAmount));
        	}
        }
        /*if sale has finished bank the money into the system*/
        if(sale_Finished == TRUE){
        	money_Tendered.money_In[coin_Tendered] = money;
        	bank_money(system, SAVE_DELETE, &money_Tendered,coin_Tendered);
        	return TRUE;
        }
        /*add money tendered into the coin deposited array*/
        money_Tendered.money_In[coin_Tendered] = money;
        /*keep track of how many coins deposited*/
        coin_Tendered++; 
    }while(owingAmount != 0);
    
    /*Add the given currency into coin bank*/
    bank_money(system, SAVE_DELETE, &money_Tendered,coin_Tendered);
   
    printf("Thank you. Here is your %s, there is no change.",item_Name);

return TRUE;
}
/*function calculates change then dispenses the change*/
BOOLEAN calculate_Change(struct ppd_system *system, int change, char * item_Name){

	int counter = NUM_DENOMS-1, 
		i = 0, 
		*money, 
		*temp, 
		coin_Count = 0, 
		positive_Change = change*-1, /*converts change to positive*/
		moneyArray = MAX_MONEY_HOLDER;
	/*allocated memory to store change*/
	if((money = malloc(sizeof(int)*moneyArray)) == NULL){

		fprintf(stderr, "Unable to allocate memory, Aborting...\n");
		return FALSE;
	}
	/*Sets the array positions to 0*/
	memset(money, 0, sizeof(int)*moneyArray);
	
	/*Checks if there is enough change to give to customer*/
	if((available_money(system) - positive_Change) < 0){
		fprintf(stderr, "Our appologies, the machine hasn't enough change.\n");
		return FALSE;
		}

	do{
		/*if change is availble in the system add add to money array*/ 
		if((system->cash_register[counter].denom <= positive_Change)&&
			(system->cash_register[counter].count > 0)){
			/*add the denom to holding array*/
			money[coin_Count] = system->cash_register[counter].denom;
			/*reduce the amount owing by dispensed denom*/
			positive_Change = positive_Change - system->cash_register[counter].denom;
			/*minus currency count*/
			--system->cash_register[counter].count;
			counter = 0;
			/*Allocate more memory as needed*/ 
			temp = realloc(money, (coin_Count + EXTRA_COIN_SLOT)*sizeof(int));
			/*If allocation approved assign it to money*/
			if(temp != NULL){
				money = temp;
			}
			else{
				/*Allocation fails free memory and exit program*/
				fprintf(stderr, "Unable to allocate memory, Aborting...\n");
				free(money);
				return FALSE;
			}
			/*increase coin count by 1 for next array position*/
			coin_Count++;
		}/*if the counter is set to 0, start at the top of the denom array*/
		if(counter == 0)
		{
			counter = NUM_DENOMS;
		}
		else{
			counter--;
		}
		
	}while(positive_Change != 0);
	/*the following dispenses what is being held in the money array*/
	printf("Thank you. Here is your %s, and your change of $%.2f:", item_Name, 
													convert_Dollars(change*-1));
	for (i = 0; i < coin_Count; i++ ){

	 	if(money[i] >= ONE_HUNDRED_CENTS){
	 		printf(" $%d ",(int) convert_Dollars(money[i]));
	 	}else{
	 		printf(" %dC ",money[i]);
	 	}
	}
	printf("\n");
	free(money);
	
	return TRUE;
}
/*function checks that there is enough money in cash hold*/
int available_money(struct ppd_system *system){

	int i;
	float total_money = 0;
	
	for(i = 0; i < NUM_DENOMS; i++){
		/*calculates the total money in the coins array*/
		total_money += (system->cash_register[i].count * 
							convert_Dollars(system->cash_register[i].denom));
	}
	/*converts the money to cents*/
	return convert_Cents(total_money, 0);
}
/*Function banks tendered money and zeros out tem coin array*/
BOOLEAN bank_money(struct ppd_system *system, BOOLEAN add_delete, 
							struct money *money_Tendered, int coins_Tendered){
	
	int i,j;
	if(add_delete == DELETE){
		/*Resets money tendered array for next transaction*/
		memset(money_Tendered, 0, sizeof(int)*MAX_MONEY_TENDERED);
		return FALSE;
	}
	else if(add_delete == SAVE_DELETE){

		for(i = 0; i <= NUM_DENOMS; i++){
			for(j = 0; j <= coins_Tendered; j++){
				/*Goes through array of coins if it finds increments count*/
				if(system->cash_register[i].denom == money_Tendered->money_In[j]){
					system->cash_register[i].count++;
				}
			}
		}
	}/*Resets money tendered array for next transaction*/
	memset(money_Tendered, 0, sizeof(int)*MAX_MONEY_TENDERED);
	return TRUE;
}
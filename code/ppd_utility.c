#include "ppd_utility.h"
/**
 * @file ppd_utility.c contains implementations of important functions for
 * the system. If you are not sure where to implement a new function, 
 * here is probably a good spot.
 **/

void read_rest_of_line(void)
{
    int ch;
    /* keep retrieving characters from stdin until we
     * are at the end of the buffer
     */
    while(ch = getc(stdin), ch!='\n' && ch != EOF)
        ;
    /* reset error flags on stdin */
    clearerr(stdin);
}

/**
 * @param system a pointer to a @ref ppd_system struct that contains
 * all the data in the system we are manipulating
 * @param coins_name the name of the coins file
 * @param stock name the name of the stock file
 **/
BOOLEAN load_data(struct ppd_system * system , const char * coins_name, 
const char * stock_name)
{
    FILE *itemsfile, *coinsfile;
  
    /*opens the file for reading*/
    if((coinsfile = fopen(coins_name, "r")) == NULL){
        fprintf(stderr, "Error: File: %s could not be opened!\n", coins_name );
        return FALSE;
    }
     /*opens the file for reading*/
    if((itemsfile = fopen(stock_name, "r")) == NULL){
        fprintf(stderr, "Error: File: %s could not be opened!\n", stock_name );
        
        fclose(coinsfile);
        return FALSE;
    }
    /*checks if the file is empty*/
    if(is_valid_file(system, coinsfile, itemsfile) == FALSE){
      
        fclose(itemsfile);
        fclose(coinsfile);

        return FALSE;
    }
    /*sends the files to be added to the system*/
    if((coins_handle(system, coinsfile) == FALSE) || 
            (items_handle(system, itemsfile)== FALSE)){
      
        fclose(itemsfile);
        fclose(coinsfile);

        return FALSE;
    }
   
    fclose(itemsfile);
    fclose(coinsfile);

    return TRUE;
}

/*Tests if the file is empty*/
BOOLEAN is_valid_file(struct ppd_system *system, FILE * coins_file, 
                                                          FILE * stock_file){
    /*starts at the begining of the file*/
    fseek(coins_file,0,SEEK_END);
    fseek(stock_file,0,SEEK_END);
    /*returns error if file is empty*/
    if(ftell(coins_file)==0){
      
      fprintf(stderr, "Error: File: %s is empty!\n", system->coin_file_name );
      return FALSE;
    }
    if(ftell(stock_file)==0){
      
      fprintf(stderr, "Error: File: %s is empty!\n", system->stock_file_name );
      return FALSE;
    }/*sets fseek back to the begining*/
    fseek(coins_file,0,SEEK_SET);
    fseek(stock_file,0,SEEK_SET);

  return TRUE;
}
/**
 * @param system a pointer to a @ref ppd_system struct that holds all 
 * the data for the system we are creating
 **/
BOOLEAN system_init(struct ppd_system * system)
{
    assert(system);
    /*malloc space for the itenm list*/
    if((system->item_list = malloc(sizeof( *system->item_list))) == NULL){

        fprintf(stderr, "Error: Memory allocation failed.\n" );
        return FALSE;
    }
    /*set the head to null and count to 0*/
    system->item_list->head = NULL;
    system->item_list->count = 0; 
    
    return TRUE;
}

/*Function written by Steven Burrows*/
int getInteger(int* integer, unsigned LENGTH_INT, char* prompt, int min, int max)
{
   
   int finished = FALSE;
   char tempString[OPTION_LENGTH + TERM_CHARS];
   int tempInteger = 0;
   char* endPtr;

   /* Continue to interact with the user until the input is valid. */
   do
   {
      /* Provide a custom prompt. */
      printf("%s", prompt);
      
      /* Accept input. "+2" is for the \n and \0 characters. 
         tests if the user has entered ctrl-D to return to menu*/
      if(fgets(tempString, LENGTH_INT + TERM_CHARS, stdin) == NULL){
         
         return RTM;
      }
      else{
         /*tests if the user has hit enter to return to menu*/
         if(tempString[0] == '\n'){
           
            return RTM;
         }
        
         /* A string that doesn't have a newline character is too long. */
         if (tempString[strlen(tempString) - 1] != '\n')
         {
            printf("Input was too long. Please try again\n");
            read_rest_of_line();
         }
         else
         {
            /* Overwrite the \n character with \0. */
            tempString[strlen(tempString) - 1] = '\0';

            /* Convert string to an integer. */
            tempInteger = (int) strtol(tempString, &endPtr, 10);

            /* Validate integer result. */
            if (strcmp(endPtr, "") != 0)
            {
               printf("Input was not numeric.\n");
            }
            else if (tempInteger < min || tempInteger > max)
            {
               printf("Input was not within range %d - %d.\n", min, max);
            }
            else
            {
               finished = TRUE;
            }
         }
      }
   } while (finished == FALSE);

   /* Make the result integer available to calling function. */
   *integer = tempInteger;
   
   return TRUE;
}
/*Function written by Steven Burrows*/
int getString(char* string, unsigned LENGTH)
{
   int finished = FALSE;
 
   char *tempString = NULL;
   /*Alloctae memory because we have a variable length array 
   that needs to be changed at runtime*/
   tempString = malloc((LENGTH + TERM_CHARS)*((int) sizeof(char))
               + TERM_CHARS);

   if(tempString == NULL){

      fprintf(stderr, "Error: Memory allocation failed!!\n" );
      
      exit(EXIT_FAILURE);
   }
    /*Continue to interact with the user until the input is valid. */
   do
   {        
      /* Accept input. "+2" is for the \n and \0 characters. tests if the user 
      has entered ctrl-D to return to menu*/
      if(fgets(tempString, LENGTH + 2, stdin) == NULL){
        
        free(tempString);
        return RTM;
      }
      else{/*tests if the user has hit enter to return to menu*/
      if(tempString[0] == '\n'){
          
          free(tempString);
          return RTM;
         }
      /* A string that doesn't have a newline character is too long. */
      if (tempString[strlen(tempString) - 1] != '\n')
      {
         printf("Input was too long. Please try again!\n");
         read_rest_of_line();
      }
      else
      {
         finished = TRUE;
      }
}
   } while (finished == FALSE);

   /* Overwrite the \n character with \0. */
   tempString[strlen(tempString) - 1] = '\0';
   
   /* Make the result string available to calling function. */
   strcpy(string, tempString);
   free(tempString);
   return TRUE;
}
/*this function searches for a node and return the struct*/
struct ppd_node *search_Nodes(struct ppd_system *system, char *id)
{
  struct ppd_node *current;
  current = system->item_list->head;
  /*while the list is not empty*/
  while(current != NULL)
  { /*if found return it*/
    if(strcmp(current->data->id, id) == 0){
      
      return current;
    }
    /*iterate to next node*/
    current=current->next;
  }
  return NULL;
}
/*check if the node exists, thus returning true or false*/
BOOLEAN node_exists(struct ppd_system *system, char *id){

  struct ppd_node *current;
  
  current = system->item_list->head;
  /*while the list is not empty*/
  while(current != NULL){
    /*if found return TRUE*/
    if(strcmp(current->data->id, id) == 0) {
      
      return TRUE;
    }

    else{
      /*iterate to next node*/
      current = current->next;
    }
  }

  return FALSE;
}
/**
 * @param system a pointer to a @ref ppd_system struct that holds all 
 * the data for the system we are creating
 **/
void system_free(struct ppd_system * system){

  struct ppd_node *current, *previous = NULL;
  /*Set current to the head*/
  current = system->item_list->head;
  /*while the list is not empty*/
  while(current != NULL){

    previous = current;
    current = current->next;
    
    /*clear/free the data*/
    free(previous->data);
    /*free the node*/
    free(previous);
  }
  /*free the ppd_system list*/
  free(system->item_list);
}
/*validate the items from the stock file but not the money component*/
BOOLEAN validate_stock_file(struct ppd_system *system, char * stock_array[], 
                                                              int line_number){
  int i;
  char  *id = stock_array[0],
        *name = stock_array[1],
        *desc = stock_array[2],
        *quantity = stock_array[4];
  
  /*tests if max items reached in machine */
  if(system->item_list->count >= 30){

    fprintf(stderr, "\nError: Machine cannot hold more than 30 items.\n\n");
    return FALSE;
  }

  /*Tests if the first character is an 'I'*/
  if(id[0] != 'I')
  {
    fprintf(stderr, "\nError: ID on line: %d in File: '%s' not valid. Id"
      " --MUST-- start with an 'I' and have 4 trailing digits ** ID: %s **\n\n",
                          line_number, system->stock_file_name, id);
    return FALSE; 
  }
  /*Tests if there is a duplicate entry in the stock file*/
  if(node_exists(system,id) == TRUE){

    fprintf(stderr, "\nError: Duplicate entry on line: %d in File: '%s'.\n\n", 
      line_number, system->stock_file_name);
      return FALSE; 
  }
  /*Tests if id contains 4 zeros*/
  for(i = 1; i < IDLEN; i++){
    if(!isdigit(id[i])){
      fprintf(stderr, "\nError:a ID on line: %d in File: '%s' not valid. Id"
      " --MUST-- start with an 'I' and have 4 trailing digits ** ID: %s **\n\n",
                          line_number, system->stock_file_name, id);
      return FALSE;   
    }
  }
  /*Tests if id contains 5 characters*/
  if(strlen(id) != 5){
    fprintf(stderr, "\nError:a ID on line: %d in File: '%s' not valid. Id"
    " --MUST-- start with an 'I' and have 4 trailing digits ** ID: %s **\n\n",
                          line_number, system->stock_file_name, id);
      return FALSE;   
  }
  /*tests the name length returns error if breached, if not copies 
        data to data structure*/
  if(strlen(name) > NAMELEN){
          
    fprintf(stderr, "\nError: Name length on line: %d in File:"
      "'%s' not valid. ** Name: %s **:\n\n", line_number, 
          system->stock_file_name, name);
          
    return FALSE;
  }
  /*tests the decription length returns error if breached, if not 
        copies data to data structure*/
  if(strlen(desc) > DESCLEN){
          
    fprintf(stderr, "\nError: Description length on line: %d in" 
      "File: '%s' not valid. ** Description: %s **:\n\n",
        line_number, system->stock_file_name, desc);
          
    return FALSE;
  }
  /*test that quantity amount is numeric in position 1*/
  if(!isdigit(quantity[0])){
         
    fprintf(stderr, "\nError: Quantity amount must be numeric in file:"
      " even if 0.'%s' on line %d\n\n",  system->stock_file_name,line_number);
    
    return FALSE;
  }
  /*test that quantity amount is numeric in position 2, if \n exisits 
    then ignore*/
  if((!isdigit(quantity[1])) && (quantity[1] != '\n')){
         
    fprintf(stderr, "\nError: Quantity amount must be numeric in file:"
      " evens if 0.'%s' on line %d\n\n",  system->stock_file_name,line_number);
    
    return FALSE;
  }
  /*Tests that the quantity does not exceed limit*/
  if(strlen(quantity) > MAX_STOCK_DIGITS + NEW_LINE_CHAR){

    fprintf(stderr, "\nError: Quantity amount must be less than 99:"
      " '%s' on line %d\n\n",  system->stock_file_name,line_number);
    
    return FALSE;
  }
  
  return TRUE;
}
/*validate the money compoment from the stock file*/
BOOLEAN validate_money(struct ppd_system *system, char *dollars, char *cents, 
                                                            int line_number){
  char *endPtr;
  int temp_cents;
  /*tests if price is is greater than 99 dollars*/
  if(strlen(dollars) > MAX_PRICE_DIGITS ){
    
    fprintf(stderr, "\nError: Dollars component of cost in file: '%s'" 
      " incorrect must be between 0 and 99\n\n", system->stock_file_name);
    return FALSE;
  }
  /*check that the first position is a digit*/
  if(!isdigit(dollars[0])){
    
    fprintf(stderr, "\nError: Cost value in file: '%s'" 
      " not valid. Note: Spaces not allowed.\n\n", 
              system->stock_file_name);
    return FALSE;
  }
  /*check that the second position is a digit*/
  if(strlen(dollars) == MAX_PRICE_DIGITS){
    if(!isdigit(dollars[1])){
    
    fprintf(stderr, "\nError: Cost value in file: '%s' is" 
      " not valid. Note: Spaces not allowed.\n\n", 
              system->stock_file_name);
    return FALSE;
    }
  }
  /*tests if imput is corrects and there is a value on the left side */
  if(cents == NULL){
    
    fprintf(stderr, "\nError: Price amount incorrect." 
      " Please ensure price is in format *** (0)0.00 *** \n\n");
    return FALSE;
  }
  /*tests if price is is greater than 99 cents*/
  if(strlen(cents) > MAX_PRICE_DIGITS ){
   
    fprintf(stderr, "\nError: Cents component of cost in file: '%s' is" 
      "not valid. Must be between 0 and 99\n\n", system->stock_file_name);
    return FALSE;
  }
  /*check that the first position is a digit*/
  if((!isdigit(cents[0])) || (!isdigit(cents[1]))){
   
    fprintf(stderr, "\nError: Cost value in file: '%s' is" 
      " not valid. Note: Spaces not allowed.\n\n", 
              system->stock_file_name);
    return FALSE;
  }
  /*Validates that cents is a multiple of 5*/
  temp_cents = (int) strtol(cents, &endPtr, 10);
  if(temp_cents % 5 != 0){
    fprintf(stderr, "Error: The cents need to be a multiple of 5\n"); 
    return FALSE;
  }

  return TRUE;
}

/*validates the coin file before it load into the system*/
BOOLEAN validate_coins(struct ppd_system *system, char * coin[], int line_number){
  
  int i, denom, denomCount = 0;
  char *endPtr, *quantity; 
  int coin_cents_array[NUM_DENOMS] = {FIVE,TEN,TWENTY,FIFTY,HUNDRED,
                                            TWO_HUNDRED,FIVE_HUNDRED,THOUSAND};     
  /*assigns values and converts to number*/
  quantity = coin[1];
  denom = strtol(coin[0], &endPtr, 10);
  /*tests if the denom is numeric*/  
  if(strcmp(endPtr, "") != 0){
    
    fprintf(stderr, "\n\nError: Coin file: '%s' invalid on line: %d. \n\n", 
      system->coin_file_name,line_number );
    return FALSE;
  }
  /*checks if the denom exists*/
  for(i = 0; i < NUM_DENOMS; i++){

    if(denom != coin_cents_array[i]){
      denomCount++;
    }
  }
  /*if denom count equal 8 then it does not exist in file if it greater 
  than 1 then it exists twwice*/
  if(denomCount == NUM_DENOMS){
    fprintf(stderr, "\n\nError: Coin file: '%s' invalid on line: %d. \n\n", 
        system->coin_file_name,line_number );

    return FALSE;
  }
   /*test that quantity amount is numeric in position 1*/
  if(!isdigit(quantity[0])){
         
    fprintf(stderr, "\nError: Quantity amount must be numeric in file:"
      " even if 0.'%s' on line %d\n\n",  system->coin_file_name,line_number);
    
    return FALSE;
  }
  /*test that quantity amount is numeric in position 2, if \n exisits 
    then ignore*/
  if((!isdigit(quantity[1])) && (quantity[1] != '\n')){
         
    fprintf(stderr, "\nError: Quantity amount must be numeric in file:"
      " evens if 0.'%s' on line %d\n\n",  system->coin_file_name,line_number);
    
    return FALSE;
  }
  /*Tests that the quantity does not exceed limit*/
  if(strlen(quantity) > MAX_COIN_DIGITS + NEW_LINE_CHAR){

    fprintf(stderr, "\nError: Quantity amount must be less than 99:"
      " '%s' on line %d\n\n",  system->coin_file_name,line_number);
    
    return FALSE;
  }

  return TRUE;
}
/*validates that there in only one denom of each typr in file*/
BOOLEAN validate_duplicate_coin(struct ppd_system *system){

  int i,j,sameCoin = 0;
  /*last coin validation to check that sanme coin doesnt appear twice*/
  for(i = 0; i < NUM_DENOMS; i++){
    for(j = 0; j < NUM_DENOMS; j++){
      if(system->cash_register[i].denom == system->cash_register[j].denom){
        sameCoin++;
      }
    }/*if denom appears more than once, exit with error message*/
    if(sameCoin > 1){
      fprintf(stderr, "\nError: You cannot not have duplicate denominations in"
      " file. File: '%s'\n\n", system->coin_file_name);
      return FALSE;
    } 
    /*reset denom count*/
    sameCoin = 0;
  }
  return TRUE;
}
/*this function sorts the coin file so that it does not corrupt the system 
if its not in correct order*/
void sort_coins(struct ppd_system *system){

  int i, j, temp_denom, temp_count;

  for(i = 0; i < NUM_DENOMS; i++){
    for(j = 0; j < NUM_DENOMS - 1; j++){
     /*goes through the coin array sorting it into place*/
      if(system->cash_register[i].denom < system->cash_register[j].denom){
        
        temp_denom = system->cash_register[i].denom;
        system->cash_register[i].denom = system->cash_register[j].denom; 
        system->cash_register[j].denom = temp_denom;

        temp_count = system->cash_register[i].count;
        system->cash_register[i].count = system->cash_register[j].count; 
        system->cash_register[j].count = temp_count;
        
      }
    }
  }
}
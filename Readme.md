
# Vending Machine - CLI 

This is just a simple vending machine application that is intended for CLI. It takes in two files as parameters, stock.dat (list of products available in the vending machine) and coins.dat (the available coins in the vending machine).

## Installing

Extract the zip file to any desired folder and make sure the vm, stock.dat and coin.dat are in the same folder.

## Usage

./ppd stock.dat coins.dat


### Use Instructions 

1. The vending machine does not accept any empty files. It is assumed that it is 
always loaded with items and coins. 

2. The vending machine does not stock more than 30 items at any 1 time. For this reason 
when allocating a unique ID it start at I0001 and then moves its way up to I0030. I know in the 
scheme of thing if we were to have **many** items this technique would not be efficient, but because of
the low amount of items I thought it was a good design decision. 

3. The vending machine will not accept any file that does not have a new line '\n at the end of it', all
files must have a '\n' at the end of the file. 

4. calculate_Change() - When I initially wrote this function up I wanted to save all the refunded coins into an array and then release them once the sale has finalised. To do this I made a dynamic array
that stored all of these. I guess are far simplar option would have been if I just made a fixed size array and then when it was full to exit with an error. However, using realloc and learning about how you can reallocate memory was a good learning experience hence why I didnt change my function in the end. 

5. When a user tries to purchase a product that doesnt exist they are returned to the main menu. I have implemented my function like this so that the user can select display items again and retrieve the correct item ID.

## Contributing

1. Fork it!
2. Create your feature branch: git checkout -b my-new-feature
3. Commit your changes: git commit -am 'Add some feature'
4. Push to the branch: git push origin my-new-feature
5. Submit a pull request 

## Credits

Startup Code: Paul Miller

Amended: John Lakkis

## License

The MIT License (MIT)
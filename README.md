README:


Application name: myApp

###########################################################################################

Application Description:
*	Application displays the menu of the food items available for any given date.
*	It shows the best possible choice of items to cover the given budget from present date to
	upcoming subsequent dates. 
*	It also keeps in mind the preference of not having to see certain items in the optimum solution.
*	Application connects to Internet to secure information about latest entries in Menu. 
*   Information about how the application behaves is constantly logged. 

###########################################################################################

Application setup: 

	If the application file myApp is missing then please use the makefile to regenerate 
	the file.

	> make -f zp_make.GNUmakefile -l

###########################################################################################

Application Running Modes:

1. BEST CHOICE OF ITEMS ON A GIVEN BUDGET:
   
	* In order to show these items, run the following command on the terminal.

   	>./myApp BUDGET 		

   	here budget represents currency, please ensure that no special symbols viz. # $ %
   	precede that value.
   	
   	![alt tag] (https://github.com/srajappa/budgetCanteen/blob/master/zp1.png)

   	* In addition to this, filters can also be applied so that certain items be excluded
   	while computing the optimal solution. Run, the following command on the terminal.

   	>./myApp BUDGET ITEM

   	here budget needs to be entered the same way as it is expected in the section above.
   	The ITEM should be name or item, no special symbols should be entered. 
   	
   	![alt tag] (https://github.com/srajappa/budgetCanteen/blob/master/zp2.png)

2. MENU FOR THE UPCOMING DAYS:

	* Menu of items from current day to the subsequent available dates is printed by
	running the following command. 

	>./myApp menu 
	
	![alt tag] (https://github.com/srajappa/budgetCanteen/blob/master/zp3.png)
	
	* If menu details of a specific date needs to be displayed, type the following.

	>./myApp menu YYYY-MM-DD

	here YYYY represents the year eg. 2015, MM represents the month eg. 06 and DD represents
	date. Ensure that the year, month and date are seperated by hyphen '-' symbol.
	
	![alt tag] (https://github.com/srajappa/budgetCanteen/blob/master/zp4.png)

3. HELP 
	
	* For help type the following command in the terminal.

	>./myApp -help

	![alt tag] (https://github.com/srajappa/budgetCanteen/blob/master/zp5.png)

*****  Screenshots zp1, zp2, zp3, zp4, zp5 show the above scenarios respectively.    ******

###########################################################################################

Application Testing Done: 

Tested OK:
System details:
	OS: linux 14.04
	GCC: 4.8.4
	Terminal: gnome-terminal 3.16.2
	Internet Service Provider: Time-Warner

###########################################################################################

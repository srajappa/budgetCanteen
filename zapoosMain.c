/*
* Description: 
* Main calls functions in helper.c to perform operations after validatation of inputs viz. 
* 1. Scraping and saving JSON data from the given webpage.
* 2. Analyze the content and create several CSV files which are differentiated based on the date.
* 3. Check the system date and find the appropriate menus that are either fit for that date or further dates. 
* 4. Perform analysis to what can be purchased for the current date on a given limit of budget and then print the same.
* 5. Delete all the meta-data files. 
* Date: 2 July, 2015
* @author: Srinivasan Rajappa
*/

#include "bistro.h"

int main(int argc, char*argv[]){
	const char url[] = "http://codapi.zappos.biz/menus";
	char *rawFile,*csvfile,*sysDate;
	
	float budgetVal;
	
	
	sysDate = (char*)malloc(sizeof(char)*20);
		memset(sysDate,'\0',20);

	budgetVal = validator(argc, argv);	//check the validity of the input arguments
	rawFile = readURL(url);				//Read the content from the website and get the name of the file 
	
	parseFile(rawFile);					//Performs parsing of the JSON data on the file name stored in rawFile
	
	sysDate = systemDate();				//Find the system date and save it in sysDate variable. 
	
	//Debugging statements
		strcpy(sysDate,"2015-05-11");	
	
	returnBestList(sysDate,directoryExam());	//Use the sysdate to find the appropriate files and then perform operations.
	
	printf("HAVE A NICE DAY! \n");

	purgeFiles(directoryExam());				//Maintenance :) 
	deleteFile(rawFile);

	return 0;
}

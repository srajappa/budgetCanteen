/*
* Description: 
* Calls functions in helper.c . 
* Date: 2 July, 2015
* @author: Srinivasan Rajappa
*/

#include "bistro.h"
#include "error.h"


int main(int argc, char*argv[]){
	refresh(START);

	char *rawFile,*csvfile,*sysDate;
	const char url[] = "http://codapi.zappos.biz/menus";
	float budgetVal;
	
	
	sysDate = (char*)malloc(sizeof(char)*20);
		memset(sysDate,'\0',20);

	budgetVal = validator(argc, argv);	//check the validity of the input arguments
	rawFile = readURL(url);				//Read the content from the website and get the name of the file 
	
	parseFile(rawFile);					//Performs parsing of the JSON data on the file name stored in rawFile
	
	sysDate = systemDate();				//Find the system date and save it in sysDate variable. 
	
	//Debugging statements
	//strcpy(sysDate,"2015-05-11");	
	
	returnBestList(sysDate,directoryExam());	//Use the sysdate to find the appropriate files and then perform operations.
	
	purgeFiles(directoryExam());				//Maintenance :) 
	deleteFile(rawFile);

	refresh(STOP);

	return 0;
}


/*--2015-07-10 02:03:09--  http://codapi.zappos.biz/menus
Resolving codapi.zappos.biz (codapi.zappos.biz)... failed: Name or service not known.
wget: unable to resolve host address ‘codapi.zappos.biz’*/
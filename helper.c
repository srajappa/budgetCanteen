/*
* Contains all the helper functions that will help extract JSON data from weblink
* and create multple CSV files. Some functions also perform analysis so that a best fit
* can be found to buy content on a limited budget. 
* Other functions help validate the command line input etc. 
* All the operations are tracked by logging events and errors.
* There are various sections and each section has similar set of functions.
* 1. String Utilities.
* 2. Date related functions.
* 3. Input Validators.
* 4. File Download and directory exploration fucntions.
* 5. JSON data parsing and CSV file operations. 
* 6. Operations involving calculation of best solutions. 
* 7. Purging meta data files. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* Edited: 
*      - 11 July, 2015 
*/

#include "bistro.h"
#include "time.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"
#include "error.h"



const char url[] = "http://codapi.zappos.biz/menus";

int numOfVal=0;
int numOfFiles = 0;
char *firstEntry;
float bd;
int dot,prec=0;
char str_budget[10];
int norteDam = 0; 
char filteredItem[MG];
int EXCLUSION_MODE = 0;
int WRONG_DATE = 0;




//===============================================================================================================================//
//=============================== String Utility functions here, below ==========================================================//
//===============================================================================================================================//
//===============================================================================================================================//



/*
* Function: Toupper
* Description:
* Converts a string in Any case to Upper case. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *Toupper(char*strVal){
	int i=0; 
	static char tmpStr[MG];
	strcpy(tmpStr,strVal);
	while(tmpStr[i]){
		tmpStr[i] = toupper(tmpStr[i]);
		i++;
	}
	return(tmpStr);
}

/*
* Function: stringToFloat
* Description:
* Character values in the string is mapped and checked and corresponding number is used 
* for constructing a float value. The precision level of the float string is also calculated. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
float stringToFloat(char *budStr){
	int i,locDot,pv=0,x,v=0;
	float rad;
	float result=0.0; 
	locDot =0;
	rad =1;
	if(strcmp(budStr,"Free")==0){
		return FREE;
	}

	for(i=0; i<strlen(budStr);i++){
		if(budStr[i]=='.'){
			dot = 1; 
			locDot = i+1;
		}
	}
	if(locDot==0){
		result = atoi(budStr);
		return result;
	}
	locDot-=1;
	for(i=locDot-1; i>=0; i--){
		v = mapToInt(budStr[i]);
		result += (rad * v);
		rad*=10;
	}
	rad = 0.1;
	x = min(strlen(budStr),locDot+3);
	for(i=locDot+1; i<x;i++){
		
		v = mapToInt(budStr[i]);
		
		if(v==0)
			pv = max(pv,0);
		else
			pv +=1;
		result += (rad*v);
		rad*=0.1;
	}
	
	prec = max(prec,pv);			//Precision value is estimated and stored in the gloabl variable
	return result;
}

/*
* Function: mapToInt
* Description:
* finds the corresponding value of character if it is a number. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
int mapToInt(char c){
	switch(c){
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		default: printf("mapToInt: Input Error\n");
				 logEntry("mapToInt: ","Input to switch incorrect",Err);
				 Exit(EXIT_ABNORMAL);
	}
}


/*
* Function: strMatchCase
* Description:
* Checks whether a string is a substring of another irrespective of case. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
int strMatchCase(char *ch, char *comp){
	int i,j, k,l;
	i=j=k=l=0; 
	int check=0,ind=0;
	char s1[strlen(ch)];
	char s2[strlen(comp)];
		memset(s1,'\0',strlen(ch));
		memset(s2,'\0',strlen(comp));

	strcpy(s1,ch);
	strcpy(s2,comp);

	strcpy(s1,Toupper(s1));			//change case of both strings to upper.
	strcpy(s2 ,Toupper(s2));

	if(strlen(s1)<strlen(s2))
		return NOTFOUND;
	for(i=0; i<strlen(s1)-strlen(s2)+1; i++){
		k=i;
		for(j=0;j<strlen(s2);j++){
			if(s1[k]==s2[j]){
				k++;
				check++;
			}
			else{
				break;
			}
		}
		k=0;
		if(check==strlen(s2)){
			l = 1;
			break;
		}
		check=0;
	}

	return l==0 ?  NOTFOUND : FOUND;
}


/*
* Function: strMatch
* Description:
* returns FOUND (1) if the second string is present in the first. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
int strMatch(char *ch, char *comp){
	int i,j, k,l;
	i=j=k=l=0; 
	int check=0,ind=0;
	if(strlen(ch)<strlen(comp))
		return NOTFOUND;
	for(i=0; i<strlen(ch)-strlen(comp)+1; i++){
		k=i;
		for(j=0;j<strlen(comp);j++){
			if(ch[k]==comp[j]){
				k++;
				check++;
			}
			else{
				break;
			}
		}
		k=0;
		if(check==strlen(comp)){
			l = 1;
			break;
		}
		check=0;
	}

	return l==0 ?  NOTFOUND : FOUND;
}


/*
* Function: sortFileNames
* Description:
* Sorts a list of strings.
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void sortFileNames(char **dayFiles, int num){
	int i,j;
	char temp[NG];
		memset(temp,'\0',NG);
	for(i=0; i< num; i++){
		for (j=0; j < num-1; j++){
			if(strcmp(dayFiles[j],dayFiles[j+1])>0){
				strcpy(temp,dayFiles[j]);
				strcpy(dayFiles[j],dayFiles[j+1]);
				strcpy(dayFiles[j+1],temp);
				memset(temp,'\0',NG);
			}
		}
	}

}



//===============================================================================================================================//
//=============================== Date Related functions here, below ============================================================//
//===============================================================================================================================//
//===============================================================================================================================//

/*
* Function: systemDate
* Description:
* Returns system date as a string. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *systemDate(){
	time_t now = time(NULL);
	int m,d,y;
    struct tm *Time = localtime(&now);
    m = Time->tm_mon + 1;
    d = Time->tm_mday;
    y = Time->tm_year+1900;
    char *mm,*dd,*yy;
    mm = (char*)malloc(sizeof(char)*NG);
    dd = (char*)malloc(sizeof(char)*NG); 
    yy = (char*)malloc(sizeof(char)*NG);

    memset(mm,'\0',NG);
    memset(yy,'\0',NG);
   	memset(dd,'\0',NG);
    
    if(m<10){
    	snprintf(mm,sizeof(mm),"%d",m);
    	mm[1] = mm[0];
    	mm[0] = '0';
    }else{
    	snprintf(mm,sizeof(mm),"%d",m);
    }

    if(d<10){
    	snprintf(dd,sizeof(dd),"%d",d);
    	dd[1] = dd[0];
    	dd[0] = '0';
    }else{
    	snprintf(dd,sizeof(dd),"%d",d);
    } 

    snprintf(yy,sizeof(yy),"%d",y);

    strcat(yy,"-");
    strcat(mm,"-");
    strcat(mm,dd);
    strcat(yy,mm);
   	logEntry("systemDate: ",yy,Info);

    return yy;
}

/*
* Function: formatDate
* Description:
* bespoke function to trim the date to a more friendly format. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void formatDate(char* dateVal){
	int i; 
	int flag = 0; 
	for (i=0; i<strlen(dateVal);i++){
		if(flag==1){
			dateVal[i] = '\0';
		}
		if(dateVal[i]=='T'){
			dateVal[i] = '\0';
			flag =1;
		}
	}
}

/*
* Function: properFormat
* Description:
* Changes the format of date from YYYY-MM-DD to DD Month, YYYY. Looks pretty good. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *properFormat(char *name){
	
	int year, month, date,i,j=0;
	for(i=0; i< 11; i++){
		if(i==4 || i==7)
			continue;
		else {
			if(name[i]>57 && name[i] <48){
				printf("Date Error- expected format YYYY-MM-DD\n");
				logEntry("properFormat (Incorrect date): ",name,Err);
				Exit(EXIT_APP);
			}
		}
	}


	year = atoi(csvVal(name,'-',1));
	month = atoi(csvVal(name,'-',2));
	

	name[10]='-';
	name[11]='\0';

	date = atoi(csvVal(name,'-',3));
	memset(name,'\0',MG);
	switch(month){
		case 1:	snprintf(name,MG,"%d January,%d",date, year);
				break;
		case 2:	snprintf(name,MG,"%d February,%d",date, year);
				break;
		case 3: snprintf(name,MG,"%d March,%d",date, year);
				break;
		case 4: snprintf(name,MG,"%d April,%d",date, year);
				break;
		case 5: snprintf(name,MG,"%d May,%d",date, year);
				break;
		case 6: snprintf(name,MG,"%d June,%d",date, year);
				break;
		case 7: snprintf(name,MG,"%d July,%d",date, year);
				break;
		case 8: snprintf(name,MG,"%d August,%d",date, year);
				break;
		case 9: snprintf(name,MG,"%d September,%d",date, year);
				break;
		case 10:snprintf(name,MG,"%d October,%d",date, year);
				break;
		case 11:snprintf(name,MG,"%d November,%d",date, year);
				break;
		case 12:snprintf(name,MG,"%d December,%d",date, year);
				break;
		default: logEntry("properFormat: ","Month entry incorrect",Err);
				 Exit(EXIT_ABNORMAL);
	}
	return name;
}


//===============================================================================================================================//
//=============================== Input Validating functions here, below ========================================================//
//===============================================================================================================================//
//===============================================================================================================================//


/*
* Function: validator
* Description:
* Analyzes the arguments and checks if the input are valid, 
* else throws out errors and terminates the application.
* If the arguments are correct then budget value is extracted and is returned as float.
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/

float validator(int argc, char *argv[]){
	float ret; 
	char inDate[NG];
		memset(inDate,'\0',NG);
	int i;
	if(argc==1){
		printf("Invalid number of Arguments \nType\n\t./executable -HELP for help\n");
		exit(0);
	}else{
		if(strcmp(Toupper(argv[1]),"-HELP")==0){
			logEntry("Help: ", "happy to do ;)", Info);

			printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			printf("Application prints the items that can be purchased\nfor consumption on a given budget per day.\n");
			printf("Running modes are as follows\n");
			printf("\t- ./executable BUDGET  \n\t\t[BUDGET amount can have decimal values]\n");
			printf("\t- ./executable BUDGET ITEM \n\t\t[ITEM refers to the specific food dish you want to exclude from list]\n");
			printf("=====================================\n");
			printf("Application can also show the Menu for particular day.\n");
			printf("\t- ./executable MENU \n\t\tPrints the Menu available for the upcoming dates.\n");
			printf("\t- ./executable MENU YYYY-MM-DD\n\t\tPrints the Menu available for given date.\n");
			Exit(EXIT_APP);
		}else if(strcmp(Toupper(argv[1]),"MENU")==0){
			//If a date is entered
			if(argc==3){
				strtok(argv[2],"\n");
				strcpy(inDate,argv[2]);

				
				if(inDate[4]=='-' && inDate[7]=='-' && strlen(inDate)==10){
					logEntry("Menu: Additional Param ",inDate,Info);
					char *rawFile = readURL(url);				
					parseFile(rawFile);

					properFormat(inDate);
					printf("\n\t****Menu for Date: %s****\n",inDate);
					printf("-----------------------------------------------------\n");
						memset(inDate,'\0',NG);
					strcpy(inDate,argv[2]);
					strcat(inDate,".csv");
					
					printContentFile(inDate);
					printf("_____________________________________________________\n");
					purgeFiles(directoryExam());
					deleteFile(rawFile);

					//free(rawFile);

				}
				Exit(EXIT_APP);
			}
			displayMenu();
		}else{
			ret = validBudgetInput(argv[1]);
			if(argc == 3){
				memset(filteredItem,'\0',MG);
				strcpy(filteredItem,strtok(argv[2],"\n"));
				logEntry("Filter: ",filteredItem,Info);


				printf("\n****Results after filtering: %s****\n",filteredItem );
				EXCLUSION_MODE = 1;
			}


			return ret;
		}
	}
}

/*
* Function: validBudgetInput
* Description:
* It analyzes if the budget entry is indeed a valid number. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
float validBudgetInput(char *budStr){
	int i,j,hasDoll; 
	float value;
	
	if(strcmp("FREE",Toupper(budStr))==0)
		return 0;


	for(i=0; budStr[i]!='\0'; i++){
		if(budStr[i] >= 48 && budStr[i] <= 57 || budStr[i]== 46){
			continue;
		}else{
			printf("Invalid Argument (BUDGET)\n\t Expected Format - number  \nType\n\t./executable -HELP for help\n");
			WRONG_COMMAND;
			logEntry("validBudgetInput: ","Budget Value not a number",Err);
			Exit(EXIT_IO);
		}
	}	
	memset(str_budget,'\0',10);
	strcpy(str_budget,budStr);
	value = stringToFloat(budStr);
	if(value<0){
		printf("Budget value cannot be a negative, Try again\nType\n\t./executable -HELP for help\n");
		WRONG_COMMAND;
		logEntry("validBudgetInput: ","Budget Value is negative",Err);
		Exit(EXIT_IO);
	}


	bd = value; 					//Store the Budget value in a Global Variable. 
	return value;
}


//===============================================================================================================================//
//=============================== File Download and directory exploration fucntions, below ======================================//
//===============================================================================================================================//
//===============================================================================================================================//


/*
* Function: readURL
* Description:
* wget command is used to save the JSON data from the given 
* URL link. It saves the content in a txt file whose name is hard coded. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *readURL(char* urlLink){

    FILE *filePtr;
    char wgetMagic[STR_MAX];
    memset(wgetMagic,'\0',STR_MAX);
    
    strcpy(wgetMagic, "wget --quiet ");
    strcat(wgetMagic,urlLink);
    strcat(wgetMagic," -O urlContent.txt");
    
    logEntry("wget on URL: ",urlLink,Info);

    filePtr = popen(wgetMagic,"w");
   
    if(filePtr == NULL){
    	printf("Unable to connect to BISTRO Website\n\t Unable to check because computer isn't connected to Internet \n");
    	logEntry("readURL: ","Internet Access Limited",Err);
    	NO_CONNECTION_TO_INTERNET;
    	Exit(0);
    }
    pclose(filePtr);
    return("urlContent.txt");
}


/*
* Function: getFileNames
* Description:
* Returns the file list in form of ; appending string values. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: http://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
*/

char * getFileNames(char *dir){
	int i=0,j=0,count=0; 
	char *namesOfFiles;
	DIR *dp,*ds;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        logEntry("getFileNames: ","Directory Missing",Err);
        Exit(EXIT_TRANS);
    }
    chdir(dir);
    namesOfFiles = (char*)malloc(sizeof(char)*MAXLINE);
    	memset(namesOfFiles,'\0',MAXLINE);

    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(strMatch(entry->d_name,".csv")==FOUND && entry->d_name[4]== '-' && entry->d_name[7]=='-'){
        	strcat(namesOfFiles,entry->d_name);
        	strcat(namesOfFiles,";");
        	i++;
        }
    }
    numOfFiles = i;
    return namesOfFiles;
}
/*
* Function: directoryExam
* Description:
* It calls the above function; it has stop gate measures to avoid
* falling into a situation when the current working directory is 
* not to be found. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: http://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
*/

char* directoryExam(){
	char cwd[MG];
	if (getcwd(cwd, sizeof(cwd)) == NULL){
       	fprintf(stderr,"Cannot find current working directory\n");
        logEntry("directoryExam: ","Current Working Directory missing",Err);
        Exit(EXIT_TRANS);
    }
    else
    	return getFileNames(cwd);
}




//===============================================================================================================================//
//=============================== JSON data extraction and parsing functions here, below ========================================//
//===============================================================================================================================//
//===============================================================================================================================//



/*
* Function: parseFile
* Description:
* Using cues from the paranthesis rule, the date, names and price of respective items are found. 
* Each menu for the date is assumed to be in a group and it will be easy to save the content from that
* entire file only for that subdate. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void parseFile(char *rawFile){
	FILE *filePtr,*noInt;
	char temp,c;
	int i = 0,j, parendCalc =0,noI=0;
	char *str = (char*) malloc(sizeof(char)*MAXLINE);
	filePtr = fopen(rawFile,"r");
	noInt = fopen(rawFile,"r");

	if(filePtr==NULL){
		printf("parseFile: FILE %s couldn't be opened\n", rawFile);
		exit(0);
	}

	while(c=(char)fgetc(noInt)!=EOF){
		noI++;
	}
	fclose(noInt);
	if(noI <10){
		printf("Unable to connect to BISTRO Website.\nUnable to check because computer isn't connected to Internet. \n");
		logEntry("readURL: ","Internet Access Limited",Err);
    	NO_CONNECTION_TO_INTERNET;
    	deleteFile(rawFile);
    	Exit(0);
	}

	while(i!=10){		//Position the cursor to the head 
		temp = (char) fgetc(filePtr);
		i++;
	}		
	i=0,j=0;
	memset(str,'\0',MAXLINE);
	while(1){
		temp = (char) fgetc(filePtr);
		if(temp=='{'||temp=='['){
			parendCalc++;
		}
		else if(temp == '}' || temp == ']'){
			parendCalc--;
		}else if(temp == EOF){
			break;
		}
		else if(temp==','){
			continue;
		}else{
			str[j] = temp;
			j++;
		}
		if(parendCalc == 0){
			j=0;
			analyzeTrail(str);
			memset(str,'\0',MAXLINE);
		}
	}
	
	close(filePtr);

}

/*
* Function: extract_val
* Description:
* Finds the value of particular name, date or price value by positioning
* the index to the first " symbol. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char * extract_val(int index,char *ptr){
	int i,k=0; 
	int countQuote =0; 
	char *newString = (char*)malloc(sizeof(char)*MG);
	memset(newString,'\0',MG);
	
	for(i =index; i<strlen(ptr); i++){
		if(ptr[i]=='"'){
			countQuote++;
		}else{
			newString[k] = ptr[i];
			k++;
		}

		if(countQuote==2){
			break;
		}

	}
	return newString;
}



/*
* Function: match
* Description:
* Returns the index in a string (specifically for JSON) where 
* open clause is found. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
int match(char *ch, char *comp, int index){
	int i,j, k;
	i=j=k=0; 
	int check=0,ind=0;
	for(i=index; i<strlen(ch)-strlen(comp)+1; i++){
		k=i;
		for(j=0;j<strlen(comp);j++){
			if(ch[k]==comp[j]){
				k++;
				check++;
			}
			else{
				break;
			}
		}
		k=0;
		if(check==strlen(comp)){
			ind = i;
			break;
		}
		check=0;
	}

	return ind==0 ?  NOTFOUND : (ind + strlen(comp)+2);
}

/*
* Function: extractValue
* Description:
* returns the string of values for names,etc.
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char ** extractValue(char *ptr, char *comp, int MODE){
	if(MODE == SINGLE){
		char **v = malloc(sizeof(char));
		v[numOfVal] = malloc(sizeof(char)*MG);
		memset(v[numOfVal],'\0',MG);
		int index = match(ptr,comp,0);
		strcpy(v[numOfVal],extract_val(index,ptr));
		numOfVal++;
		return v;
	}else{
		char **p = malloc(sizeof(char)*10);
		int index = 1;
	
		while(index != NOTFOUND)
			//numOfVal!=39||index!=strlen(comp)+2)
		{
			p[numOfVal] = malloc(sizeof(char)*MG);
			memset(p[numOfVal],'\0',MG);
			index = (index == 1 ? match(ptr,comp,index-1): match(ptr,comp,index)) ;
			strcpy(p[numOfVal],extract_val(index,ptr));
			if(numOfVal==0)
				strcpy(firstEntry,p[numOfVal]);
			numOfVal++;
		}
		return p;
	}
}


/*
* Function: analyzeTrail
* Description:
* Writes the content of the menu i.e. name, price etc. into a csv file.  
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void analyzeTrail(char *str){
	int i;
	FILE *writePtr;
	char *fileName = (char*)malloc(sizeof(char)*MG);
	firstEntry = (char*)malloc(sizeof(char)*MG);
	char *t1 = (char*)malloc(sizeof(char)*MG);
	char *t2 = (char*)malloc(sizeof(char)*MG);



	memset(t1,'\0',MG);
	memset(t2,'\0',MG);
	memset(fileName,'\0',MG);
	memset(firstEntry,'\0',MG);


	char **v = extractValue(str,"date",SINGLE);
	strcpy(fileName,v[0]);
	//printf("analyzeTrail: date: %s\n",v[0]);
	numOfVal =0; 
	formatDate(fileName);


	strcat(fileName,".csv");
	writePtr =fopen(fileName,"w");

	//Find the names
	char **n = extractValue(str,"name",MULTI);
	numOfVal = 0; 

	strcpy(t1,firstEntry);
	memset(firstEntry,'\0',MG);

	//Find the price
	char **p = extractValue(str,"price",MULTI);
	strcpy(t2,firstEntry);
	memset(firstEntry,'\0',MG);
	for(i=0; i<numOfVal; i++){
		if(i==0){
			int numChar = fprintf(writePtr, "%s,%s,\n",t1,t2);
			memset(t1,'\0',MG);
			memset(t2,'\0',MG);
		}
		else{
			int numChar = fprintf(writePtr, "%s,%s,\n",n[i],p[i]);
		}
	}
	fflush(writePtr);
	//free(p);
	//free(n);
	//free(v);
	
	close(writePtr);
	numOfVal = 0; 
}



/*
* Function: csvVal
* Description:
* Extracts a string from a seperated value file, the inputs are the special symbols viz.
* , - or ; and the line input. Num identifies the column. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: http://stackoverflow.com/questions/12911299/read-csv-file-in-c
*/
char *csvVal(char*line,char symbol, int num){
	int i; 
	int fCom=0,lCom;
	if(line[0]==symbol){
		if(num==1){
			return "NULL";
		}else{
			return strtok(line,",-");
		}
	}else{
		for(i=0;i<strlen(line);i++){
			if(line[i]==symbol){
				lCom = i;
				num--;
				if(num==0){
					break;
				}else{
					fCom = lCom+1;
				}
			}
		}

		char *result = (char*)malloc(sizeof(char)*MG);
		memset(result,'\0',MG);
		int k =0; 
		for(i=fCom; i<lCom; i++){
			result[k] = line[i];
			k++;
		}
		return result;
	}
	return NULL;
}


//===============================================================================================================================//
//=============================== Calculation of optimal solution done here, below ==============================================//
//===============================================================================================================================//
//===============================================================================================================================//


void printContentFile(char *fileName){
	FILE *fptr,*recPtr;
	fptr = fopen(fileName,"r");
	recPtr = fopen(fileName,"r");
	//char tmp[STR_MAX];
	logEntry("printContentFile:(dated) ",fileName,Info);
	char *tmp = (char*)malloc(sizeof(char)*STR_MAX);

	char c;
	int k=0;
	//printf("FILE: %s\n",fileName);
	if(fptr==NULL){
		printf("No items available on  %s in the database\n",properFormat(csvVal(fileName,'.',1)));
		printf("_____________________________________________________\n");
		purgeFiles(directoryExam());
		deleteFile("urlContent.txt");
		logEntry("printContentFile: ","Nothing available today",Info);
		Exit(EXIT_APP);
	}else{
		char line[128];
			memset(line,'\0',128);

		while((c=(char)fgetc(recPtr))!=EOF){
	    	k++;
	    }
	    fclose(recPtr);
	    //printf("All ok\n");
		while(fgets(line,k,fptr)){
			memset(tmp,'\0',STR_MAX);
			strcpy(tmp,strdup(line));
			if(strMatchCase(csvVal(tmp,',',1),"Closed")==FOUND || strMatchCase(csvVal(tmp,',',1),"Holiday")==FOUND)
				printf("Closed for the Holiday\n");
			else{
				if(strMatchCase(csvVal(tmp,',',1),"NULL")==FOUND)			//Skip null entries;
					continue;						
/*				if(strMatchCase(csvVal(tmp,',',2),"free")==FOUND || 
						strcmp(csvVal(tmp,',',2),"0.00")==0 ||
						strcmp(csvVal(tmp,',',2),"0")==0 ||
						strcmp(csvVal(tmp,',',2),"0.0")==0) 
					printf("%-40s Free\n",csvVal(tmp,'-',1));*/
				else{
					printf("%-40s $%.2f\n",csvVal(tmp,',',1),stringToFloat(csvVal(tmp,',',2)));
				}	
					//printf("%s\n",tmp);
			}
		}
	}


	fclose(fptr);
}


void displayMenu(){
	int i,j=0,k=0,q=0;
	char c;
	char name[NG];
		memset(name,'\0',NG);


	char *rawFile = readURL(url);				
	
	parseFile(rawFile);	

	char filesList[MAXLINE];
		memset(filesList,'\0',MAXLINE);
	strcpy(filesList,directoryExam());

	char *sysDate = (char*)malloc(sizeof(char)*MG);
		memset(sysDate,'\0',MG);
	sysDate = systemDate();
	//strcpy(sysDate,"2015-05-11");
	char **dayFiles = malloc(sizeof(char*)*STR_MAX);

	strcat(sysDate,".csv");
	
	for(i=0; i<strlen(filesList);i++){
		if(filesList[i]==';'){
			name[j]='\0';
			j=0;
			
			if(strcmp(sysDate,name)<=0){
				dayFiles[q] = (char*)malloc(sizeof(char)*NG);
					memset(dayFiles[q],'\0',NG);
				strcpy(dayFiles[q++],name); 
			}
		}else{
			name[j++]=filesList[i];
		}
	}
	sortFileNames(dayFiles,q);
	char tempDate[NG];
	for(i=0; i<q; i++){
		printf("\n");
		
			memset(tempDate,'\0',NG);
		strcpy(tempDate,dayFiles[i]);
		//printf("%s \n",tempDate );
		if(strcmp(sysDate,dayFiles[i])==0)
			printf("Date: %s (Today)\n",properFormat(tempDate) );
		else{
			printf("Date: %s\n",properFormat(tempDate) );
		}
		printf("-------------------\n");
		printContentFile(dayFiles[i]);
		printf("____________________\n");
		//free(tempDate);
	}
	//printf("\n");
	purgeFiles(directoryExam());
	deleteFile(rawFile);
	free(dayFiles);
	//free(rawFile);
	free(sysDate);
	exit(0);
}


/*
* Function: knapSackDone
* Description:
* Identified the given problem as knapsack problem. It performs all steps
* followed by the links given in the references. Precision is only used for
* resolving floating point numbers. 
* if value is 1.75 then the entire input of float numbers is multiplied by 
* 100. Thus providing accuracy in results. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: https://en.wikipedia.org/wiki/Knapsack_problem
* 			  https://www.youtube.com/watch?v=kH7weFvjLPY&list=LLrg0F1k8WcbpHzV0kZZU3ow&index=2
*/
void knapSackDone(char **n, float *fnum,int numRec, float budget,int prec){
	
	int i,j,x,y,W,a,b,c,I,J,k;
	int mplier=1;
	float temp;
	
	switch(prec){
			case 0: mplier =1;
					break;
			case 1: mplier =10;
					break;
			case 2: mplier =100;
					break;
			default: printf("knapSackDone: precision error \n");
						exit(0);
	}
	temp = budget *mplier;
	budget = temp;
	int *new_fnum = (float*)malloc(sizeof(float)*NG);
	for(i=0; i<numRec; i++){
			temp = (int)(fnum[i]*mplier);
			new_fnum[i] = temp;
	}

	x = numRec+1;

	W = (int)budget;
	
	int *a_map = malloc(sizeof(int)*((W+1)*x));
	int *b_map = malloc(sizeof(int)*((W+1)*x));
	int *prim = malloc(sizeof(int)*((W+1)*x));
	for(i=0; i<x; i++){
		for(j=0; j<(W+1); j++){
			prim[i*(W+1)+j] = 0;
			a_map[i*(W+1)+j] = 0;
			b_map[i*(W+1)+j] = 0;
		}
	}

	for(i=1; i < x; i++){
		for(j=0; j<W+1; j++){
			if(new_fnum[i-1]>j){
				y = prim[(i-1)*(W+1)+j];
				prim[i*(W+1)+j] = max(y,0);
				continue;
			}else{
				a = prim[(i-1)*(W+1)+j];
				c = abs(new_fnum[i-1]-j);
				//c = c < 0 ? 0: c;
				b = prim[(i-1)*(W+1)+c]+1;
				prim[i*(W+1)+j] = max(a,b);
				if(max(a,b)==b){
					a_map[i*(W+1)+j] = 1;
					b_map[i*(W+1)+j] = (i-1)*10 + j-new_fnum[i-1];
				}
			}
		}
	}
	I = x-1;
	J = W;
	k = 0;

	int *chosen = (int*)malloc(sizeof(int)*15);
	for(i=0; i<numRec; i++){
		chosen[i]=NOTFOUND;
	}
	while(I!=0){
		if(a_map[I*(W+1)+J]==1){
			chosen[k]=I;
			a = J-new_fnum[I-1];
			if(a==0)
				break;
			J = a;
			I--;
			k++;
		}else{
			I--;
		}
	}
	budget/=mplier;
	int notInBujget=0;
	
	for(i=0; i<numRec; i++){
		
		if(chosen[i]==NOTFOUND){
			notInBujget++;
			continue;
		}			
		if(fnum[chosen[i]-1]>budget)
			continue;
		printf("%-40s $%.2f\n",n[chosen[i]-1],fnum[chosen[i]-1]);
	}
	
	if(notInBujget==numRec && norteDam == 0){
		printf("Nothing available in given budget: $%.2f\n",budget);
	}
	//free(a_map);
	//free(b_map);
	//free(prim);
	//free(n);
	//free(fnum);
	//free(chosen);
}	


/*
* Function: makeUse
* Description:
* Calls the knapsack function and performs the operations. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/

void makeUse(char**n, char**p, int numRec, float budget){
	int i,j =0,p_val=0,flag=0, s_p=0;
	float sample_bud;
	char dupStr[NG];
	char dupStr2[NG];
		memset(dupStr2,'\0',NG);
		memset(dupStr,'\0',NG);
	//Print the free Ones and the Closed on Duty ones
	if(strcmp(n[0],Toupper("Closed for the Holiday"))==0||strMatchCase(n[0],"Closed")==FOUND||strMatchCase(n[0],"Holiday")==FOUND){
		printf("%-40s\n","Closed for the Holiday");
		flag = 1;		//exit(0);
	}
	if(flag==0){
		char **new_n = (char*)malloc(sizeof(char)*MG);
		char **new_p = (char*)malloc(sizeof(char)*MG);
		// ********STOP GATE MEASURES-----BEGIN
		if(new_n==NULL)
			printf("makeUse: new_n out of memory\n");
		if(new_p==NULL)
			printf("makeUse: new_p out of memory\n");
		// ********STOP GATE MEASURES-----END

		for(i=0; i<numRec; i++){
			if(strMatchCase(n[i],filteredItem)==FOUND && EXCLUSION_MODE==1 ){
				continue;
			}
			else if(strlen(p[i])==1 || strlen(n[i])==1){
				continue;
			}else if(strMatchCase(p[i],"Free")==FOUND||
				strcmp(p[i],"0")==0||
				strcmp(p[i],"0.0")==0||
				strcmp(p[i],"0.00")==0
				){
				printf("%-40s Free\n",n[i]);
				norteDam = 1;
				continue;
			}
			else{
				new_n[j] = (char*)malloc(sizeof(char)*MG);
				if(new_n[j]==NULL)
					printf("makeUse: new_n[%d] out of memory\n",0);
				memset(new_n[j],'\0',MG);
				strcpy(new_n[j],n[i]);
				//printf("jkdld %s\n",new_n[i] );
				new_p[j] = (char*)malloc(sizeof(char)*MG);
				if(new_p[j]==NULL)
					printf("makeUse: new_p[%d] out of memory\n",0);
				memset(new_p[j],'\0',MG);
				strcpy(new_p[j],p[i]);
				j++;
			}
		}
		
		numRec = j;

		//free(p);
		float *fnum = (float*)malloc(sizeof(float)*NG);
		dot = 0; 			//initialize
		prec = 0;
		for(i=0; i<numRec; i++){
			fnum[i] = stringToFloat(new_p[i]);
		}
		knapSackDone(new_n,fnum,numRec,budget,prec);
	}
}
/*
* Function: makeDecision
* Description:
* Calls the makeUse function after extracting values from the files. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/

void makeDecision(char *sysDate){
	FILE *readFile;
	
	readFile = fopen(sysDate,"r");
	
	char sample[MG];
		

	if(readFile == NULL){
		FILE_NOT_FOUND;
		logEntry("makeDecision, file disappears :",sysDate,Err);
		Exit(EXIT_ABNORMAL);
	}

	
	char line[1024];
	memset(line,'\0',1024);
    int x  =0, i=0, numRec =0;
    
    char c;
    
    while((c=(char)fgetc(readFile))!=EOF){
    	if(c==','){
    		numRec++;
    	}
    	i++;
    }
    numRec /=2;
    numRec --; 
    
    fclose(readFile);
    readFile = fopen(sysDate,"r");

    char **str = malloc(sizeof(char *)*numRec);
    x=0;

    /*  AWRY ALERT: 
    	Code goes awry over here, the first entry from the csv file goes corrupted the printfs
    	in comments bear witness. 

    */
    while(fgets(line,i,readFile)){

    	str[x] = (char*)malloc(sizeof(char)*STR_MAX);
    	memset(str[x],'\0',STR_MAX);
    	snprintf(str[x],MG,"%s%s%s%s",csvVal(line,',',1),"-",csvVal(line,',',2),"-");

    	x++;
    	
    }
    
    char **name = (char*)malloc(sizeof(char)*MG);
    char **price= (char*)malloc(sizeof(char)*MG);
    //Price
    for(i=0; i<numRec ; i++){
    	price[i]=(char*)malloc(sizeof(char)*MG);
    	memset(price[i],'\0',MG);
		strcpy(price[i],csvVal(str[i],'-',2));
    }
    //Name
    for(i=0; i<numRec ; i++){

    	name[i]=(char*)malloc(sizeof(char)*MG);
    	memset(name[i],'\0',MG);
		strcpy(name[i],csvVal(str[i],'-',1));
    	
    	if(strcmp(str[i+1],"NULL")==0){
    		break;
    	}
    }
    close(readFile);

    makeUse(name,price,numRec,bd);
    //free(name);
    //free(price);
}




/*
* Function: returnBestList
* Description:
* extracts the file list and checks against the sysdate,
* if the sysdate is not in the list then it terminates. If the 
* file's name i.e. date value is higher than that of sysdate then 
* it's content is printed. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void returnBestList(char *sysdate,char *namesOfFiles){
	char *fileDate = (char*)malloc(sizeof(char)*MG);
	char *name = (char*) malloc(sizeof(char)*MG);
	char *dname = (char*) malloc(sizeof(char)*MG);
		memset(fileDate,'\0',MG);
		memset(name,'\0',MG);
		memset(dname,'\0',MG);

	strcpy(fileDate,sysdate);
	strcat(fileDate,".csv");
	
	int i,j,k,flag,val,found,q;
	i=k=j=flag=q=found=0;
	
	char **dayFiles = malloc(sizeof(char*)*STR_MAX);


	for(i=0; i<strlen(namesOfFiles); i++){
		if(namesOfFiles[i]==';'){
			name[j]='\0';
			j=0;	
			if(strcmp(fileDate,name)<=0){
				dayFiles[q] = (char*)malloc(sizeof(char)*NG);
					memset(dayFiles[q],'\0',NG);
				strcpy(dayFiles[q],name); 
				//printf("(day :%s\n",dayFiles[q]);
				q++;

			}
		}else{
			name[j++] = namesOfFiles[i];		
		}
	}
	sortFileNames(dayFiles,q);
	for(i=0; i< q; i++){
		printf("\n");		
		strcpy(dname,dayFiles[i]);
		if(strcmp(fileDate,dayFiles[i])==0)
			printf("Date: %s (Today)\n",properFormat(dname) );
		else
			printf("Date: %s\n",properFormat(dname));
		printf("-------------------\n");
		//printf("%s\n",dayFiles[i]);
		makeDecision(dayFiles[i]);
		printf("____________________\n");
		flag =1;
	}

	if(flag == 0){
		printf("No menus found for the date (%s) and the ones beyond this date\n",sysdate );
	}
	free(fileDate);
	free(dayFiles);
	free(name);
	free(dname);
}


//===============================================================================================================================//
//=============================== Purging meta data files are  done here, below =================================================//
//===============================================================================================================================//
//===============================================================================================================================//






/*
* Function: deleteFile
* Description:
* Deletes the file whose name is provided as an argument.
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: http://www.programmingsimplified.com/c-program-delete-file
*/
void deleteFile(char *name){
	if(remove(name)!=0){
		logEntry("deleteFile Unable to delete: ",name,Err);
	}
}
/*
* Function: purgeFiles
* Description:
* Calls the deleteFile function in order to delete the files. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
void purgeFiles(char *fileListString){
	char *name = (char*) malloc(sizeof(char)*MG);
		memset(name,'\0',MG);
	
	int i,j,k;
	i=j=k=0;
	
	for(i=0; i<strlen(fileListString); i++){
		if(fileListString[i]==';'){
			name[j]='\0';
			deleteFile(name);
			memset(name,'\0',MG);
			j=0;	
		}else{
			name[j++] = fileListString[i];		
		}
	}

	free(name);
	free(fileListString);
}
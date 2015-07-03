/*
* Contains all the helper functions that will help extract JSON data from weblink
* and create multple CSV files. Some functions also perform analysis so that a best fit
* can be found to buy content on a limited budget. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/

#include "bistro.h"
#include "time.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"



#define MAXLINE 2048
#define STR_MAX 128
#define MAXFILE 1024
#define SINGLE 1
#define MULTI 2
#define NOTFOUND -999
#define FOUND 1
#define MG 50
#define NG 20
#define FREE 0
#define NEED_I 0
#define NEED_J 1

#define max(a,b) a>=b ? a: b
#define min(a,b) a<b  ? a: b


int numOfVal=0;
int numOfFiles = 0;
char *firstEntry;
float bd;
int dot,prec=0;
char str_budget[10];

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
	if(argc==1 || argc>2){
		printf("Invalid number of Arguments \n\t Valid Arguments - ./appname BUDGET \n\t BUDGET (float/int)\n");
		exit(0);
	}else{		
		ret = validBudgetInput(argv[1]);
		return ret;
	}
}

/*
* Function: Toupper
* Description:
* Converts a string in Any case to Upper case. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *Toupper(char*strVal){
	int i=0; 
	static char tmpStr[NG];
	strcpy(tmpStr,strVal);
	while(tmpStr[i]){
		tmpStr[i] = toupper(tmpStr[i]);
		i++;
	}
	return(tmpStr);
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
			printf("Invalid Argument (BUDGET)\n\t Expected Format - number  \n");
			exit(0);
		}
	}	
	memset(str_budget,'\0',10);
	strcpy(str_budget,budStr);
	value = stringToFloat(budStr);
	if(value<0){
		printf("Budget value cannot be a negative, Try again\n");
		exit(0);
	}


	bd = value; 					//Store the Budget value in a Global Variable. 
	return value;
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
				 exit(0);
	}
}

/*
* Function: readURL
* Description:
* wget command is used to save the JSON data from the given 
* URL link. It saves the content in a txt file whose name is hard coded. 
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
*/
char *readURL(char* url){

    FILE *filePtr;
    char wgetMagic[STR_MAX];
    memset(wgetMagic,'\0',STR_MAX);
    
    strcpy(wgetMagic, "wget --quiet ");
    strcat(wgetMagic,url);
    strcat(wgetMagic," -O urlContent.txt");
    
    filePtr = popen(wgetMagic,"w");
    pclose(filePtr);
    return("urlContent.txt");
}

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
	FILE *filePtr;
	char temp;
	int i = 0,j, parendCalc =0;
	char *str = (char*) malloc(sizeof(char)*MAXLINE);
	filePtr = fopen(rawFile,"r");
	if(filePtr==NULL){
		printf("parseFile: FILE %s couldn't be opened\n", rawFile);
		exit(0);
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
    mm = (char*)malloc(sizeof(char)*10);
    dd = (char*)malloc(sizeof(char)*10); 
    yy = (char*)malloc(sizeof(char)*10);

    memset(mm,'\0',10);
    memset(yy,'\0',10);
   	memset(dd,'\0',10);
    
    
    snprintf(mm,sizeof(mm),"%d",m);
    snprintf(dd,sizeof(dd),"%d",d);
    snprintf(yy,sizeof(yy),"%d",y);

    strcat(yy,"-");
    strcat(mm,"-");
    strcat(mm,dd);
    strcat(yy,mm);

    return yy;
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
	for(i=0; i<numRec; i++){
		if(chosen[i]==NOTFOUND)
			continue;
		if(fnum[chosen[i]-1]>budget)
			continue;
		printf("%-40s $%.2f\n",n[chosen[i]-1],fnum[chosen[i]-1]);
	}
	//free(a_map);
	//free(b_map);
	//free(prim);
	//free(n);
	//free(fnum);
	//free(chosen);
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
        return;
    }
    chdir(dir);
    namesOfFiles = (char*)malloc(sizeof(char)*MAXLINE);
    	memset(namesOfFiles,'\0',MAXLINE);

    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(strMatch(entry->d_name,".csv")==FOUND){
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
	if (getcwd(cwd, sizeof(cwd)) == NULL)
       	printf("DirectoryExam: getcwd() error\n");
    else
    	return getFileNames(cwd);
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

	//Print the free Ones and the Closed on Duty ones
	if(strcmp(n[0],Toupper("Closed for the Holiday"))==0||strMatch(n[0],"Closed")==FOUND||strMatch(n[0],"Holiday")==FOUND){
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
			if(strcmp(p[i],"Free")==0||
				strcmp(p[i],"0")==0||
				strcmp(p[i],"0.0")==0||
				strcmp(p[i],"0.00")==0||
				strcmp(p[i],"free")==0
				){
				printf("%-40s Free\n",n[i]);
				continue;
			}else if(strlen(p[i])==1 || strlen(n[i])==1){
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
		//free(n);
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
	
	strcat(sysDate,".csv");
	readFile = fopen(sysDate,"r");
	
	char sample[MG];
		

	if(readFile == NULL){
		printf("makeDecision: Unable to read File %s\n",sysDate);
		exit(0);
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

    	str[x] = (char*)malloc(sizeof(char)*MG);
    	memset(str[x],'\0',MG);
    	snprintf(str[x],MG,"%s%s%s%s",csvVal(line,',',1),"-",csvVal(line,',',2),"-");
    	if(x==0){
    		firstEntry = (char*)malloc(sizeof(char)*MG);
    		memset(firstEntry,'\0',MG);
    		strcpy(firstEntry,str[x]);
    	}
    	
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
		memset(fileDate,'\0',MG);
		memset(name,'\0',MG);
	strcpy(fileDate,sysdate);
	strcat(fileDate,".csv");
	
	int i,j,k;
	i=j=k=0;
	
	for(i=0; i<strlen(namesOfFiles); i++){
		if(namesOfFiles[i]==';'){
			name[j]='\0';
			if(strcmp(fileDate,name)<=0){
				printf("\n");
				printf("Date #%s\n",csvVal(name,'.',1));
				makeDecision(csvVal(name,'.',1));	
				printf("____________________\n");				
			}	
			j=0;	
		}else{
			name[j++] = namesOfFiles[i];		
		}
	}
	free(fileDate);
	free(name);
}

/*
* Function: deleteFile
* Description:
* Deletes the file whose name is provided as an argument.
* @author: Srinivasan Rajappa
* Date: 2 July, 2015
* References: http://www.programmingsimplified.com/c-program-delete-file
*/
void deleteFile(char *name){
	if(remove(name)!=0)
	  printf("deleteFile: Unable to delete file %s\n",name);
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
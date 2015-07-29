#include "bistro.h"
#include "error.h"

char* logTime(){
   	time_t now = time(NULL);
   	struct tm *t = localtime(&now);
   	float milliSec = t->tm_sec;
   	milliSec/=1000;
   	char *stringDate = (char*)malloc(sizeof(char)*MG);
   		memset(stringDate,'\0',MG);
   	snprintf(stringDate,MG,"<%d-%d-%d:%d-%d-%d-%-0.3f>",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,milliSec);
   	return stringDate;
}


void refresh(int MODE){
	FILE *writeFD;
	writeFD = fopen(FILENAME,"a");
	if(MODE==START){
		fprintf(writeFD, "\n\nApplication Starts [%s]==================================================\n",logTime() );
	}else{
		fprintf(writeFD, "Application Stops  [%s]**************************************************\n",logTime() );
	}
	fflush(writeFD);
	close(writeFD);
}


void Exit(int i){
	FILE *writeFD;
	writeFD = fopen(FILENAME,"a");
	switch(i){
		case EXIT_IO : 				fprintf(writeFD, "EXIT: I/O Error\n");
					  				break;
		
		case EXIT_TRANS: 			fprintf(writeFD, "EXIT: System Error \n");
									break;
		
		case EXIT_APP:				fprintf(writeFD, "EXIT: Desirable exit from application\n");
									break;
		
		case EXIT_ABNORMAL:			fprintf(writeFD, "EXIT: Unexpected and abnormal value \n");
									LOG_MESSAGE;
									break;

		default:      				fprintf(writeFD, "Exit----> argument is incorrect at <error.c>\n");
	}
	fprintf(writeFD, "ft_main Stops [%s]**************************************************\n",logTime() );
	fflush(writeFD);
	close(writeFD);
	exit(i);
}

void logEntry(char *str,char *val,char mode){
	FILE *writeFD;
	writeFD = fopen(FILENAME,"a");
	fprintf(writeFD, "%-20s[%c]->%-20s<%s>\n",logTime(),mode,str,val);
	fflush(writeFD);
	close(writeFD);
}
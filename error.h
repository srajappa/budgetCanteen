/*
* Contains header files for enabling error checks and logging.
*/

#ifndef __error_h
#define __error_h

#include "stdarg.h"
#include "time.h"

#define START							0
#define STOP							999

#define EXIT_IO 						0
#define EXIT_TRANS 						1
#define EXIT_ABNORMAL 					2
#define EXIT_APP 						3

#define FILENAME 						"myAppLog.txt"

#define LOG_MESSAGE 					printf("Abnormal Exit: Check myAppLog.txt file")
#define WRONG_COMMAND 					printf("Incorrect Input: try -HELP command\n")
#define FILE_NOT_FOUND 					printf("Error: File not found\n")
#define NO_CONNECTION_TO_INTERNET 		printf("Unable to connect to Internet\n")

#define	Err								'E'
#define Info							'I'
#define Des								'D'
#define Prob							'P'


#endif
#ifndef __bistro_h
#define __bistro_h

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"


/*const char url[] = "http://codapi.zappos.biz/menus";*/

char *readURL(char*);
float validator(int, char*[]);
float validBudgetInput(char*);
float stringToFloat(char *);
int mapToInt(char);
void parseFile(char*);
void displayStack(struct Stack*);
void stack_push(char, struct Stack*);
char* date_extract(char*,char*);
void analyzeString(char *);
void parseFile(char *);
char* Toupper(char *);
char* directoryExam();
float stringToFloat(char *);
int mapToInt(char );
char* readURL(char* );
char* extract_val(int ,char *);
int strMatch(char *, char *);
int match(char *, char *, int );
char** extractValue(char *, char *, int );
void formatDate(char* );
void analyzeTrail(char *);
char* systemDate();
char* csvVal(char*,char , int );
void knapSackDone(char **, float *,int , float ,int );
char* getFileNames(char *);
void makeUse(char**, char**, int , float );
void makeDecision(char *);
void returnBestList(char *,char *);
void purgeFiles(char *);
void deleteFile(char *);
char *properFormat(char*);



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




#endif
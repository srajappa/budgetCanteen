#ifndef __bistro_h
#define __bistro_h

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"




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

#endif
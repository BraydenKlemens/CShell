#ifndef ISH_H
#define ISH_H

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include <math.h>
#include <sys/wait.h>

char** parseInput(char *input);
int hexToDec(char *string);
bool isHex(char *string);
int gcd(int num1, int num2);
int gcdCommand(char *arg1, char *arg2);
bool isNum(char *string);
bool checkBinary(char *arg1);
double B2D(char *arg1);
void argsCommand(char *input);
char *getStreamName(char **args);

#endif


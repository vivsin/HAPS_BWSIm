#ifndef CSECURITY_H
#define CSECURITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sys/time.h>	//! For Time difference in microseconds resolution
#include "limits.h"
#include <time.h>
#include <algorithm>
#include "aes.h"

using namespace std;

char * fGetUniversalID();
bool fVerifyPassword(char *uID, char *pass);
void encrypt (char e[] ) ;
void decrypt( char * ePtr ) ;

#endif // CSECURITY_H

#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include <tchar.h>
#include <afxsock.h>
#include "stdafx.h"
#include "Resource.h"
#include "targetver.h"
using namespace std;

#define _AFXDLL  
// #include <unistd.h>
#include <cstdlib>
#include <signal.h>


struct fileZip {
	string name;
	int size = 0;
	string sizeType;
};


fileZip getFileInfo(string fileInfo);
void readInputFile(string fileName, vector<fileZip>& file);
bool sendFile(CSocket& client, string fileName);
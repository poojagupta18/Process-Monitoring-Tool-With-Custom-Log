//
//  @Author: Pooja Gupta
//  @Date: 25/7/19
//  @Purpose: Provide Process Explorer Functionality
//

#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<windows.h>
#include<TlHelp32.h>
#include<tchar.h>
#include<conio.h>
#include<string>
#include<io.h>
#include<stdio.h>

using namespace std;


typedef struct LogFile   //typedef for a structure of logFile for writing process information into a log file
{
	char ProcessName[100];
	unsigned int pid;
	unsigned int ppid;
	unsigned int thread_cnt;
}LOGFILE;



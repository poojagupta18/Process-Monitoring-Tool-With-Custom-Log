#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<windows.h>
#include<TlHelp32.h>
#include <tchar.h>
#include<conio.h>

using namespace std;

typedef struct LogFile
{
	char ProcessName[100];
	unsigned int pid;
	unsigned int ppid;
	unsigned int thread_cnt;
}LOGFILE;



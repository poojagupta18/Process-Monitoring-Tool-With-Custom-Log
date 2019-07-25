#include "MyHeader.h"

class ThreadInfo
{
private:
	DWORD PID;
	HANDLE hThreadSnap;
	THREADENTRY32 te32;

public:
	ThreadInfo(DWORD);
	BOOL ThreadsDisplay();
};

/////////////////////////////////////////////////////////
//
//Name:				Constructor
//Input:			PID
//Output:			-
//Description:			Takes the snapshot of running Thread of
//				specified process
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

ThreadInfo::ThreadInfo(DWORD no)
{
	PID = no;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID); // Take a snapshot of all running threads  within specified pid

	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		cout << "Enable to create snapshot of current thread pool" << endl;
		return;
	}
	te32.dwSize = sizeof(THREADENTRY32);  // Fill in the size of the structure before using it. 
}

/////////////////////////////////////////////////////////
//
//Name:				ThreadsDisplay
//Input:			-
//Output:			-
//Description:			Displays the running Thread information
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

BOOL ThreadInfo::ThreadsDisplay()
{
	if (!Thread32First(hThreadSnap, &te32))	// Retrieve information about the first thread and exit if unsuccessful
	{
		cout << "ERROR in getting first thread" << endl;
		CloseHandle(hThreadSnap);                //To destroy the snapshot
		return FALSE;
	}

	cout << endl << "Thread of This Process:" << endl;
	do
	{
		if (te32.th32OwnerProcessID == PID)
		{
			cout << "\tTHREAD ID : " << te32.th32ThreadID << endl;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);

	return TRUE;
}

class DLLInfo
{
private:
	DWORD PID;
	MODULEENTRY32 me32;
	HANDLE hProcessSanp;

public:
	DLLInfo(DWORD);
	BOOL DependentDLLDisplay();
};

/////////////////////////////////////////////////////////
//
//Name:				Constructor
//Input:			PID
//Output:			-
//Description:			Takes the snapshot of running modules 
//				of specified process
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

DLLInfo::DLLInfo(DWORD no)
{
	PID = no;
	hProcessSanp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	if (hProcessSanp == INVALID_HANDLE_VALUE)
	{
		cout << "ERROR: Unable to create snapshot of current Module" << endl;
		return;
	}
	me32.dwSize = sizeof(MODULEENTRY32);

}

/////////////////////////////////////////////////////////
//
//Name:				DependentDLLDisplay
//Input:			-
//Output:			Boolean
//Description:			Displays the Dependent DLL Information 
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

BOOL DLLInfo::DependentDLLDisplay()
{
	if (!Module32First(hProcessSanp, &me32))
	{

		cout << "Failed to get DLL information" << endl;
		CloseHandle(hProcessSanp);
		return FALSE;

	}

	cout <<endl<< "Dependent DLL OF THIS PROCESS" << endl;
	do
	{
		cout <<endl<< "MODULE NAME :     " << me32.szModule;
	} while (Module32Next(hProcessSanp, &me32));

	CloseHandle(hProcessSanp);
	return TRUE;

}

class ProcessInfo
{
private:
	DWORD PID;
	DLLInfo *pdobj;
	ThreadInfo *ptobj;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
public:
	ProcessInfo();
	BOOL ProcessDisplay(char const *);
	BOOL ProcessLog();
	BOOL ReadLog(DWORD, DWORD, DWORD, DWORD);
	BOOL ProcessSearch(char *);
	BOOL KillProcess(char *);
};

/////////////////////////////////////////////////////////
//
//Name:				Constructor
//Input:			-
//Output:			-
//Description:			Takes the snapshot of running processes
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

ProcessInfo::ProcessInfo()
{
	ptobj = NULL;
	pdobj = NULL;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout << "ERROR : Unable to create snapshot of running processes" << endl;
		return;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
}

/////////////////////////////////////////////////////////
//
//Name:				ProcessDisplay
//Input:			option with ps command(-p / -t/ -d)
//Output:			Boolean
//Description:			Displays the Process Information based 
//				on provided  option
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

BOOL ProcessInfo::ProcessDisplay(char const * option)
{
	char arr[200];

	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "ERROR: In finding the First Process";
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		cout << endl << "------------------------------------------------";
		cout << endl << "Process Name : " << pe32.szExeFile;
		cout << endl << "PID : " << pe32.th32ProcessID;
		cout << endl << "PPID : " << pe32.th32ParentProcessID;
		cout << endl << "No of Threads : " << pe32.cntThreads;

		if ((_stricmp(option, "-a") == 0) ||
			(_stricmp(option, "-d") == 0) ||
			(_stricmp(option, "-t") == 0))
		{
			if ((_stricmp(option, "-t") == 0 || _stricmp(option, "-a") == 0))
			{
				ptobj = new ThreadInfo(pe32.th32ProcessID);
				ptobj->ThreadsDisplay();
				delete ptobj;
			}
			if ((_stricmp(option, "-d") == 0 || _stricmp(option, "-a") == 0))
			{
				pdobj = new DLLInfo(pe32.th32ProcessID);
				pdobj->DependentDLLDisplay();
				delete pdobj;
			}
		}
		cout << endl << "------------------------------------------------";
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return TRUE;
}

/////////////////////////////////////////////////////////
//
//Name:				ProcessLog
//Input:			-
//Output:			Boolean
//Description:			Create the log File and write process 
//				information in it
//Date:				25 July 2019
//
///////////////////////////////////////////////////////////

BOOL ProcessInfo::ProcessLog()
{
	const char* month[] = { "JAN", "FEB", "MAR", "APR","MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
	char FileName[100], arr[512];
	int ret = 0, fd = 0, count = 0;
	SYSTEMTIME It;
	LOGFILE fobj;
	FILE *fp;

	GetLocalTime(&It);

	sprintf(FileName, "C:\\Users\\sonug\\Desktop\\ProcMon\\LogFile_%02d_%02d_%02d %s.txt", It.wHour, It.wMinute, It.wDay, month[It.wMonth - 1]);
	
	fp = fopen(FileName, "wb");

	if (fp == NULL)
	{
		return FALSE;	
	}
	else
	{
		cout << "Log file created successfully as :" <<FileName<< endl;
		cout << "Time of log file creation is ->" << It.wHour << ":" << It.wMinute << ":" << It.wDay << "th " << month[It.wMonth - 1] << endl;
	}
	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "ERROR: In finding the First process." << endl;
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		strcpy_s(fobj.ProcessName, pe32.szExeFile);
		fobj.pid = pe32.th32ProcessID;
		fobj.ppid = pe32.th32ParentProcessID;
		fobj.thread_cnt = pe32.cntThreads;
		fwrite(&fobj, sizeof(fobj), 1, fp);
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	fclose(fp);
	return TRUE;

}

/////////////////////////////////////////////////////////
//
//Name:				ReadLog
//Input:			Hour, Minute, Date, Month
//Output:			Boolean
//Description:			Read the contents of log file
//Date:				25 July 2019
//
///////////////////////////////////////////////////////////

BOOL ProcessInfo::ReadLog(DWORD hr, DWORD min, DWORD date, DWORD month)
{
	char FileName[100];
	const char* montharr[] = { "JAN", "FEB", "MAR", "APR","MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

	int ret = 0, count = 0;
	LOGFILE fobj;
	FILE *fp;

	sprintf(FileName, "C:\\Users\\sonug\\Desktop\\ProcMon\\LogFile_%02d_%02d_%02d %s.txt", hr, min, date,montharr[month]);

	fp = fopen(FileName, "rb");

	if (fp == NULL)
	{
		cout << "ERROR : Unable to open log file named as : " << FileName << endl;
		return FALSE;
	}
	cout <<"Opening a file :" << FileName << endl;
	while (ret = fread(&fobj, 1, sizeof(fobj), fp) != 0)
	{
		cout << "----------------------------------------------------" << endl;
		cout << "Process Name : " << fobj.ProcessName << endl;
		cout << "PID of current Process : " << fobj.pid << endl;
		cout << "Parent process PID : " << fobj.ppid << endl;
		cout << "Thread count of process:" << fobj.thread_cnt << endl;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////
//
//Name:				ProcessSearch
//Input:			Process_Name
//Output:			Boolean
//Description:			Search the specified process and Display 
//				it's information
//Date:				25 July 2019
//
///////////////////////////////////////////////////////////

BOOL ProcessInfo::ProcessSearch(char* name)
{
	BOOL flag = FALSE;

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return flag;
	}
	do
	{
		if (_stricmp(pe32.szExeFile, name) == 0)
		{
			cout << endl << "Process Name : " << pe32.szExeFile;
			cout << endl << "Process ID : " << pe32.th32ProcessID;
			cout << endl << "Parent Process ID : " << pe32.th32ParentProcessID;
			cout << endl << "Thraed count : " << pe32.cntThreads;
			flag = TRUE;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return flag;
}

/////////////////////////////////////////////////////////
//
//Name:				KillProcess
//Input:			Process_Name
//Output:			Boolean
//Description:			Terminate the specified process
//Date:				25 July 2019
//
///////////////////////////////////////////////////////////

BOOL ProcessInfo::KillProcess(char * name)
{
	int pid = -1;
	BOOL bRet;

	HANDLE hprocess;

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		if (_stricmp(pe32.szExeFile, name) == 0)
		{
			pid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	if (pid == -1)
	{
		return FALSE;
	}

	hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

	if (hprocess == NULL)
	{
		cout << " ERROR : Their is no access to terminate." << endl;
		return FALSE;
	}
	bRet = TerminateProcess(hprocess, 0);

	if (bRet == FALSE)
	{
		cout << "ERROR : Unable to terminate process.";
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////
//
//Name:				HardWareInfo
//Input:			-
//Output:			Boolean
//Description:			It Displays the Hardware information
//Date:				25 July 2019
//
////////////////////////////////////////////////////////

BOOL HardWareInfo()
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);

	
	cout << endl << "------------Hardware information of current System------------:" << endl;
	cout << "OEM ID: (Original Equipment Manifacturers) : " << siSysInfo.dwOemId << endl;
	cout << "Number of processors : " << siSysInfo.dwNumberOfProcessors << endl;
	cout << "Page size : " << siSysInfo.dwPageSize << endl;
	cout << "Processor Type : " << siSysInfo.dwProcessorType << endl;
	cout << "Minimum application address : " << siSysInfo.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address : " << siSysInfo.lpMaximumApplicationAddress << endl;
	cout << "Active Processor mask : " << siSysInfo.dwActiveProcessorMask << endl;

	return TRUE;
}


/////////////////////////////////////////////////////////
//
//Name:				DisplayHelp
//Input:			-
//Output:			-
//Description:			It displays the command and it's usage
//Date:				25 July 2019
//
///////////////////////////////////////////////////////////

void DisplayHelp()
{
	cout << endl;
	cout << "ps           : Display all information of process" << endl;
	cout << "ps -t        : Display all the information about threads" << endl;
	cout << "ps -d        : Display all the information about DLL" << endl;
	cout << "cls          : Clears the contents on Console" << endl;
	cout << "log          : Create the log of current running processes" << endl;
	cout << "readlog      : Display the information from specified log file" << endl;
	cout << "sysinfo      : Displays Hardware configuration" << endl;
	cout << "search Process_Name  : Search and displays information of specific process" << endl;
	cout << "kill Process_Name    : Terminate the specified process" << endl;
	cout << "exit         : Terminate the ProcMon shell" << endl;
}

/////////////////////////////////////////////////////////
//
//Name:				main
//Input:			-
//Output:			Integer
//Description:			Entry point function, which calls other 
//				Functionalities from it
//Date:				24 July 2019
//
///////////////////////////////////////////////////////////

int main()
{
	int count = 0;
	BOOL bRet = FALSE;
	ProcessInfo * pobj = NULL;
	char str[80];
	char command[4][80];
	int min, date, Month, hr;
	char mon[5];

	while (1)
	{
		strcpy_s(str, "");

		cout << endl << "ProcMon : > ";
		fflush(stdin);
		fgets(str, 80, stdin);

		count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		if (count == 1)
		{
			if (_stricmp(command[0], "ps") == 0)
			{
				pobj = new ProcessInfo();

				bRet = pobj->ProcessDisplay("-a");

				if (bRet == FALSE)
				{
					cout << "ERROR : Unable to display the process."<<endl;
				}

				delete pobj;
				continue;
			}
			else if (_stricmp(command[0], "log") == 0)
			{
				pobj = new ProcessInfo();
				bRet = pobj->ProcessLog();

				if (bRet == FALSE)
				{
					cout << "ERROR: Unable to create log file" << endl;
				}
				delete pobj;
				continue;
			}
			else if (_stricmp(command[0], "readlog") == 0)
			{
				
				pobj = new ProcessInfo();

				const char* month[] = { "JAN", "FEB", "MAR", "APR","MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

				cout << "Enter log file details as:" << endl;

				cout << "Hour : ";
				cin >> hr;
				cout << endl << "Minute : ";
				cin >> min;
				cout << endl << "Date : ";
				cin >> date;
				cout << endl << "Month : ";
				strcpy_s(mon, "");

			
				scanf("%s", mon);
				getchar();

				for (int i = 0; i < 12; i++)
				{
					if (_stricmp(mon, month[i]) == 0)
					{
						Month = i;
						break;
					}
				}

				bRet = pobj->ReadLog(hr, min, date, Month);

				if (bRet == FALSE)
				{
					cout << "ERROR : Unable to read specified log file" << endl;
				}

				delete pobj;
				continue;
			}
			else if (_stricmp(command[0], "sysinfo") == 0)
			{
				bRet = HardWareInfo();

				if (bRet != TRUE)
				{
					cout << "ERROR : Unable to get Hardware Information" << endl;
				}
				continue;
			}
			else if (_stricmp(command[0], "help") == 0)
			{
				DisplayHelp();
				continue;
			}
			else if (_stricmp(command[0], "clear") == 0)
			{
				system("cls");
				continue;
			}
			else if (_stricmp(command[0], "exit") == 0)
			{
				cout << endl << "Terminating a ProcMon.....";
				break;
			}
			else
			{
				cout << endl << "ERROR : Invalid Command..." << endl;
			}
		}
		else if (count == 2)
		{
			if (_stricmp(command[0], "ps") == 0)
			{
				pobj = new ProcessInfo();
				bRet = pobj->ProcessDisplay(command[1]);
				if (bRet == FALSE)
				{
					cout << "ERROR : Their is no such process." << endl;
				}

				delete pobj;
				continue;
			}
			else if (_stricmp(command[0], "search") == 0)
			{
				pobj = new ProcessInfo();
				bRet = pobj->ProcessSearch(command[1]);

				if (bRet == FALSE)
				{
					cout << "ERROR : Their is no such process";
				}

				delete pobj;
				continue;
			}
			else if (_stricmp(command[0], "kill") == 0)
			{
				pobj = new ProcessInfo();
				bRet = pobj->KillProcess(command[1]);

				if (bRet == FALSE)
				{
					cout << "ERROR : There is no such process" << endl;
				}
				else
				{
					cout << command[1] << " Terminated successfully..."<<endl;
				}
				delete pobj;
				continue;
				
			}
			else
			{
				cout << endl <<"ERROR : Invalid Command..." << endl;
			}
		}
		else
		{
		cout << endl << "ERROR : Command not found!!!" << endl;
		}
		
	}
	_getch();
	return 0;
}





















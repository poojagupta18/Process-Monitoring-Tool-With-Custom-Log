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


BOOL ThreadInfo::ThreadsDisplay()
{
	if (!Thread32First(hThreadSnap, &te32))	// Retrieve information about the first thread and exit if unsuccessful
	{
		cout << "Error in getting first thread" << endl;
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

DLLInfo::DLLInfo(DWORD no)
{
	PID = no;
	hProcessSanp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	if (hProcessSanp == INVALID_HANDLE_VALUE)
	{
		cout << "Error: Unable to create snapshot of current Module" << endl;
		return;
	}
	me32.dwSize = sizeof(MODULEENTRY32);

}

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

BOOL ProcessInfo::ProcessDisplay(char const * option)
{
	char arr[200];

	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "Error: In finding the First Process";
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do
	{
		cout << endl << "------------------------------------------------";
		//wcstombs_s(NULL, arr, 200, pe32.szExeFile, 200);
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

BOOL ProcessInfo::ProcessLog()
{

}

int main()
{
	int count = 0;
	BOOL bRet = FALSE;
	ProcessInfo * pobj = NULL;
	char str[80];
	char command[4][80];

	while (1)
	{
		fflush(stdin);

		strcpy_s(str, "");

		cout << endl << "ProcMon : > ";
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
					cout << "Error : Unable to display the process."<<endl;
				}

				delete pobj;
				continue;
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
					cout << "Error : Their is no such process." << endl;
				}

				delete pobj;
				continue;
			}
		}

	}

	cout << "Hello";
	_getch();
	return 0;
}





















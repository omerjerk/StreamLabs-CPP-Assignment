#undef _UNICODE

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#include <iostream>

#include "Messages.h"

using namespace std;

#define BUFSIZE 512

int connectToServer(LPCTSTR lpszPipename, HANDLE &hPipe) {
	BOOL   fSuccess = FALSE;
	DWORD  dwMode;
	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

	  // Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			printf("Could not open pipe. GLE=%d\n", GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		printf("SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
		return -1;
	}
}

int sendMessageToServer(HANDLE hPipe, LPCTSTR lpvMessage) {
	DWORD cbToWrite, cbWritten, cbRead;
	BOOL fSuccess = FALSE;
	TCHAR  chBuf[BUFSIZE];

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	printf("Sending %d byte message\n", cbToWrite);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		printf("WriteFile to pipe failed. GLE=%d\n", GetLastError());
		return -1;
	}

	cout << "\n======================================================" << endl;
	cout<<"Reply from server:\n";

	do
	{
		// Read from the pipe.
		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;
		printf("\"%s\"\n", chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	cout << "======================================================\n" << endl;

	if (!fSuccess)
	{
		printf("ReadFile from pipe failed. GLE=%d\n", GetLastError());
		return -1;
	}
}

int _tmain(int argc, TCHAR * argv[])
{
	HANDLE hPipe;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	int mode;
	cout << "Please select the server mode from below:" << endl;
	cout << "1. Synchornous" << endl;
	cout << "2. Asynchronous" << endl;
	cin >> mode;
	if (mode != 1 && mode != 2) {
		cout << "Invalid mode selected" << endl;
		exit(0);
	}

	if (mode != 1) {
		cout << "Asunchronous not implemented yet" << endl;
		exit(0);
	}

	connectToServer(lpszPipename, hPipe);

	cout << "Connected to the server." << endl;

	while (true) {
		// Send a message to the pipe server.
		int msgType = -1;
		cout << "Please select the type of message to send or 0 to exit:" << endl;
		cout << "0: Exit" << endl;
		cout << "1: Send string/number" << endl;
		cout << "2: Register a custom class" << endl;
		cout << "3: Create an object" << endl;
		cout << "4: Print all objects of a class" << endl;
		cin >> msgType;
		if (msgType < 0 || msgType > 4) {
			cout << "Invalid msg type. Please select correct message type\n\n";
			continue;
		}

		string msg;

		switch (msgType) {
		case 0:
			return 0;
		case 1: {
			string str;
			cout << "Enter number/string to send to the server:" << endl;
			cin >> str;
			msg = Messages::getStringMessage(str);
			break;
		}
		case 2: {
			string className;
			cout << "Enter the class name to register at the server:" << endl;
			cin >> className;
			msg = Messages::getCreateClassMessage(className);
			break;
		}
		case 3: {
			string className;
			string jsonobj;
			cout << "Enter the name of the class for which to create the objects:" << endl;
			cin >> className;
			cout << "Enter object in json format:" << endl;
			cin >> jsonobj;
			msg = Messages::getCreateObjMessage(className, jsonobj);
			break;
		}
		default:
			cout << "Message type invalid or not supported." << endl;
			continue;
		}

		if (sendMessageToServer(hPipe, (const TCHAR*) msg.c_str()) == -1) {
			return -1;
		}

	}

	_getch();

	CloseHandle(hPipe);

	return 0;
}
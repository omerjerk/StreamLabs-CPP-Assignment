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
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
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
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}
}

int sendMessageToServer(HANDLE hPipe, LPCTSTR lpvMessage) {
	DWORD cbToWrite, cbWritten, cbRead;
	BOOL fSuccess = FALSE;
	TCHAR  chBuf[BUFSIZE];

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message\n"), cbToWrite);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

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

		_tprintf(TEXT("\"%s\"\n"), chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
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

		wstring msg;

		switch (msgType) {
		case 0:
			return 0;
		case 1: {
			wstring str;
			cout << "Enter number/string to send to the server" << endl;
			wcin >> str;
			msg = Messages::getStringMessage(str);
			wcout << L"sending message : " << msg << endl;
			break;
		}
		default:
			cout << "Message type invalid or not supported." << endl;
			continue;
		}

		if (sendMessageToServer(hPipe, msg.c_str()) == -1) {
			return -1;
		}

	}

	_getch();

	CloseHandle(hPipe);

	return 0;
}
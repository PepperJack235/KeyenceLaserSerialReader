#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <ctime>
#include<sstream>
#include<fstream>
using namespace std;

LPCWSTR   ComPortName = L"\\\\.\\COM11"; // Name of the Serial port(May Change) to be opened,
char testval[] = "SR,00,038\r\n"; //sets the command to be sent to the serial port

string readvalue;
string reading = "";
string filename;
int testamount;
string readorquit;
HANDLE hComm;                          // Handle to the Serial port
BOOL   Status;
DWORD dwEventMask;                     // Event mask to trigger
char  TempChar;                        // Temperory Character
char SerialBuffer[256];               // Buffer Containing Rxed Data
DWORD NoBytesRead;                     // Bytes read by ReadFile()
int i = 0;
int z = 0;                            // variable for the loop responsible for starting a new line after test repeats

void writeSerial(char lpBuffer[]) {
	DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port

	dNoOFBytestoWrite = strlen(lpBuffer); // How many characters will 

	Status = WriteFile(hComm,               // Handle to the Serialport
		lpBuffer,            // Data to be written to the port 
		dNoOFBytestoWrite,
		&dNoOfBytesWritten,  // How many characters written to port
		NULL);

	if (Status == TRUE)
		printf("\n\n    %s - Written to %s", lpBuffer, ComPortName);
	else
		printf("\n\n   Error %d in Writing to Serial Port", GetLastError());

}

string readSerial() {
	i = 0; //fixed the SerialBuffer expanding infinitely

	/*------------------------------------ Setting Receive Mask ----------------------------------------------*/

	Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

	if (Status == FALSE)
		printf("\n\n    Error! in Setting CommMask");
	else
		printf("\n\n    Setting CommMask successfull");


	/*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

	printf("\n\n    Waiting for Data Reception");


	Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

	/*-------------------------- Program will Wait here till a Character is received ------------------------*/


	if (Status == FALSE)
	{
		printf("\n    Error! in Setting WaitCommEvent()");
	}
	else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
	{

		readvalue = ""; //resets readvalue
		do
		{

			Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
			SerialBuffer[i] = TempChar;  //writes the number in the ith array spot
			i++;

		} while (NoBytesRead > 0); //i holds index for one character beyond last meaningful characters in array


		SerialBuffer[i] = 0;
		printf("\n\n    Characters Received	\n\n"); //next line, next line, characters received
		printf("%s", SerialBuffer);         // prints the entire string of data at once without looping
		printf("\n\n");
	}
	string readvalue(SerialBuffer);
	return readvalue;
}

int main()
{
	hComm = CreateFile(ComPortName,                       // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
		0,                                 // No Sharing, ports cant be shared
		NULL,                              // No Security
		OPEN_EXISTING,                     // Open existing port only
		0,                                 // Non Overlapped I/O
		NULL);                             // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
		printf("\n   Error! - Port %s can't be opened", ComPortName);
	else
		printf("\n   Port %s Opened\n ", ComPortName);


	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

	DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

	if (Status == FALSE)
		printf("\n   Error! in GetCommState()");

	dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;      // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

	if (Status == FALSE)
	{
		printf("\n   Error! in Setting DCB Structure");
	}
	else
	{
		printf("\n   Setting DCB Structure Successfull\n");
		printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
		printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
		printf("\n       StopBits = %d", dcbSerialParams.StopBits);
		printf("\n       Parity   = %d", dcbSerialParams.Parity);
	}

	/*------------------------------------ Setting Timeouts --------------------------------------------------*/

	COMMTIMEOUTS timeouts = { 0 };

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE)
		printf("\n   Error! in Setting Time Outs");
	else
		printf("\n\n   Setting Serial Port Timeouts Successfull   \n \n");

	/*-------------------------------User input---------------------------------*/

	ofstream file;
	cout << " Enter the filename you wish to write, then press enter. \n Do not include the extension, spaces, or any non-alphanumeric characters. \n If the file name is already taken, that file will be overwritten. \n";

	cin >> filename;					//stores user input as string filename
	file.open(filename + ".csv");			//creates a .csv file with the name specified by the user.

	cout << "Enter the number of measurements you will be taking per test \n";
	cin >> testamount;


	cout << "\nThe device is now ready to read, press r, then enter to record values. When you are done recording, you may press q then enter or simply close the program. \n\n";
	cout << "Press x then enter to mark the previous data point as invalid. \nThe point after it will have text stating the prior point is invalid.\n";
	do {

		cin >> readorquit;
		if (readorquit == "r") //if the user only presses enter without typing anything, then do the read and store it in the file
		{
			writeSerial(testval);		//sends testval to the serial device
			reading = readSerial();		//sets readvalue equal to the output from the serial device
			reading.erase(reading.length() - 3, reading.length());  //removes the \r\n\n from the string, which makes it misbehave in a csv
			reading.erase(0, reading.length() - 7); //removes irrevelant data "SR,00" at beginning of laser data
			file << reading;			//places readvalue into the file
			file << ",";				//separates the entries with a comma
			z++;
			if (z >= testamount) //if the amount of measurements we want to take per 
			{
				file << "\n"; // starts a new line
				z = 0;
			}
		}
		else if (readorquit == "q") //if the user presses q and wants to stop data collection
		{
			file.close();			//closes the file being written on
			CloseHandle(hComm);     //Closes the Serial Port

		}
		else if (readorquit == "x")
		{
			file << "Disregard value prior to this";
		}
		else //if the user inputs something that isn't one of the commands
		{
			cout << "Unrecognized command";

		}
	} while (readorquit != "q");
}
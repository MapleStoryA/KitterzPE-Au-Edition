#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <string>
#include <Commctrl.h>
using namespace std;

#include "Resource.h"
#include "Send.h"
#include "Bin.h"
#include "Base.h"

//---------------------------------------------------------------------------

string& replaceAll(string& context, const string& from, const string& to) 
{
	size_t lookHere = 0;
	size_t foundHere;
	while((foundHere = context.find(from, lookHere)) != string::npos) 
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
	return context;
}

//---------------------------------------------------------------------------

int HexStrToInt( const char* hexStr )
{
	int hexInt;
	sscanf( hexStr, "%x", &hexInt );
	return hexInt;
} 

//---------------------------------------------------------------------------

string ToStringArray (LPVOID PacketData, INT Length)
{
	BYTE* Bytes = (BYTE*)PacketData;
	string Packet = "";
	char Buf [20];

	for (int i = 0; i < Length; i++)
	{
		sprintf (Buf, "%02X", Bytes [i]);
		Packet += Buf;
		if (i != (Length - 1))
			Packet += " ";
	}

	return Packet;
}

//---------------------------------------------------------------------------

string ReverseBytes (string StringBytes)
{
	char Tmp [100];
	int Counter = StringBytes.length();

	for (unsigned int i = 0; i < StringBytes.length(); i+= 2)
	{
		Tmp [i] = StringBytes [Counter - 2];
		Tmp [i + 1] = StringBytes [Counter - 1];
		Counter -= 2;
	}

	Tmp [StringBytes.length()] = 0;
	return Tmp;
}

//---------------------------------------------------------------------------

char* ToBytesArray (string HexString)
{
	replaceAll(HexString, " ", "");

	char* SendingBuffer = (char*)malloc (HexString.length() / 2);

	for (unsigned int i = 0; i < HexString.length() / 2; i++)
		SendingBuffer [i] = strtol (HexString.substr(i * 2, 2).c_str(), 0, 16);  

	return SendingBuffer;
}

//---------------------------------------------------------------------------

string ToSpam = "";
int SleepTime = 1000;
extern BOOL SpamOn;
extern HWND hSend;

void SpamThis ()
{
	while (SpamOn)
	{
		SendPacket (ToSpam);
		Sleep (SleepTime);
	}
}

void SpamPacket (string Packet)
{
	char buf [60];
	GetDlgItemText (hSend, IDC_DELAY, buf, 1024);

	SleepTime = atoi (buf);
	ToSpam = Packet;
	
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SpamThis, 0, 0, NULL);
}

//---------------------------------------------------------------------------

PACKETCHANGE GetChange (string Change)
{
	PACKETCHANGE tmp;
	tmp.PacketID = HexStrToInt (Change.substr (0, 2).c_str());
	tmp.Change = Change.substr (5, 1).c_str();

	if (tmp.Change == "R")
	{
		tmp.Length = atoi (Change.substr (Change.length() - 2, 2).c_str());
		tmp.Packet = Change.substr (9, tmp.Length * 2).c_str();
	}
	else
	{
		tmp.Length = 0;
		tmp.Packet = "";
	}

	return tmp;
}

//---------------------------------------------------------------------------
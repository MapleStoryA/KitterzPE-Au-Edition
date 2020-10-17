#define JMP(frm, to) (int)(((int)to - (int)frm) - 5);
#include <windows.h>
#include <string>
#include <Commctrl.h>
using namespace std;

#include "Packet.h"
#include "Search.h"
#include "Send.h"
#include "Bin.h"

//-------------------------------------------------------------------------

SENDPACKET SendPacketAPI;
SendStruct* DataStructSend;

DWORD ClassAddy = NULL;
DWORD SendAddyReal = NULL;
DWORD MovVal = NULL;

//-------------------------------------------------------------------------

void SendPacket(string Bytes)
{
	PACKET Packet;
//	Packet.Unknown1 = 0;
//	Packet.Unknown2 = 0;
	//Packet.Data = ToBytesArray (Bytes);
	Packet.Size = (Bytes.length() / 2);

	_asm 
	{
		mov ecx,ClassAddy
		mov ecx,[ecx]
	}

	SendPacketAPI(&Packet);
}

//---------------------------------------------------------------------------

void RecordPacketSend ()
{
	PacketEdit* PE = new PacketEdit;

	PE->isSend = TRUE;
	PE->SendStructP = DataStructSend;

	PE->AddToTreeView (hTreeSend);
	PE->CheckChanges();

	delete PE;
}

//---------------------------------------------------------------------------

void __declspec(naked) __stdcall SendHook () 
{
	_asm 
	{
		mov dword ptr [DataStructSend],esp
		pushad
		call [RecordPacketSend]
		popad
		mov eax,[MovVal]
		jmp [SendAddyReal]
	}
}

//---------------------------------------------------------------------------

void GetPacketAddysSend() 
{
	DWORD SendAddy = 0;
	MovVal = 0;
	ClassAddy = 0;

	if ((SendAddy == NULL) || (ClassAddy == NULL))
		MessageBox (0, "SendAddy or ClassAddy is NULL",0,0);

	SendPacketAPI = (SENDPACKET)(SendAddy + 5);
	SendAddyReal = (SendAddy + 5);
}

void PacketSendHook ()
{
	DWORD SendAddy = 0;
	MovVal = 0;
	ClassAddy = 0;

	if ((SendAddy == NULL) || (ClassAddy == NULL))
		MessageBox (0, "SendAddy or ClassAddy is NULL",0,0);

	SendPacketAPI = (SENDPACKET)(SendAddy + 5);
	SendAddyReal = (SendAddy + 5);

	//Protect
	DWORD oldprotection;
	VirtualProtect((LPVOID)SendAddy, 5, PAGE_EXECUTE_READWRITE, &oldprotection);

	//Hook
	*(BYTE*)SendAddy = 0xE9;
	*(DWORD*)(SendAddy + 1) = JMP(SendAddy, SendHook);
}

//---------------------------------------------------------------------------
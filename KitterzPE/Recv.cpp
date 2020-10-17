#define JMP(frm, to) (int)(((int)to - (int)frm) - 5);
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <string>
#include <Commctrl.h>
using namespace std;

#include "Resource.h"
#include "Packet.h"
#include "Search.h"
#include "Recv.h"
#include "Bin.h"
#include "Send.h"
#include "MemoryEdit.h"

//-------------------------------------------------------------------------

extern HWND hRecv;
PACKET* DataStructRecv;
DWORD RecvAddy = 0x00930EE4;

//-------------------------------------------------------------------------

void RecordPacketRecv ()
{
	PacketEdit* PE = new PacketEdit;
	SendStruct* Temp = new SendStruct; 

	Temp->ReturnAddy = NULL;
	Temp->Data = DataStructRecv;
	Temp->Data->Size = Temp->Data->Size - 2;

	PE->isSend = FALSE;
	PE->SendStructP = Temp;

	PE->AddToTreeView (hTreeRecv);
	PE->CheckChanges();
	
	delete PE;
}

//-------------------------------------------------------------------------

void __declspec(naked) __stdcall RecvHook()
{
	__asm {
		mov DataStructRecv, ecx
		pushad
		call RecordPacketRecv
		popad
		push 0x931BF0
		ret
	}
}


void PacketRecvHook ()
{
	MemoryEdit::hookCall((BYTE*)RecvAddy, (DWORD)RecvHook);
}

//-------------------------------------------------------------------------
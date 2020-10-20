#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <string>
#include <Commctrl.h>
#include <fstream>
using namespace std;

#include "Packet.h"
#include "Send.h"
#include "Resource.h"
#include "Bin.h"

//---------------------------------------------------------------------------

HTREEITEM NodesSend_ID [MAX_NODES];
HTREEITEM NodesRecv_ID [MAX_NODES];
string NodesSend_String [MAX_NODES];
string NodesRecv_String [MAX_NODES];

int NodesSend_Num = 0;
int NodesRecv_Num = 0;

extern HWND hSend, hRecv;

//---------------------------------------------------------------------------

void PacketEdit::Replace (BYTE* Bytes, INT Length)
{
	DWORD TempAddress = NULL;

	SendStructP->Data->Data = (byte*)malloc(Length);
	SendStructP->Data->Size = Length;
	TempAddress = (DWORD)SendStructP->Data->Data;

	for (int i = 0; i < Length; i++)
	{
		*(BYTE*)TempAddress = Bytes [i];
		TempAddress++;
	}
}

void PacketEdit::Replace (string ToReplace, INT Length)
{
	DWORD TempAddress = NULL;
	int TempCounter = 0;

	BYTE * Bytes = (BYTE*)malloc(Length);
	TempAddress = (DWORD)Bytes;

	for (unsigned int i = 0; i <  ToReplace.length(); i+=2)
	{
		string Buffer = ToReplace.substr(i, 2 ).c_str();
		int bb = (unsigned char)strtol (Buffer.c_str(), 0, 16); 
		*(BYTE*)TempAddress = bb;
		TempAddress++;
	}

	SendStructP->Data->Data = (byte*)malloc(Length);
	SendStructP->Data->Size = Length;
	TempAddress = (DWORD)SendStructP->Data->Data;

	for (int i = 0; i < Length; i++)
	{
		*(BYTE*)TempAddress = Bytes [i];
		TempAddress++;
	}
}

//---------------------------------------------------------------------------

void PacketEdit::Block ()
{
	BYTE Nothing [5] = "\xFF\x00\x00\x00";
	Replace (Nothing, 4);
}

//---------------------------------------------------------------------------

void PacketEdit::CheckChanges ()
{
	string Packet = "";
	char Buffer[1000];

	if (isSend)
	{
		Packet = ToStringArray (SendStructP->Data->Data, SendStructP->Data->Size);
		short PacketID = HexStrToInt (Packet.substr (0, 2).c_str());

		//Replace
		int NumChanges = SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETCOUNT, 0, 0);
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "R"))
			{
				Replace (pc.Packet, pc.Length);
				return;
			}
		}

		//Blocked
		NumChanges = SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETCOUNT, 0, 0);
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "B"))
			{
				Block();
				return;
			}
		}
	}
	else
	{
		Packet = ToStringArray(&SendStructP->Data->Type, SendStructP->Data->Size);

		//Don't Log
		short PacketID = SendStructP->Data->Type;

		//Replace
		int NumChanges = SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETCOUNT, 0, 0);
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "R"))
			{
				Replace (pc.Packet, pc.Length);
				return;
			}
		}

		//Blocked
		NumChanges = SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETCOUNT, 0, 0);
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "B"))
			{
				Block();
				return;
			}
		}
	}
}

//---------------------------------------------------------------------------

HTREEITEM PacketEdit::AddItem (TV_INSERTSTRUCT tvinsert, HWND hWnd)
{
	return TreeView_InsertItem (hWnd, &tvinsert);
}

//---------------------------------------------------------------------------

HTREEITEM PacketEdit::GetTreeItemByText (string ToSearch)
{
	if (isSend)
	{
		for (int i = 0; i < NodesSend_Num; i++)
		{
			if (strcmp (NodesSend_String [i].c_str(), ToSearch.substr(0, 13).c_str()) == 0)
				return NodesSend_ID [i];
		}
	}
	else
	{
		for (int i = 0; i < NodesRecv_Num; i++)
		{
			if (strcmp (NodesRecv_String [i].c_str(), ToSearch.c_str()) == 0)
				return NodesRecv_ID [i];
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------

BOOL PacketEdit::ItemRootExists (string ToCheck)
{
	if (isSend)
	{
		for (int i = 0; i < NodesSend_Num; i++)
		{
			if (strcmp (NodesSend_String [i].c_str(), ToCheck.substr(0, 13).c_str()) == 0)
				return TRUE;
		}
	}
	else
	{
		for (int i = 0; i < NodesRecv_Num; i++)
		{
			if (strcmp (NodesRecv_String [i].c_str(), ToCheck.c_str()) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

//---------------------------------------------------------------------------

void PacketEdit::SavePacket (string Packet)
{
	BYTE PacketID = HexStrToInt (Packet.substr (0, 2).c_str());

	if ((PacketID == 0xD4) || (PacketID == 0xB2))
	{
		if (isSend)
		{
			ofstream File ("Send.txt", ofstream::app);
			File << Packet << "\n";
			File.close();
		}
		else
		{
			ofstream File ("Recv.txt", ofstream::app);
			File << Packet << "\n";
			File.close();
		}
	}

}



void PacketEdit::AddToTreeView (HWND hTree)
{
	string Packet = "";
	char Buffer[60];

	if (isSend)
	{
		Packet = ToStringArray (&SendStructP->Data->Data, SendStructP->Data->Size);
		

		//Don't Log
		short PacketID = HexStrToInt (Packet.substr (0, 2).c_str());
		int NumChanges = SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETCOUNT , 0, 0);
		
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hSend, IDC_CHANGESLIST), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "D"))
				return;
		}

		//Create Header
		sprintf (Buffer, "%08X", SendStructP->ReturnAddy);
		string ReturnAddy = Buffer;
		string All = ReturnAddy + " - " + Packet.substr (0, 2);

		//If already exists in a TreeView
		if (!ItemRootExists (All))
		{
			TV_INSERTSTRUCT tvinsert;

			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.mask = TVIF_TEXT; 
			tvinsert.item.pszText = (LPSTR)All.c_str();
			tvinsert.item.cchTextMax = All.length();
			NodesSend_ID [NodesSend_Num] = AddItem (tvinsert, hTreeSend);
			NodesSend_String [NodesSend_Num] = All;
			NodesSend_Num++;
		}

		TV_INSERTSTRUCT tvinsert2;
		tvinsert2.hParent = GetTreeItemByText (All);
		tvinsert2.hInsertAfter = TVI_LAST;
		tvinsert2.item.pszText = (LPSTR)Packet.c_str();
		tvinsert2.item.cchTextMax = Packet.length() + 1;
		tvinsert2.item.mask = TVIF_TEXT;
		AddItem (tvinsert2, hTreeSend);

	}
	else
	{
		Packet = ToStringArray (&SendStructP->Data->Type, SendStructP->Data->Size);
		
		//Don't Log
		short PacketID = SendStructP->Data->Type;
		int NumChanges = SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETCOUNT , 0, 0);
		
		for (int i = 0; i < NumChanges; i++)
		{
			SendMessage (GetDlgItem(hRecv, IDC_CHANGESLIST2), LB_GETTEXT , i, (LPARAM)Buffer);
			PACKETCHANGE pc = GetChange (Buffer);
			if ((pc.PacketID == PacketID) && (pc.Change == "D"))
				return;
		}

		if (Packet.length() < 4) {
			MessageBox(0, "Small packet",0,0);
			return;
		}

		//Create Header
		string All = Packet.substr (0, 2) + Packet.substr(3, 2);

		//If already exists in a TreeView
		if (!ItemRootExists (All))
		{
			TV_INSERTSTRUCT tvinsert;

			tvinsert.hParent = NULL;
			tvinsert.hInsertAfter = TVI_ROOT;
			tvinsert.item.mask = TVIF_TEXT; 
			tvinsert.item.pszText = (LPSTR)All.c_str();
			tvinsert.item.cchTextMax = All.length();
			NodesRecv_ID [NodesRecv_Num] = AddItem (tvinsert, hTreeRecv);
			NodesRecv_String [NodesRecv_Num] = All;
			NodesRecv_Num++;
		}

		TV_INSERTSTRUCT tvinsert2;
		tvinsert2.hParent = GetTreeItemByText (All);
		tvinsert2.hInsertAfter = TVI_LAST;
		tvinsert2.item.pszText = (LPSTR)Packet.c_str();
		tvinsert2.item.cchTextMax = Packet.length();
		tvinsert2.item.mask = TVIF_TEXT;
		AddItem (tvinsert2, hTreeRecv);
	}
}
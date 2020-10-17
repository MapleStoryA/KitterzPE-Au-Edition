#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <string>
#include <Commctrl.h>
using namespace std;

#include "Send.h"
#include "Recv.h"
#include "Resource.h"
#include "Packet.h"
#include "Bin.h"
#include "PEraser.h"


#pragma comment(lib, "comctl32.lib")

//-------------------------------------------------------------------------

HWND hTreeSend;
HWND hTreeRecv;

HWND hSend;
HWND hRecv;

char buf [1024];
BOOL SpamOn = FALSE;

//---------------------------------------------------------------------

//Main Callback procedure
BOOL CALLBACK SendDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(uMsg) 
	{

	case WM_INITDIALOG:		
		hTreeSend = GetDlgItem (hWnd, IDC_PACKETTREESEND);
		hSend = hWnd;
		InitCommonControls();

		SetDlgItemText (hWnd, IDC_EPACKET, (LPCSTR)"30 00 02 00 45 45 00");
		SetDlgItemText (hWnd, IDC_REPLACESTRING, (LPCSTR)"30 00 02 00 45 45 00");
		SetDlgItemText (hWnd, IDC_DELAY, (LPCSTR)"300");
		break;

	case WM_NOTIFY:
	{
		switch(LOWORD(wParam))
		{
			case IDC_PACKETTREESEND:
				if(((LPNMHDR)lParam)->code == NM_DBLCLK) 
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeSend);
					
					if (Selected == NULL)
						break;

					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = TRUE;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREESEND,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (!Tmp->ItemRootExists (Text))
						SetDlgItemText (hWnd, IDC_EPACKET, (LPCSTR)Text);

					delete Tmp;
				}
				break;
		}
		break;
	}
	break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			//--------------------------------------------------------

			case IDC_CLEAR:
				TreeView_DeleteItem (hTreeSend, TVI_ROOT);
				NodesSend_Num = 0;
				SetDlgItemText (hWnd, IDC_PACKETREPLACESTRING2, (LPCSTR)"FF 00 00 00 00 00 00");
				break;

			case IDC_REMOVE:
				{
					int TheOne = SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST), LB_GETCURSEL , 0, 0);
					
					if (TheOne == -1)
						break;
					
					SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST), LB_DELETESTRING , TheOne, 0);
				}
				break;

			//--------------------------------------------------------

			case IDC_BSENDPACKET:
				GetDlgItemText (hWnd, IDC_EPACKET, buf, 1024);
			
				if (SpamOn)
				{
					SpamOn = FALSE;
					SetDlgItemText (hWnd, IDC_BSENDPACKET, (LPCSTR)"Send Packet");
				}
				else
				{
					if (SendMessage(GetDlgItem(hWnd, IDC_CHECKBOX1), BM_GETCHECK, 0, 0) == TRUE)
					{
						SpamOn = TRUE;
						SpamPacket (buf);
						SetDlgItemText (hWnd, IDC_BSENDPACKET, (LPCSTR)"STOP");
					}
					else
						SendPacket (buf);
				}
				break;

			//--------------------------------------------------------

			case IDC_DONTLOG:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeSend);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = TRUE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREESEND,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (11, 2).c_str());
						sprintf (buf, "%02X - D", ID);
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

			case IDC_BLOCK:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeSend);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = TRUE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREESEND,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (11, 2).c_str());
						sprintf (buf, "%02X - B", ID);
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

			case IDC_REPLACE:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeSend);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = TRUE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREESEND,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (11, 2).c_str());
						GetDlgItemText (hWnd, IDC_REPLACESTRING, buf, 1024);
						
						string Temp = buf;
						replaceAll(Temp, " ", "");

						sprintf (buf, "%02X - R - %s - %02i", ID, Temp.c_str(), (Temp.length() / 2));
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return 0;
}

//-------------------------------------------------------------------------

//Main Callback procedure
BOOL CALLBACK RecvDlg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(uMsg) 
	{

	case WM_INITDIALOG:		
		hTreeRecv = GetDlgItem (hWnd, IDC_PACKETTREERECV);
		hRecv = hWnd;
		InitCommonControls();
		break;

	case WM_NOTIFY:
	{
		switch(LOWORD(wParam))
		{
			case IDC_PACKETTREERECV:
				if(((LPNMHDR)lParam)->code == NM_DBLCLK) 
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeRecv);
					
					if (Selected == NULL)
						break;

					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = FALSE;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREERECV,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (!Tmp->ItemRootExists (Text))
						SetDlgItemText (hWnd, IDC_PACKETINFO, (LPCSTR)Text);

					delete Tmp;
				}
				break;
		}
		break;
	}
	break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			//--------------------------------------------------------

			case IDC_CLEAR2:
				TreeView_DeleteItem (hTreeRecv, TVI_ROOT);
				NodesRecv_Num = 0;
				break;

			case IDC_REMOVE2:
				{
					int TheOne = SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST2), LB_GETCURSEL , 0, 0);
					
					if (TheOne == -1)
						break;

					SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST2), LB_DELETESTRING , TheOne, 0);
				}
				break;

			//--------------------------------------------------------

			case IDC_DONTLOG2:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeRecv);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = FALSE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREERECV,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (0, 2).c_str());
						sprintf (buf, "%02X - D", ID);
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST2), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

			case IDC_BLOCK2:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeRecv);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = FALSE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREERECV,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (0, 2).c_str());
						sprintf (buf, "%02X - B", ID);
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST2), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

			case IDC_REPLACE2:
				{
					char Text [1024];
					HTREEITEM Selected = TreeView_GetSelection (hTreeRecv);
					
					PacketEdit* Tmp = new PacketEdit;
					Tmp->isSend = FALSE;

					if (Selected == NULL)
						break;

					TV_ITEM Item;
					Item.mask = TVIF_TEXT;
					Item.pszText = Text;
					Item.cchTextMax = 1024;
					Item.hItem = Selected;

					SendDlgItemMessage(hWnd, IDC_PACKETTREERECV,
						TVM_GETITEM, TVGN_CARET, (LPARAM)&Item);

					if (Tmp->ItemRootExists (Text))
					{
						BYTE ID = HexStrToInt (((string)Text).substr (0, 2).c_str());
						GetDlgItemText (hWnd, IDC_PACKETREPLACESTRING2, buf, 1024);
						
						string Temp = buf;
						replaceAll(Temp, " ", "");

						sprintf (buf, "%02X - R - %s - %02i", ID, Temp.c_str(), (Temp.length() / 2));
						SendMessage (GetDlgItem(hWnd, IDC_CHANGESLIST2), LB_ADDSTRING , 0, (LPARAM)buf);
					}
				}
			break;

			//--------------------------------------------------------

		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return 0;
}

//--------------------------------------------------------------------

DWORD WINAPI Send (HMODULE hModule)
{
	//GetPacketAddysSend();
	//PacketSendHook ();
	//DialogBox(hModule, MAKEINTRESOURCE(IDD_SEND), NULL, (DLGPROC)SendDlg);
	return 1;
}

DWORD WINAPI Recv (HMODULE hModule)
{
	PacketRecvHook();
	DialogBox(hModule, MAKEINTRESOURCE(IDD_RECV), NULL, (DLGPROC)RecvDlg);
	PEraser *pRaser = new PEraser();
	pRaser->Erase((void*)hModule);
	return 1;
}

//--------------------------------------------------------------------

//Entry Point into program.
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Send, hModule, 0, NULL);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Recv, hModule, 0, NULL);
		break;
	}
	return TRUE;
}

//-------------------------------------------------------------------------
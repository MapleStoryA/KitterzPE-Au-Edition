#include "Base.h"

class PacketEdit
{
public:
	SendStruct* SendStructP;
	BOOL isSend;

	void Replace (BYTE* Bytes, INT Length);
	void Replace (string ToReplace, INT Length);
	void Block ();

	void CheckChanges ();
	void AddToTreeView (HWND hTree);
	BOOL ItemRootExists (string ToCheck);

private:
	HTREEITEM AddItem (TV_INSERTSTRUCT tvinsert, HWND hWnd);
	HTREEITEM GetTreeItemByText (string ToSearch);
	void SavePacket (string Packet);
	
};

void DCAll (BOOL* State);
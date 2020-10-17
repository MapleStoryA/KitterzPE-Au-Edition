struct PACKET
{ 
	short Size;
	BYTE Type;
	BYTE SubType;
	byte * Data;
};

struct SendStruct 
{
	DWORD ReturnAddy;
	PACKET*	Data;
};

typedef DWORD (__stdcall *SENDPACKET)(PACKET *Buffer);

extern HWND hTreeSend;
extern HWND hTreeRecv;
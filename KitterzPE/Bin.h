#define MAX_NODES 100
#define MAX_SPECIAL 20

struct PACKETCHANGE
{
	BYTE PacketID;
	string Change;
	string Packet;
	int Length;
};

string& replaceAll(string& context, const string& from, const string& to);
int HexStrToInt(const char* hexStr);
string ToStringArray (LPVOID PacketData, INT Length);
string ReverseBytes (string StringBytes);
char* ToBytesArray (string HexString);

void SpamPacket (string Packet);
PACKETCHANGE GetChange (string Change);

//-----------------------------------------------

extern int NodesSend_Num;
extern int NodesRecv_Num;
extern string NodesSend_String [MAX_NODES];
extern string NodesRecv_String [MAX_NODES];
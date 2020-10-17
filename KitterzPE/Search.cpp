#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <string>
using namespace std;

#include "Bin.h"

//------------------------------------------------------------------

bool Check(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(; *szMask; ++szMask, ++pData, ++bMask)
        if(*szMask == 'x' && *pData != *bMask ) 
            return false;
	
    return (*szMask) == NULL;
}

//------------------------------------------------------------------

DWORD FindPattern (string Bytes, DWORD dwOffset, bool isPointer)
{
	replaceAll(Bytes, " ", "");

	char AddyBytes [100] = "";
	char mask [60] = "";

	for (unsigned i = 0, pos = 0; i <  Bytes.length(); i+=2, pos++)
	{
		string SendBuffer = Bytes.substr(i, 2).c_str();

		if (!strcmp (SendBuffer.c_str(), "??"))
		{
			AddyBytes [pos] = 0;
			strcat (mask, "?");
		}
		else
		{
			AddyBytes [pos] = (unsigned char)strtol (SendBuffer.c_str(), 0, 16);
			strcat (mask, "x");
		}
	}

	DWORD Addy = NULL;

	//Get Addy
    for(DWORD i=0; i < 0x01FFFFFF; i++)
	{
		if( Check ((BYTE*)( 0x00400000 + i ), (BYTE *)AddyBytes, mask) )
		{
			Addy = (DWORD)(0x00400000 + i + dwOffset);
			break;
		}
	}

	if (isPointer)
		Addy = *(DWORD*)Addy;

    return Addy;
}

//------------------------------------------------------------------
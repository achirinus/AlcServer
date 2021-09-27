#include "AlcString.h"

namespace Alc
{
	int GetStringSize(const wchar_t* sIn)
	{
		int nRet = 0;
		if (!sIn) return nRet;
		while (*sIn++) nRet++;
		return nRet;
	}
	
	wchar_t* CopyString(const wchar_t* sIn)
	{
		if (!sIn) return nullptr;
		int nSize = GetStringSize(sIn);
		wchar_t* sOut = new wchar_t[nSize + 1];
		wchar_t* sRet = sOut;
		while (*sOut++ = *sIn++);
		return sRet;
	}
}
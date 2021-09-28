#include "AlcString.h"
#include "AlcMemory.h"

namespace Alc
{
	int GetStringSize(const u16* sIn)
	{
		int nRet = 0;
		if (!sIn) return nRet;
		while (*sIn++) nRet++;
		return nRet;
	}
	
	u16* CopyString(const u16* sIn)
	{
		if (!sIn) return nullptr;
		int nSize = GetStringSize(sIn);
		u16* sOut = ALC_PUSH_TEMP_ARRAY(u16, nSize + 1);
		u16* sRet = sOut;
		while (*sOut++ = *sIn++);
		return sRet;
	}

	void ToSmallString(const u16* sIn, u8* sOut)
	{
		do
		{
			u16 cWide = *sIn;
			u8* aSmall = (u8*)&cWide;
			*sOut++ = aSmall[0];
		} while (*sIn++);
	}
}
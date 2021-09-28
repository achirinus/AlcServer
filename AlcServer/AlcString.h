#pragma once

#include "AlcDefines.h"

struct AlcString
{
	u16* m_sBuffer = nullptr;
	s32 m_nSize = -1;
};

namespace Alc
{
	s32 GetStringSize(const u16* sIn);
	u16* CopyString(const u16* sIn);

	void ToSmallString(const u16* sIn, u8* sOut);
}

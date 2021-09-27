#pragma once

struct AlcString
{
	wchar_t* m_sBuffer;
	int m_nSize;
};

namespace Alc
{
	int GetStringSize(const wchar_t* sIn);
	wchar_t* CopyString(const wchar_t* sIn);
}

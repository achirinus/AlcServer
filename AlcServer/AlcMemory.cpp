#include "AlcMemory.h"
#include <cstdlib>
#include <cassert>

static AlcMemoryArena g_aArenas[3];

namespace Alc
{
	void* _PushStruct(s32 nSize, AlcMemoryArenaType eType, int nCount)
	{
		assert(eType < AlcArena_Count);
		AlcMemoryArena& Arena = g_aArenas[eType];
		assert((Arena.m_nSize - Arena.m_nCursor) >= (nSize * nCount));
		assert(Arena.m_pMem);

		void* pRet = Arena.m_pMem + Arena.m_nCursor;
		Arena.m_nCursor += (nSize * nCount);

		return pRet;
	}

	void InitMemory()
	{
		AlcMemoryArena& FrameArena = g_aArenas[AlcArena_Frame];

		FrameArena.m_nCursor = 0;
		FrameArena.m_nSize = 1024 * 1024;
		FrameArena.m_pMem = (u8*)malloc(FrameArena.m_nSize);

		AlcMemoryArena& PermanentArena = g_aArenas[AlcArena_Permanent];
		PermanentArena.m_nCursor = 0;
		PermanentArena.m_nSize = 1024 * 1024;
		PermanentArena.m_pMem = (u8*)malloc(PermanentArena.m_nSize);

		AlcMemoryArena& TempArena = g_aArenas[AlcArena_Temp];
		TempArena.m_nCursor = 0;
		TempArena.m_nSize = 1024 * 1024;
		TempArena.m_pMem = (u8*)malloc(TempArena.m_nSize);
	}

	void ClearArena(AlcMemoryArenaType eType)
	{
		assert(eType < AlcArena_Count);
		AlcMemoryArena& Arena = g_aArenas[eType];
		assert(Arena.m_pMem);

		for (int i = 0; i < Arena.m_nSize; i++)
		{
			Arena.m_pMem[i] = 0;
		}
		Arena.m_nCursor = 0;
	}

	AlcMemoryArena& GetArena(AlcMemoryArenaType eType)
	{
		assert(g_aArenas[eType].m_pMem);
		return g_aArenas[eType];
	}
}
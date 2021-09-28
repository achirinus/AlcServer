#pragma once

#include "AlcDefines.h"

#define ALC_PUSH_STRUCT(type, arena) (type*)Alc::_PushStruct(sizeof(type), arena);
#define ALC_PUSH_STRUCT_ARRAY(type, arena, typeCount) (type*)Alc::_PushStruct(sizeof(type), arena, (typeCount));
#define ALC_PUSH_FRAME(type) ALC_PUSH_STRUCT(type, AlcArena_Frame)
#define ALC_PUSH_PERMANENT(type) ALC_PUSH_STRUCT(type, AlcArena_Permanent)
#define ALC_PUSH_TEMP(type) ALC_PUSH_STRUCT(type, AlcArena_Temp)
#define ALC_PUSH_FRAME_ARRAY(type, typeCount) ALC_PUSH_STRUCT_ARRAY(type, AlcArena_Frame, (typeCount))
#define ALC_PUSH_PERMANENT_ARRAY(type, typeCount) ALC_PUSH_STRUCT_ARRAY(type, AlcArena_Permanent, (typeCount))
#define ALC_PUSH_TEMP_ARRAY(type, typeCount) ALC_PUSH_STRUCT_ARRAY(type, AlcArena_Temp, (typeCount))

enum AlcMemoryArenaType
{
	AlcArena_Frame,
	AlcArena_Permanent,
	AlcArena_Temp,
	
	AlcArena_Count
};

struct AlcMemoryArena
{
	u8* m_pMem = nullptr;
	s32 m_nSize = 0;
	s32 m_nCursor = 0;
};

namespace Alc
{
	void* _PushStruct(s32 nSize, AlcMemoryArenaType eType, int nCount = 1);

	void InitMemory();
	void ClearArena(AlcMemoryArenaType eType);
	AlcMemoryArena& GetArena(AlcMemoryArenaType eType);
}
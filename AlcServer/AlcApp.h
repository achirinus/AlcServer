#pragma once
#include <vector>
#include "AlcSql.h"
#include "AlcDefines.h"

struct AlcAccount
{
	u16* m_sEmail;
	u16* m_sPass;
	u16* m_sFirstName;
	u16* m_sLastName;
};

struct AlcApp
{
	bool m_bShowMemoryOverlay;

	AlcAccount* m_aAccounts;
	s32 m_nNumAccounts;
};

namespace Alc
{
	AlcApp* GetApp();
	void LoadAccounts();
}
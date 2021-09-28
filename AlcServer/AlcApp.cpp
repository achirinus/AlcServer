#include "AlcApp.h"
#include "AlcSql.h"
#include "AlcMemory.h"
#include "AlcLog.h"
#include "AlcString.h"

static AlcApp* g_pApp;

namespace Alc
{
	AlcApp* GetApp()
	{
		if (!g_pApp)
		{
			g_pApp = ALC_PUSH_PERMANENT(AlcApp);
			g_pApp->m_aAccounts = nullptr;
			g_pApp->m_bShowMemoryOverlay = false;
			g_pApp->m_nNumAccounts = 0;
		}
		return g_pApp;
	}

	void LoadAccounts()
	{
		s32 nCount = 0;
		AlcStatement CountUsersRead;
		if (CreateSqlStatement(&CountUsersRead, (wchar_t*)L"SELECT COUNT(*) FROM Users"))
		{
			
			while (IsSqlStatementValid(&CountUsersRead))
			{
				int nCol = 1;
				if (!GetIntFromStatement(&CountUsersRead, nCol++, &nCount))
				{
					LogError("Failed to get users count");
					continue;
				}
			}
			CloseSqlStatement(&CountUsersRead);
		}

		if (nCount <= 0) return;

		AlcApp* pApp = Alc::GetApp();
		pApp->m_aAccounts = ALC_PUSH_PERMANENT_ARRAY(AlcAccount, nCount);
		pApp->m_nNumAccounts = nCount;

		AlcStatement UsersRead;
		int nTempAcount = 0;
		if (CreateSqlStatement(&UsersRead, (wchar_t*)L"SELECT * FROM Users"))
		{
			int nID;
			u16 sEmail[256];
			u16 sPass[256];
			u16 sFirstName[256];
			u16 sSecondName[256];

			while (IsSqlStatementValid(&UsersRead))
			{
				int nCol = 1;
				if (!GetIntFromStatement(&UsersRead, nCol++, &nID))
				{
					LogError("Failed to get ID");
					continue;
				}
				if (!GetStringFromStatement(&UsersRead, nCol++, sEmail, 256))
				{
					LogError("Failed to get Email");
					continue;
				}
				if (!GetStringFromStatement(&UsersRead, nCol++, sPass, 256))
				{
					LogError("Failed to get Pass");
					continue;
				}
				if (!GetStringFromStatement(&UsersRead, nCol++, sFirstName, 256))
				{
					LogError("Failed to get First Name");
					continue;
				}
				if (!GetStringFromStatement(&UsersRead, nCol++, sSecondName, 256))
				{
					LogError("Failed to get Last Name");
					continue;
				}
				pApp->m_aAccounts[nTempAcount].m_sEmail = CopyString(sEmail);
				pApp->m_aAccounts[nTempAcount].m_sPass = CopyString(sPass);
				pApp->m_aAccounts[nTempAcount].m_sFirstName = CopyString(sFirstName);
				pApp->m_aAccounts[nTempAcount].m_sLastName = CopyString(sSecondName);
				nTempAcount++;
			}
			CloseSqlStatement(&UsersRead);
		}
	}
}
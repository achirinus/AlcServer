// AlcServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "AlcSql.h"
#include "AlcLog.h"

int main()
{
	

	AlcStatement TestRead;
	if (CreateSqlStatement(&TestRead, (wchar_t*)L"SELECT * FROM Users"))
	{
		int nID;
		wchar_t sEmail[256];
		wchar_t sPass[256];
		wchar_t sFirstName[256];
		wchar_t sSecondName[256];
		while (IsSqlStatementValid(&TestRead))
		{
			int nCol = 1;
			if (!GetIntFromStatement(&TestRead, nCol++, &nID))
			{
				LogError("Failed to get ID");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sEmail, 256))
			{
				LogError("Failed to get Email");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sPass, 256))
			{
				LogError("Failed to get Pass");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sFirstName, 256))
			{
				LogError("Failed to get First Name");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sSecondName, 256))
			{
				LogError("Failed to get Last Name");
				continue;
			}
			LogInfo("Id: %d", nID);
			LogInfo("Email: %ls", sEmail);
			LogInfo("Pass: %ls", sPass);
			LogInfo("First Name: %ls", sFirstName);
			LogInfo("Last Name: %ls", sSecondName);
		}
		CloseSqlStatement(&TestRead);
	}

    return 0;
}


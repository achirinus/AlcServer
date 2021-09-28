#pragma once
#include "AlcDefines.h"

struct AlcConnection
{
	void* m_pHandle = nullptr;
};

AlcConnection* GetDbConnection();

struct AlcStatement
{
	void* m_pHandle;
};

bool CreateSqlStatement(AlcStatement* OutStatement, wchar_t* sStatement);

void CloseSqlStatement(AlcStatement* InStatement);

bool IsSqlStatementValid(AlcStatement* InStatement);

bool GetIntFromStatement(AlcStatement* InStatement, int nCol, int* nOutInt);

bool GetStringFromStatement(AlcStatement* InStatement, int nCol, u16* sOutStr, int nOutCount);

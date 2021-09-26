#pragma once

struct AlcConnection
{
	void* pHandle = nullptr;
};

AlcConnection* GetDbConnection();

struct AlcStatement
{
	void* pHandle;
};

bool CreateSqlStatement(AlcStatement* OutStatement, wchar_t* sStatement);

void CloseSqlStatement(AlcStatement* InStatement);

bool IsSqlStatementValid(AlcStatement* InStatement);

bool GetIntFromStatement(AlcStatement* InStatement, int nCol, int* nOutInt);

bool GetStringFromStatement(AlcStatement* InStatement, int nCol, wchar_t* sOutStr, int nOutCount);

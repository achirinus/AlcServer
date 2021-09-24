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

void CreateSqlStatement(AlcStatement* OutStatement, wchar_t* sStatement);



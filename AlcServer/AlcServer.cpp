// AlcServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "AlcSql.h"

int main()
{
    
	SQLCHAR sVersionReturn[SQL_RESULT_LEN];
	SQLINTEGER nVersion = -1;

	SQLUSMALLINT nColumnNumber = 1;
	while (SQLFetch(hStatement) == SQL_SUCCESS)
	{
		SQLGetData(hStatement, nColumnNumber, SQL_CHAR, sVersionReturn, SQL_RESULT_LEN, &nVersion);

		printf("Version query result: %u\n", nVersion);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hStatement);
	SQLDisconnect(hConnection);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnvironment);
	SQLFreeHandle(SQL_HANDLE_DBC, hConnection);
	
	AlcStatement TestRead;
	CreateSqlStatement(&TestRead, L"")

	

    return 0;
}


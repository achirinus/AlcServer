#include "AlcSql.h"

//TODO Make my own Windows.h with just the basics
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include "AlcLog.h"

static AlcConnection* s_pConnection = nullptr;

#define SQL_RETURN_CODE_LEN 1024

static 
SQLWCHAR* GetSqlErrorDetails()
{
	SQLWCHAR sOutState[10];
	static SQLWCHAR sOutText[1024];
	SQLINTEGER nNativeErr;
	SQLSMALLINT nTextLength;
	SQLGetDiagRec(SQL_HANDLE_DBC, hConnection, 1, sOutState, &nNativeErr, sOutText, 1024, &nTextLength);
}

AlcConnection* GetDbConnection()
{
	if (s_pConnection) return s_pConnection;

	SQLHANDLE hConnection = NULL;
	SQLHANDLE hEnvironment = NULL;
	SQLWCHAR sRetString[SQL_RETURN_CODE_LEN];

	//TODO DB info should be in some ini file
	SQLWCHAR* sConn = (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost;DATABASE=alc;UID=alcpower;PWD=hemorel-craps;";

	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnvironment);
	if (nRet != SQL_SUCCESS)
	{
		LogError("Failed to alloc SQL environment handle");
		return nullptr;
	}
	nRet = SQLSetEnvAttr(hEnvironment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (nRet != SQL_SUCCESS)
	{
		LogError("Failed to set SQL ENV handle attribute (ODBC Version)");
		SQLFreeHandle(SQL_HANDLE_ENV, hEnvironment);

		return nullptr;
	}

	nRet = SQLAllocHandle(SQL_HANDLE_DBC, hEnvironment, &hConnection);
	if (nRet != SQL_SUCCESS)
	{
		LogError("Failed to alloc SQL Connection handle");
		SQLFreeHandle(SQL_HANDLE_ENV, hEnvironment);

		return nullptr;
	}
	nRet = SQLDriverConnect(hConnection, NULL, sConn, SQL_NTS, sRetString, SQL_RETURN_CODE_LEN, NULL, SQL_DRIVER_NOPROMPT);
	switch (nRet)
	{
	case SQL_SUCCESS:
	{
		LogInfo("Connection to database was successfull");
	}break;
	case SQL_SUCCESS_WITH_INFO:
	{
		SQLWCHAR* sDetails = GetSqlErrorDetails();
		LogInfo("Connection to database was successfull");
		LogWarning("%ls", sDetails);
	}break;
	case SQL_INVALID_HANDLE:
	{
		LogError("Invalid Database handle!");
		SQLFreeHandle(SQL_HANDLE_ENV, hEnvironment);
		SQLFreeHandle(SQL_HANDLE_DBC, hConnection);

		return nullptr;
	}break;
	case SQL_ERROR:
	{
		SQLWCHAR* sDetails = GetSqlErrorDetails();

		LogError("Database connection error!");
		LogError("%ls", sDetails);
		SQLFreeHandle(SQL_HANDLE_ENV, hEnvironment);
		SQLFreeHandle(SQL_HANDLE_DBC, hConnection);

		return nullptr;
	}break;
	default: return nullptr;
	}

	return s_pConnection;
}

void CreateSqlStatement(AlcStatement* OutStatement, wchar_t* sStatement)
{
	if (!OutStatement) return;

	nRet = SQLAllocHandle(SQL_HANDLE_STMT, hConnection, &OutStatement->pHandle);
	if (nRet != SQL_SUCCESS)
	{
		LogError("Failed to alloc SQL Statement handle");
	}

	nRet = SQLExecDirect(OutStatement->pHandle, sStatement, SQL_NTS);
	if (nRet != SQL_SUCCESS)
	{
		LogError("SQL statement failed: %ls. Error: %ls", sStatement);
	}
}

void CloseStatement(AlcStatement* InStatement)
{
	if (!InStatement) return;
	SQLFreeHandle(SQL_HANDLE_STMT, InStatement->pHandle);
}
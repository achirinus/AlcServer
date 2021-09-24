#include "AlcSql.h"

//TODO Make my own Windows.h with just the basics
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include "AlcLog.h"

static AlcConnection* s_pConnection = nullptr;

#define SQL_RETURN_CODE_LEN 1024

static 
SQLWCHAR* GetSqlConErrorDetails(AlcConnection* pCon)
{
	SQLWCHAR sOutState[10];
	static SQLWCHAR sOutText[1024];
	
	for (int i = 0; i < 1024; i++)
	{
		sOutText[i] = 0;
	}

	SQLINTEGER nNativeErr;
	SQLSMALLINT nTextLength;
	if (pCon)
	{
		SQLGetDiagRec(SQL_HANDLE_DBC, pCon->pHandle, 1, sOutState, &nNativeErr, sOutText, 1024, &nTextLength);
	}
	return sOutText;
}

static
SQLWCHAR* GetSqlStatementErrorDetails(AlcStatement* pStatement)
{
	SQLWCHAR sOutState[10];
	static SQLWCHAR sOutStatement[1024];

	for (int i = 0; i < 1024; i++)
	{
		sOutStatement[i] = 0;
	}

	SQLINTEGER nNativeErr;
	SQLSMALLINT nTextLength;
	if (pStatement)
	{
		SQLGetDiagRec(SQL_HANDLE_STMT, pStatement->pHandle, 1, sOutState, &nNativeErr, sOutStatement, 1024, &nTextLength);
	}
	return sOutStatement;
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
		s_pConnection = new AlcConnection;
		s_pConnection->pHandle = hConnection;
		LogInfo("Connection to database was successfull");
	}break;
	case SQL_SUCCESS_WITH_INFO:
	{
		s_pConnection = new AlcConnection;
		s_pConnection->pHandle = hConnection;
		SQLWCHAR* sDetails = GetSqlConErrorDetails(s_pConnection);
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
		AlcConnection temp;
		temp.pHandle = hConnection;
		SQLWCHAR* sDetails = GetSqlConErrorDetails(&temp);

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

bool CreateSqlStatement(AlcStatement* OutStatement, wchar_t* sStatement)
{
	if (!OutStatement) return false;
	AlcConnection* pCon = GetDbConnection();

	if (!pCon) return false;

	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_STMT, pCon->pHandle, &OutStatement->pHandle);
	if (nRet != SQL_SUCCESS)
	{
		LogError("Failed to alloc SQL Statement handle");
		return false;
	}

	nRet = SQLExecDirect(OutStatement->pHandle, sStatement, SQL_NTS);
	if (nRet != SQL_SUCCESS)
	{
		LogError("SQL statement failed: %ls. Error: %ls", sStatement);
		return false;
	}
	return true;
}

void CloseSqlStatement(AlcStatement* InStatement)
{
	if (!InStatement) return;
	SQLFreeHandle(SQL_HANDLE_STMT, InStatement->pHandle);
}

bool IsSqlStatementValid(AlcStatement* InStatement)
{
	if (!InStatement)return false;
	return (SQLFetch(InStatement->pHandle) == SQL_SUCCESS);
}

bool GetIntFromStatement(AlcStatement* InStatement, int nCol, int* nOutInt)
{
	if (!InStatement) return false;
	if (SQLGetData(InStatement->pHandle, nCol, SQL_C_SLONG, nOutInt, 0, nullptr) != SQL_SUCCESS)
	{
		wchar_t* sDetails = GetSqlStatementErrorDetails(InStatement);
		LogError("%ls", sDetails);
		return false;
	}
	return true;
}

bool GetStringFromStatement(AlcStatement* InStatement, int nCol, wchar_t* sOutStr, int nOutCount)
{
	if (!InStatement) return false;

	if (SQLGetData(InStatement->pHandle, nCol, SQL_C_WCHAR, sOutStr, nOutCount, nullptr) != SQL_SUCCESS)
	{
		wchar_t* sDetails = GetSqlStatementErrorDetails(InStatement);
		LogError("%ls", sDetails);
		return false;
	}

	return true;
}


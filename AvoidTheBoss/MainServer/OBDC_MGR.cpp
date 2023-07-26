#include "pch.h"
#include "OBDC_MGR.h"
#include "Session.h"


void USER_DB_MANAGER::AllocateHandles()
{
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
    {
        // ODBC 드라이버 버전 명시
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
        {
            // 연결 핸들러 할당
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
        }
    }
}

void USER_DB_MANAGER::ConnectDataSource(const SQLWCHAR* obdc)
{
    SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    retcode = SQLConnect(hdbc, (SQLWCHAR*)obdc, SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
}

void USER_DB_MANAGER::ExecuteStatementDirect(const SQLWCHAR* sql)
{
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
    {
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)sql, SQL_NTS);
    }
    else 
    {
        show_error(hdbc, SQL_HANDLE_DBC, retcode);
    }
}

void USER_DB_MANAGER::ExecuteStatement(const SQLWCHAR* exec)
{
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
    {
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS); 
    }
    else 
    {
        show_error(hdbc, SQL_HANDLE_DBC, retcode);
    }
   
  
}

void USER_DB_MANAGER::RetrieveResult()
{
    // 데이터베이스로 부터 값을 받아와 변수에 바인드
    // Bind columns 1, 2, and 3 
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
    {
        retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &user_cid, 4, &cb_cid);
        // 실제 데이터를 꺼낸다
        // Fetch and print each row of data. On an error, display a message and exit.  
        for (int i = 0; ; i++) {
            retcode = SQLFetch(hstmt);
            if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
                show_error(hstmt, SQL_HANDLE_STMT, retcode);
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
            {   
                break;
            }
            else
                break;
        }
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
        {
            SQLCancel(hstmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        }
    }
    else 
    {
        show_error(hstmt, SQL_HANDLE_STMT, retcode);
    }
}

void USER_DB_MANAGER::DisconnectDataSource()
{
    if (hstmt) 
    {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = NULL;
    }
    SQLDisconnect(hdbc);
    if (hdbc) 
    {
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        hdbc = NULL;
    }
    if (henv) 
    {
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = NULL;
    }
}

void USER_DB_MANAGER::show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER iError;
    WCHAR wszMessage[1000];
    WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
    if (retcode == SQL_INVALID_HANDLE) 
    {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
        (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) 
    {
        // Hide data truncated..
        if (wcsncmp(wszState, L"01004", 5)) 
        {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}

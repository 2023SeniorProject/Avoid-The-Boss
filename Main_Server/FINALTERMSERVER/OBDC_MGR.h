#pragma once
#include <windows.h>  
#include <iostream>  
#include <sqlext.h>  
class USER_DB_MANAGER
{
    SQLHENV henv = 0;
    SQLHDBC hdbc = 0;
    SQLHSTMT hstmt = 0;
    SQLRETURN retcode = 0;
    SQLLEN cb_cid = 0;
public:
    SQLINTEGER user_cid = -1;
    USER_DB_MANAGER() {};
    ~USER_DB_MANAGER() {};
    void AllocateHandles();
    void ConnectDataSource(const SQLWCHAR* obdc);
    void ExecuteStatementDirect(const SQLWCHAR* sql);
    void ExecuteStatement(const SQLWCHAR* exec);
    void RetrieveResult();
    void DisconnectDataSource();
    void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};


#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <tchar.h>
#import "C:/Program Files/Common Files/System/ado/msado15.dll" no_namespace rename("EOF","adoEOF")
#pragma warning(default:4146)

#define ADOERROR_USER_EXISTED -105		//用户已存在
#define ADOERROR_USER_ERROR -106		//用户名或密码不正确
#define ADOERROR_USER_BANED -107		//用户已被禁用
#define ADOERROR_USER_NOREST -108		//用户时长不足
#define ADOERROR_USER_BANDED -109		//用户未解绑
#define ADOERROR_CARD_NOT_EXISTED -110		//卡不存在
#define ADOERROR_CARD_USED -111		//卡已使用

#define ADOERROR_INSERT_USER_ERROR -405
#define ADOERROR_GET_REST_TIME_ERROR -406
#define ADOERROR_LOGIN_USER_ERROR -407
#define ADOERROR_RECHARGE_USER_ERROR -408


//using namespace adodb;
using namespace std;


class AdoConn
{
public:
	AdoConn();
	~AdoConn();

public:
	bool OpenConn(string connstr, string connUserId, string connUserPwd);
	vector<vector<string>> SelectData(string selectTxt);
	void ExecProc(string procName);
	int InsertUser(string username, string userpwd, string userqq,string userbiosid);
	int LoginUser(string username, string userpwd, string userdiskdriver);
	int RechargeUser(string username, string userpwd, string cardId);
	int GetPt(string username, string userpwd);

	string GetRestTime(string username, string userpwd);

	string GetDiskDriverID();
	string DiffToString(ULONG pDiff);

	string KsDecry(vector<BYTE> data);
private:
	_ConnectionPtr m_pConnection;//定义数据库连接对象
	_RecordsetPtr m_pRecordset;//定义记录集对象

	bool CreateConn();
	
};






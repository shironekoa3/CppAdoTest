#include "stdafx.h"
#include "AdoConn.h"


AdoConn::AdoConn()
{
	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	m_pConnection = NULL;
}


AdoConn::~AdoConn()
{
	if (NULL != m_pRecordset)
		m_pRecordset->Close();
	if (NULL != m_pConnection)
		m_pConnection->Close();
	::CoUninitialize();
}

bool AdoConn::CreateConn()
{
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))return false;
	else return true;
}

bool AdoConn::OpenConn(string connstr, string connUserId, string connUserPwd)
{
	if (NULL == m_pConnection)
		if (!CreateConn()) return false;

	_bstr_t strConnect = connstr.c_str();

	//connecting to the database server now:
	try
	{
		m_pConnection->Open(strConnect, connUserId.c_str(), connUserPwd.c_str(), NULL);
		if (NULL == m_pConnection) return false;
	}
	catch (_com_error err)
	{
		//printf("连接数据库失败!错误信息:%ls", err.ErrorMessage());
		return false;
	}
	return true;

}

vector<vector<string>> AdoConn::SelectData(string selectTxt)
{
	vector<vector<string>> result;
	if (NULL == m_pConnection) return result;
	try
	{
		if (NULL == m_pRecordset)
			m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open(selectTxt.c_str(), m_pConnection.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdText);
		m_pRecordset->MoveFirst();
		while (m_pRecordset->adoEOF == VARIANT_FALSE)
		{
			vector<string> temp;
			temp.resize(m_pRecordset->GetFields()->Count);
			for (UINT i = 0; i < temp.size(); i++)
			{
				temp[i] = (char *)(_bstr_t)m_pRecordset->GetCollect(m_pRecordset->GetFields()->GetItem((long)i)->GetName());
			}
			result.push_back(temp);
			m_pRecordset->MoveNext();//移到下一条记录
		}
	}
	catch (_com_error err)
	{
		return result;
	}
	return result;

	//③CursorType光标类型，它可以是以下值之一,请看这个枚举结构:
	/*enum CursorTypeEnum
	{
		adOpenUnspecified = -1,///不作特别指定
		adOpenForwardOnly = 0,///前滚静态光标。这种光标只能向前浏览记录集，比如用MoveNext向前滚动,这

		种方式可以提高浏览速度。但诸如BookMark, RecordCount, AbsolutePosition, AbsolutePage都不能使用
		adOpenKeyset = 1,///采用这种光标的记录集看不到其它用户的新增、删除操作，但对于更新原有记录的

		操作对你是可见的。
		adOpenDynamic = 2,///动态光标。所有数据库的操作都会立即在各用户记录集上反应出来。
		adOpenStatic = 3///静态光标。它为你的记录集产生一个静态备份，但其它用户的新增、删除、更新操

		作对你的记录集来说是不可见的。
	};*/

	//④LockType锁定类型，它可以是以下值之一，请看如下枚举结构：
	/*enum LockTypeEnum
	{
		adLockUnspecified = -1,///未指定
		adLockReadOnly = 1,///只读记录集
		adLockPessimistic = 2, 悲观锁定方式。数据在更新时锁定其它所有动作，这是最安全的锁定机制
		adLockOptimistic = 3, 乐观锁定方式。只有在你调用Update方法时才锁定记录。在此之前仍然可以做数

		据的更新、插入、删除等动作
		adLockBatchOptimistic = 4，乐观分批更新。编辑时记录不会锁定，更改、插入及删除是在批处理模式

		下完成。
	};*/

	//5.option可以取以下值 
	/*adCmdText:表明CommandText是文本命令
	adCmdTable : 表明CommandText是一个表名
	adCmdProc : 表明CommandText是一个存储过程
	adCmdUnknown : 未知*/

}

void AdoConn::ExecProc(string procName)
{
	//这里只是一个例子，执行其他存储过程在此基础上改参数就行了
	if (NULL == m_pConnection) return;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(KsDecry({ 238,97,84,98,100,91,99,1 }).c_str()), adVarChar, adParamOutput, 50, ""));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(KsDecry({ 112,56,43,57,59,50,58,86 }).c_str()), adVarChar, adParamOutput, 50, ""));

		cmmd->CommandText = _bstr_t(procName.c_str());
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		string strRet = (const char*)(_bstr_t)cmmd->Parameters->GetItem(KsDecry({ 210,141,128,142,144,135,143,73 }).c_str())->GetValue();
		
	}
	catch (_com_error err)
	{
		
	}
}

int AdoConn::InsertUser(string username,string userpwd,string userqq, string userdiskdriver)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_qq"), adVarChar, adParamInput, 10, userqq.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_diskdriver"), adVarChar, adParamInput, 50, userdiskdriver.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_createuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_INSERT_USER_ERROR;
	}
	return result;
}
int AdoConn::LoginUser(string username, string userpwd, string userdiskdriver)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_diskdriver"), adVarChar, adParamInput, 50, userdiskdriver.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_loginuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_LOGIN_USER_ERROR;
	}
	return result;
}
int AdoConn::RechargeUser(string username, string userpwd, string cardId)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_card_id"), adVarChar, adParamInput, 50, cardId.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_rechargeuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_RECHARGE_USER_ERROR;
	}
	return result;
}
int AdoConn::GetPt(string username, string userpwd)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_getpt");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = 0;
	}
	return result;
}
string AdoConn::GetRestTime(string username,string userpwd)
{
	string sResult;
	if (NULL == m_pConnection) return sResult;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_getresttime");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		ULONG uTime = cmmd->Parameters->GetItem("result")->GetValue();
		sResult = DiffToString(uTime);
	}
	catch (_com_error err)
	{
		sResult = "";
	}
	return sResult;
}

string AdoConn::GetDiskDriverID()
{
	string sResult;
	string sName;
	string cmd = "wmic diskdrive get serialnumber";
	char cResult[100] = { 0 };
	FILE* fp;
	if (NULL != (fp = _popen(cmd.c_str(), "r")))
	{
		fgets(cResult, sizeof(cResult), fp);
		sName = cResult;
		fgets(cResult, sizeof(cResult), fp);
		sResult = cResult;
		sResult = sResult.substr(0, sResult.find_first_of(" "));
		_pclose(fp);
	}
	return sResult;
}
string AdoConn::DiffToString(ULONG pDiff)
{
	string sResult;
	char cResult[255];
	UINT second = pDiff % 60;
	UINT minute = (pDiff / 60) % 60;
	UINT hour = (pDiff / 60 / 60) % 24;
	UINT day = pDiff / 60 / 60 / 24;
	sprintf_s(cResult, "%d天%02d时%02d分%02d秒", day, hour, minute, second);
	sResult = cResult;
	return cResult;
}


string AdoConn::KsDecry(vector<BYTE> data)
{
	UINT len = data.size();
	BYTE datF = data[0];
	BYTE datL = data[len - 1];
	char cResult[MAX_PATH] = {};
	for (UINT i = 0; i < len - 2; i++)
	{
		cResult[i] = data[i + 1] - datF - datL;
	}
	string sResult = cResult;
	return sResult;
}


